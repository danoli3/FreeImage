param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference = 'Stop'
$source = $PSScriptRoot
$vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
$instances = @(& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.ARM64 -format json | ConvertFrom-Json)
if ($LASTEXITCODE -or !$instances.Count) { throw 'No Visual Studio instance with ARM64 tools was found' }
$instance = $instances[0]
$major = ([version]$instance.installationVersion).Major
$generator = switch ($major) {
    17 { 'Visual Studio 17 2022' }
    18 { 'Visual Studio 18 2026' }
    default { throw "Unsupported Visual Studio major version: $major" }
}
$msvcRoot = Join-Path $instance.installationPath 'VC/Tools/MSVC'
$toolset = Get-ChildItem $msvcRoot -Directory | Sort-Object { [version]$_.Name } -Descending | Select-Object -First 1
$dumpbin = Join-Path $toolset.FullName 'bin/Hostx64/x64/dumpbin.exe'
if (!(Test-Path $dumpbin)) { throw 'Cannot find the x64-hosted dumpbin tool' }
$env:CMAKE_BUILD_PARALLEL_LEVEL = [Environment]::ProcessorCount
New-Item -ItemType Directory -Force $BuildRoot, $OutputRoot | Out-Null
$BuildRoot = (Resolve-Path $BuildRoot).Path
$OutputRoot = (Resolve-Path $OutputRoot).Path

function Invoke-CMake([string[]]$Arguments) {
    & cmake @Arguments
    if ($LASTEXITCODE) { throw "CMake failed with exit code $LASTEXITCODE" }
}
function Configure([string]$Folder, [string]$Architecture, [string]$Stage, [string[]]$Extra = @()) {
    Invoke-CMake (@('-S', $source, '-B', $Folder, '-G', $generator, '-A', $Architecture,
        "-DCMAKE_GENERATOR_INSTANCE=$($instance.installationPath)", "-DARM64X_STAGE=$Stage") + $Extra)
}
function Build([string]$Folder, [string[]]$Targets) {
    Invoke-CMake (@('--build', $Folder, '--config', $Configuration, '--target') + $Targets)
}
function Stage([string]$Folder, [string]$Destination) {
    $destinationPath = Join-Path $OutputRoot $Destination
    New-Item -ItemType Directory -Force $destinationPath | Out-Null
    Copy-Item (Join-Path $Folder "bin/$Configuration/FreeImage.dll") $destinationPath
}
$native = Join-Path $BuildRoot 'native'
$compatible = Join-Path $BuildRoot 'compatible'
$consumer = Join-Path $BuildRoot 'x64-consumer'
$probes = Join-Path $OutputRoot 'probes'
New-Item -ItemType Directory -Force $probes | Out-Null

Configure $native ARM64 Native
Build $native @('FreeImage', 'ARM64XSmoke')
Stage $native native
Copy-Item (Join-Path $native "bin/$Configuration/ARM64XSmoke.exe") (Join-Path $probes 'arm64.exe')

Configure $compatible ARM64EC Compatible
Build $compatible @('FreeImage', 'ARM64XSmoke')
Stage $compatible compatible
Copy-Item (Join-Path $compatible "bin/$Configuration/ARM64XSmoke.exe") (Join-Path $probes 'arm64ec.exe')

# Reconfigure and relink the EC objects with the actual native ARM64 inputs.
Configure $compatible ARM64EC Hybrid @("-DARM64X_NATIVE_RSP=$native/native.rsp")
Build $compatible @('FreeImage')
Stage $compatible hybrid

Configure $consumer x64 Consumer
Build $consumer @('ARM64XSmoke')
Copy-Item (Join-Path $consumer "bin/$Configuration/ARM64XSmoke.exe") (Join-Path $probes 'x64.exe')

foreach ($kind in @('native', 'compatible', 'hybrid')) {
    $dll = Join-Path $OutputRoot "$kind/FreeImage.dll"
    $headers = & $dumpbin /headers $dll
    if ($LASTEXITCODE) { throw "dumpbin failed for $kind" }
    $headers | Set-Content (Join-Path $OutputRoot "$kind-headers.txt")
    if ($kind -eq 'hybrid' -and (($headers -join "`n") -notmatch 'AA64 machine \(ARM64\).*\(ARM64X\)')) {
        throw 'The merged DLL does not identify as ARM64 + ARM64X'
    }
    & $dumpbin /exports $dll | Set-Content (Join-Path $OutputRoot "$kind-exports.txt")
    if ($LASTEXITCODE) { throw "Cannot inspect $kind exports" }
}
Copy-Item (Join-Path $native 'native.rsp') (Join-Path $OutputRoot 'native-link-inputs.rsp')
Get-ChildItem $OutputRoot -Recurse -File | Where-Object { $_.Extension -in '.dll', '.exe' } | ForEach-Object {
    '{0}  {1}' -f (Get-FileHash $_.FullName -Algorithm SHA256).Hash, [IO.Path]::GetRelativePath($OutputRoot, $_.FullName)
} | Set-Content (Join-Path $OutputRoot 'SHA256SUMS.txt')
"Built ARM64X $Configuration artifacts in $OutputRoot" | Write-Host
