//
// Created by Sirui Mu on 2019/12/19.
//

#ifndef JVC_DIAGNOSTICS_H
#define JVC_DIAGNOSTICS_H

#include "Frontend/SourceLocation.h"

#include <memory>
#include <string>

namespace jvc {

class CompilerInstance;

#define JVC_DIAGNOSTICS_LEVELS(h)   \
  h(Info)                           \
  h(Warning)                        \
  h(Error)                          \
  h(Fatal)

/**
 * @brief Diagnostics levels.
 */
enum class DiagnosticsLevel {
#define EMIT_ENUM_CLASS_VARIANT(v) v,
  JVC_DIAGNOSTICS_LEVELS(EMIT_ENUM_CLASS_VARIANT)
#undef EMIT_ENUM_CLASS_VARIANT
};

/**
 * @brief Provide options for the diagnostics engine.
 */
struct DiagnosticsOptions {
  /**
   * @brief Should we treat warnings as errors?
   */
  bool TreatWarningsAsErrors;

  /**
   * @brief Should we exit the compiler session on error diagnostics?
   */
  bool ExitOnError;
};

/**
 * @brief A diagnostics message that can be sent to the diagnostics engine.
 */
class DiagnosticsMessage {
public:
  /**
   * @brief Create a @see DiagnosticsMessage object holding the specified literal string as diagnostics message.
   * @param level the diagnostics level.
   * @param message the diagnostics message.
   * @return a @see std::unique_ptr to the created @see DiagnosticsMessage object.
   */
  static std::unique_ptr<DiagnosticsMessage>
      CreateLiteral(DiagnosticsLevel level, std::string message);

  /**
   * @brief Create a @see DiagnosticsMessage object holding the specified literal string as diagnostics message.
   * @param level the diagnostics level.
   * @param loc the source location from which this diagnostics message is triggered.
   * @param message the diagnostics message.
   * @return a @see std::unique_ptr to the created @see DiagnosticsMessage object.
   */
  static std::unique_ptr<DiagnosticsMessage>
      CreateLiteral(DiagnosticsLevel level, SourceLocation loc, std::string message);

  /**
   * @brief Create a @see DiagnosticsMessage object holding the specified literal string as diagnostics message.
   * @param level the diagnostics level.
   * @param range the source range from which this diagnostics message is triggered.
   * @param message the diagnostics message.
   * @return a @see std::unique_ptr to the created @see DiagnosticsMessage object.
   */
  static std::unique_ptr<DiagnosticsMessage>
      CreateLiteral(DiagnosticsLevel level, SourceRange range, std::string message);

  /**
   * @brief Destroy a @see DiagnosticsMessage object.
   */
  virtual ~DiagnosticsMessage() = default;

  /**
   * @brief Get the level of the diagnostics message.
   * @return the level of the diagnostics message.
   */
  [[nodiscard]]
  DiagnosticsLevel level() const { return _level; }

  /**
   * @brief Get the diagnostics message.
   * @return the diagnostics message.
   */
  [[nodiscard]]
  virtual std::string FormatMessage() const = 0;

  /**
   * @brief Get the source location from which the diagnostics is triggered.
   * @return the source location from which the diagnostics is triggered.
   */
  [[nodiscard]]
  SourceLocation location() const { return _loc; }

  /**
   * @brief Get the source range from which the diagnostics is triggered.
   * @return the source range from which the diagnostics is triggered.
   */
  [[nodiscard]]
  SourceRange range() const { return _range; }

protected:
  /**
   * @brief Initialize a new @see DiagnosticsMessage object.
   * @param level the diagnostics level.
   */
  explicit DiagnosticsMessage(DiagnosticsLevel level)
    : _level(level),
      _loc { },
      _range { }
  { }

  /**
   * @brief Initialize a new @see DiagnosticsMessage object.
   * @param level the diagnostics level.
   * @param loc the source location from which this diagnostics message is triggered.
   */
  explicit DiagnosticsMessage(DiagnosticsLevel level, SourceLocation loc)
    : _level(level),
      _loc(loc),
      _range { }
  { }

  /**
   * @brief Initialize a new @see DiagnosticsMessage object.
   * @param level the diagnostics level.
   * @param range the source range from which this diagnostics message is triggered.
   */
  explicit DiagnosticsMessage(DiagnosticsLevel level, SourceRange range)
    : _level(level),
      _loc { },
      _range(range)
  { }

private:
  DiagnosticsLevel _level;
  SourceLocation _loc;
  SourceRange _range;
};

/**
 * @brief Diagnostics engine used in JVC.
 */
class DiagnosticsEngine {
public:
  /**
   * @brief Initialize a new @see DiagnosticsEngine object.
   * @param options the diagnostics options.
   */
  explicit DiagnosticsEngine(CompilerInstance& ci, DiagnosticsOptions options = DiagnosticsOptions { })
    : _ci(ci),
      _opt(options)
  { }

  DiagnosticsEngine(const DiagnosticsEngine &) = delete;
  DiagnosticsEngine(DiagnosticsEngine &&) noexcept = default;

  DiagnosticsEngine& operator=(const DiagnosticsEngine &) = delete;

  /**
   * @brief Destroy a @see DiagnosticsEngine object.
   */
  virtual ~DiagnosticsEngine() = default;

  /**
   * @brief Emit the given message at the specified diagnostics level.
   *
   * This function will not return if the specified diagnostics level is Fatal. This function may will not return if
   * the specified diagnostics level is Error and the diagnostics engine is configured to not return under such
   * circumstances.
   *
   * @param level the diagnostics level.
   * @param message the diagnostics message.
   */
  virtual void Emit(const DiagnosticsMessage& message);

protected:
  /**
   * @brief Get diagnostics options.
   * @return diagnostics options.
   */
  [[nodiscard]]
  const DiagnosticsOptions& options() const { return _opt; }

private:
  CompilerInstance& _ci;
  DiagnosticsOptions _opt;

  /**
   * @brief Map the given diagnostics level according to the diagnostics engine's configuration.
   * @param level the diagnostics level.
   */
  [[nodiscard]]
  DiagnosticsLevel mapDiagLevel(DiagnosticsLevel level) const;

  /**
   * @brief Determine whether the compiler session should exit on receiving a diagnostics message of the specified
   * level.
   * @param level the diagnostics level.
   * @return whether the compiler session should exit.
   */
  [[nodiscard]]
  bool shouldExit(DiagnosticsLevel level) const;
};

} // namespace jvc

#endif // JVC_DIAGNOSTICS_H
