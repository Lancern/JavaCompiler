//
// Created by Sirui Mu on 2019/12/23.
//

#ifndef JVC_BUILTINFRONTENDACTIONS_H
#define JVC_BUILTINFRONTENDACTIONS_H

#include "Frontend/FrontendAction.h"

namespace jvc {

class LexOnlyFrontendAction : public FrontendAction {
public:
  void ExecuteAction(CompilerInstance &ci) override;
};

} // namespace jvc

#endif // JVC_BUILTINFRONTENDACTIONS_H
