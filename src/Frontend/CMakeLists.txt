add_library(JVCFrontend STATIC
        SourceManager.cpp
        SourceFileInfo.cpp
        Diagnostics.cpp
        ../Infrastructure/Stream.cpp
        ../Infrastructure/StreamWriter.cpp
        ${project_include_dir}/Infrastructure/ArrayView.h
        ${project_include_dir}/Infrastructure/Stream.h
        ${project_include_dir}/Frontend/CompilerInstance.h
        ${project_include_dir}/Frontend/SourceManager.h
        ${project_include_dir}/Frontend/SourceLocation.h
        ${project_include_dir}/Frontend/Diagnostics.h)
