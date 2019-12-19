add_executable(JVCUnitTest
        main.cpp)

set(gtest_include_dir "${CMAKE_SOURCE_DIR}/libs/googletest/googletest/include")

target_include_directories(JVCUnitTest
        PRIVATE ${gtest_include_dir})
target_link_libraries(JVCUnitTest
        PUBLIC JVCFrontend gtest)

add_test(NAME JVCUnitTest COMMAND JVCUnitTest)
