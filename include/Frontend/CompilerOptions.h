//
// Created by Sirui Mu on 2019/12/23.
//

#ifndef JVC_COMPILEROPTIONS_H
#define JVC_COMPILEROPTIONS_H

#include "FrontendAction.h"

#include <string>

namespace jvc {

struct CompilerOptions {
  bool HasOutputFile;
  std::string OutputFilePath;
};

} // namespace jvc

#endif // JVC_COMPILEROPTIONS_H
