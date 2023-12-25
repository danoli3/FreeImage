
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

target_link_libraries(MainTestSuite FreeImage::FreeImage)
add_test(NAME MainTestSuite WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/TestAPI COMMAND $<TARGET_FILE:MainTestSuite>)
