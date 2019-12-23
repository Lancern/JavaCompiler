add_executable(JVCDriver
        Driver.cpp)

set(tclap_include_dir "${CMAKE_SOURCE_DIR}/libs/tclap/include")
target_include_directories(JVCDriver
        PRIVATE ${tclap_include_dir})
target_link_libraries(JVCDriver
        PUBLIC JVCLex JVCFrontend JVCInfrastructure)
