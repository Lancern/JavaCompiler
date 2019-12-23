//
// Created by Sirui Mu on 2019/12/19.
//

#ifndef JVC_COMPILERINSTANCE_H
#define JVC_COMPILERINSTANCE_H

#include "Frontend/Diagnostics.h"
#include "Frontend/SourceManager.h"

#include <memory>

namespace jvc {

/**
 * @brief Maintain compiler session wide information.
 */
class CompilerInstance {
public:
  /**
   * @brief Create a new @see CompilerInstance object.
   */
  explicit CompilerInstance()
    : _diag(std::make_unique<DiagnosticsEngine>(*this)),
      _sources(std::make_unique<SourceManager>(*this))
  { }

  /**
   * @brief Get the diagnostics engine.
   * @return the diagnostics engine.
   */
  [[nodiscard]]
  DiagnosticsEngine& GetDiagnosticsEngine() { return *_diag; }

  /**
   * @brief Get the diagnostics engine.
   * @return the diagnostics engine.
   */
  [[nodiscard]]
  const DiagnosticsEngine& GetDiagnosticsEngine() const { return *_diag; }

  /**
   * @brief Get the source manager.
   * @return the source manager.
   */
  [[nodiscard]]
  SourceManager& GetSourceManager() { return *_sources; }

  /**
   * @brief Get the source manager.
   * @return the source manager.
   */
  [[nodiscard]]
  const SourceManager& GetSourceManager() const { return *_sources; }

private:
  std::unique_ptr<DiagnosticsEngine> _diag;
  std::unique_ptr<SourceManager> _sources;
};

} // namespace jvc

#endif // JVC_COMPILERINSTANCE_H
