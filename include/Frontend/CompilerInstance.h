//
// Created by Sirui Mu on 2019/12/19.
//

#ifndef JVC_COMPILERINSTANCE_H
#define JVC_COMPILERINSTANCE_H

#include "Frontend/CompilerOptions.h"
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
   * @param options the compiler options.
   */
  explicit CompilerInstance(CompilerOptions options = CompilerOptions { })
    : _options(std::move(options)),
      _diag(std::make_unique<DiagnosticsEngine>(*this)),
      _sources(std::make_unique<SourceManager>(*this))
  { }

  /**
   * @brief Get compiler options.
   * @return the compiler options.
   */
  [[nodiscard]]
  CompilerOptions& options() { return _options; }

  /**
   * @brief Get compiler options.
   * @return the compiler options.
   */
  [[nodiscard]]
  const CompilerOptions& options() const { return _options; }

  /**
   * @brief Set compiler options.
   * @param options the compiler options.
   */
  void SetOptions(CompilerOptions options) { _options = std::move(options); }

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
  CompilerOptions _options;
  std::unique_ptr<DiagnosticsEngine> _diag;
  std::unique_ptr<SourceManager> _sources;
};

} // namespace jvc

#endif // JVC_COMPILERINSTANCE_H
