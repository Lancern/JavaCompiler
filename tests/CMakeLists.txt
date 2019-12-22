add_executable(JVCUnitTest
        main.cpp
        Infrastructure/StreamTests.cpp
        Frontend/SourceFileInfoTests.cpp)

set(gtest_include_dir "${CMAKE_SOURCE_DIR}/libs/googletest/googletest/include")

target_include_directories(JVCUnitTest
        PRIVATE ${gtest_include_dir})
target_link_libraries(JVCUnitTest
        PUBLIC JVCInfrastructure JVCFrontend JVCLex gtest)

add_test(NAME JVCUnitTest COMMAND JVCUnitTest)
