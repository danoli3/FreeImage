param([Parameter(Mandatory)][string]$Artifacts)
$ErrorActionPreference = 'Stop'
if ([Runtime.InteropServices.RuntimeInformation]::OSArchitecture -ne [Runtime.InteropServices.Architecture]::Arm64) {
    throw 'Runtime validation requires a Windows ARM64 runner'
}
$Artifacts = (Resolve-Path $Artifacts).Path
foreach ($line in Get-Content (Join-Path $Artifacts 'SHA256SUMS.txt')) {
    $hash, $relative = $line -split '  ', 2
    $path = Join-Path $Artifacts $relative
    if ((Get-FileHash $path -Algorithm SHA256).Hash -ne $hash) { throw "Artifact checksum mismatch: $relative" }
}
function Run-Probe([string]$Architecture, [string]$Library, [switch]$Reject) {
    $exe = Join-Path $Artifacts "probes/$Architecture.exe"
    $dll = Join-Path $Artifacts "$Library/FreeImage.dll"
    Write-Host "::group::$Architecture -> $Library"
    if ($Reject) { & $exe $dll --expect-load-failure } else { & $exe $dll }
    if ($LASTEXITCODE) { throw "$Architecture/$Library probe failed: $LASTEXITCODE" }
    Write-Host '::endgroup::'
}
Run-Probe arm64 native
Run-Probe arm64ec compatible
Run-Probe arm64 compatible -Reject
Run-Probe arm64ec native -Reject
Run-Probe arm64 hybrid
Run-Probe arm64ec hybrid
Run-Probe x64 hybrid
$hash = (Get-FileHash (Join-Path $Artifacts 'hybrid/FreeImage.dll') -Algorithm SHA256).Hash
@"
### ARM64X runtime validation

- Native ARM64 and ARM64EC control DLLs passed image/callback tests.
- Incompatible unmerged DLLs were rejected by the opposite process type.
- ARM64, ARM64EC and emulated x64 consumers loaded the **same merged DLL**.
- PNG, TIFF, WebP, JP2, JXR and JPEG memory/callback round trips passed.
- Merged DLL SHA-256: ``$hash``
"@ | Out-File $env:GITHUB_STEP_SUMMARY -Append
