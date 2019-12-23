add_library(JVCFrontend STATIC
        SourceManager.cpp
        SourceLocation.cpp
        SourceFileInfo.cpp
        SourceFileLineBuffer.h
        SourceFileLineBuffer.cpp
        Diagnostics.cpp
        FrontendAction.cpp
        BuiltinFrontendActions.h
        LexOnlyFrontendAction.cpp
        ${JVC_INCLUDE_DIR}/Frontend/CompilerInstance.h
        ${JVC_INCLUDE_DIR}/Frontend/CompilerOptions.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceManager.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceLocation.h
        ${JVC_INCLUDE_DIR}/Frontend/Diagnostics.h
        ${JVC_INCLUDE_DIR}/Frontend/FrontendAction.h)
target_link_libraries(JVCFrontend
        PUBLIC JVCInfrastructure)
