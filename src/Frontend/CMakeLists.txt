add_library(JVCFrontend STATIC
        SourceManager.cpp
        SourceFileInfo.cpp
        Diagnostics.cpp
        ../Infrastructure/Stream.cpp
        ../Infrastructure/StreamWriter.cpp
        ${JVC_INCLUDE_DIR}/Infrastructure/ArrayView.h
        ${JVC_INCLUDE_DIR}/Infrastructure/Stream.h
        ${JVC_INCLUDE_DIR}/Frontend/CompilerInstance.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceManager.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceLocation.h
        ${JVC_INCLUDE_DIR}/Frontend/Diagnostics.h)
