//
// Created by Sirui Mu on 2019/12/23.
//

#ifndef JVC_FRONTENDACTION_H
#define JVC_FRONTENDACTION_H

#include <memory>

namespace jvc {

class CompilerInstance;
class DiagnosticsEngine;

#define JVC_FRONTEND_ACTION_LIST(h) \
    h(LexOnly) \
    h(SyntaxOnly) \
    h(EmitAsm) \
    h(EmitLLVM)

enum class FrontendActionKind {
#define DEF_VARIANT(v) v,
  JVC_FRONTEND_ACTION_LIST(DEF_VARIANT)
#undef DEF_VARIANT
};

/**
 * @brief Abstract base class for all actions taken on the frontend.
 */
class FrontendAction {
public:
  /**
   * @brief Create a @see FrontendAction object representing the given kind of frontend action.
   * @param kind the kind of frontend action.
   * @param diag the diagnostics engine.
   * @return a @see std::unique_ptr to the created frontend action.
   */
  static std::unique_ptr<FrontendAction> CreateAction(FrontendActionKind kind, DiagnosticsEngine& diag);

  virtual ~FrontendAction() = default;

  /**
   * @brief When overridden in derived classes, execute the action on the given compiler instance.
   */
  virtual void ExecuteAction(CompilerInstance& ci) = 0;
};

} // namespace jvc

#endif // JVC_FRONTENDACTION_H
