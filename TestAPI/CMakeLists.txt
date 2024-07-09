
add_executable(MainTestSuite 
    MainTestSuite.cpp
    testChannels.cpp
    testHeaderOnly.cpp
    testImageType.cpp
    testJPEG.cpp
    testMemIO.cpp
    testMPage.cpp
    testMPageMemory.cpp
    testMPageStream.cpp
    testPlugins.cpp
    testThumbnail.cpp
    testTools.cpp
    testWrappedBuffer.cpp
)

if (BUILD_TESTS)
    target_link_libraries(MainTestSuite FreeImage)
    if (DEFINED PNG_LIBRARY)
        target_link_libraries(MainTestSuite ${PNG_LIBRARY})
    endif()
    if (DEFINED ZLIB_LIBRARY)
        target_link_libraries(MainTestSuite ${ZLIB_LIBRARY})
    endif()
    if (WIN32)
        # Add ws2_32.lib for Windows builds
        target_link_libraries(${MainTestSuite} ws2_32)
    endif()

    add_test(NAME MainTestSuite WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/TestAPI COMMAND $<TARGET_FILE:MainTestSuite>)
endif ()
