cmake_minimum_required(VERSION 3.16)
project(FreeImage VERSION 3.19.0 LANGUAGES C CXX)

# Options for configuring the build
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_JXR "Build JPEG-XR support" WIN32)
option(BUILD_TESTS "Build the test suite" OFF)

# Define the FreeImage library
add_subdirectory(Source)

# Preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
    DISABLE_PERF_MEASUREMENT
    OPJ_STATIC
    NO_LCMS
    NO_WINDOWS
)
if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC FREEIMAGE_LIB)
endif ()

# Set C++ standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# Emscripten support
if(EMSCRIPTEN)
    add_executable(FreeImage_wasm)
    target_link_libraries(FreeImage_wasm PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
endif()

# Apple-specific settings
if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
    if(BUILD_PLATFORM_APPLE_IOS)
        set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
    endif()
endif()

# Enable Multi-core compilation in MSVC
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
endif()

# Append -d to debug build so we can have both debug and release builds in the same directory
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX -d)

# Test suite
if (BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(TestAPI)
endif ()

# Install the library
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)
install(FILES Source/FreeImage.h DESTINATION include)

# Install the CMake Targets
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the CMake config
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
    ${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)
