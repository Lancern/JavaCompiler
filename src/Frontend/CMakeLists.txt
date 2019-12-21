add_library(JVCFrontend STATIC
        SourceManager.cpp
        SourceFileInfo.cpp
        SourceFileLineBuffer.h
        SourceFileLineBuffer.cpp
        Diagnostics.cpp
        ${JVC_INCLUDE_DIR}/Frontend/CompilerInstance.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceManager.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceLocation.h
        ${JVC_INCLUDE_DIR}/Frontend/Diagnostics.h)
target_link_libraries(JVCFrontend JVCInfrastructure)
