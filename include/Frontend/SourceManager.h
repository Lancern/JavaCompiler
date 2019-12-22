//
// Created by Sirui Mu on 2019/12/18.
//

#ifndef JVC_SOURCEMANAGER_H
#define JVC_SOURCEMANAGER_H

#include "Frontend/SourceLocation.h"
#include "Diagnostics.h"

#include <cstdint>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace jvc {

class InputStream;
class CompilerInstance;

/**
 * @brief Provide information about a java source file.
 */
class SourceFileInfo {
private:
  class SourceFileLineBuffer;

public:
  /**
   * @brief Load the specified source code file and returns a @see SourceFileInfo object.
   *
   * If the source code file cannot be loaded, this function will emit a fatal diagnostics message through the given
   * diagnostics engine.
   *
   * @param fileId the ID of the new source code file.
   * @param path the path to the source code file.
   * @param diag the diagnostics engine.
   * @return a @see SourceFileInfo object containing information about the loaded source code file.
   */
  static SourceFileInfo Load(int fileId, const std::string& path, DiagnosticsEngine& diag);

  /**
   * @brief Load the source code in the given input stream and returns a @see SourceFileInfo object.
   * @param fileId the ID of the new source code file.
   * @param path path to the source code file.
   * @param inputData a @see std::unique_ptr to an @see InputStream object containing data of the source code file.
   * @return a @see SourceFileInfo object containing information about the source code.
   */
  static SourceFileInfo Load(int fileId, const std::string& path, std::unique_ptr<InputStream> inputData);

  SourceFileInfo(const SourceFileInfo &) = delete;
  SourceFileInfo(SourceFileInfo &&) noexcept = default;

  SourceFileInfo& operator=(const SourceFileInfo &) = delete;
  SourceFileInfo& operator=(SourceFileInfo &&) noexcept = default;

  /**
   * @brief Destroy this @see SourceFileInfo object.
   */
  ~SourceFileInfo();

  /**
   * @brief Get the ID of the source file.
   * @return ID of the source file.
   */
  [[nodiscard]]
  int id() const { return _id; }

  /**
   * @brief Get the path to the source code file.
   * @return path to the source code file.
   */
  [[nodiscard]]
  const std::string& path() const { return _path; }

  /**
   * @brief Get a @see std::string_view object referring to source code within the specified source range. The returned
   * view is extended to the start and the end of the corresponding line at both sides.
   * @param range the source range.
   * @return the @see std::string_view. If the given source range is invalid, then an empty @see std::string_view will
   * be returned.
   *
   * The given source range is considered invalid if any of the following conditions are true:
   * * Either the start or the end of the range is invalid;
   * * Either the start or the end of the range refers to a different file;
   * * Either the start or the end of the range is out of boundary.
   *
   */
  [[nodiscard]]
  std::string_view GetViewInRange(SourceRange range) const;

  /**
   * @brief Get a @see std::string_view object referring to source code on the same line as the specified source
   * location.
   * @param loc the source location.
   * @return the @see std::string_view. If the given source location is invalid, then an empty @see std::string_view
   * will be returned.
   *
   * The given source location is considered invalid if any of the following conditions are true:
   * * The @class SourceLocation object itself is invalid;
   * * The source location is out of boundary.
   *
   */
  [[nodiscard]]
  std::string_view GetViewAtLoc(SourceLocation loc) const;

  /**
   * @brief Get the whole content of the source code file.
   * @return the whole content of the source code file.
   */
  [[nodiscard]]
  std::string_view GetContent() const;

  /**
   * @brief Create a @see InputStream for accessing contents in this source code file.
   * @return a @see InputStream for accessing contents in this source code file.
   */
  [[nodiscard]]
  std::unique_ptr<InputStream> CreateInputStream() const;

  /**
   * @brief Get the location of the EOF indicator.
   * @return location of the EOF indicator.
   */
  [[nodiscard]]
  SourceLocation GetEOFLoc() const;

private:
  int _id;
  std::string _path;
  std::unique_ptr<SourceFileLineBuffer> _lineBuffer;

  /**
   * @brief Initialize a new @class SourceFileInfo object.
   * @param fileId the ID of the source code file.
   * @param path the path to the source code file.
   * @param lineBuffer the line buffer.
   */
  explicit SourceFileInfo(int fileId, std::string path, std::unique_ptr<SourceFileLineBuffer> lineBuffer);
}; // class SourceFileInfo

/**
 * @brief Manages java source files used in current compiler session.
 */
class SourceManager {
public:
  /**
   * @brief Initialize a new @class SourceManager object.
   * @param ci the compiler instance.
   */
  explicit SourceManager(CompilerInstance& ci);

  SourceManager(const SourceManager &) = delete;
  SourceManager(SourceManager &&) noexcept = default;

  SourceManager& operator=(const SourceManager &) = delete;

  /**
   * @brief Get the compiler instance.
   * @return the compiler instance.
   */
  [[nodiscard]]
  CompilerInstance& GetCompilerInstance() const { return _ci; }

  /**
   * @brief Get the information about the specified source code file that has been loaded.
   * @param id the ID of the source code file.
   * @return pointer to a @see SourceFileInfo object containing information about the source code file. If the
   * specified source code file could not be found, returns nullptr.
   */
  [[nodiscard]]
  const SourceFileInfo* GetSourceFileInfo(int id) const;

  /**
   * @brief Get the information about the source code file referred to by the specified source location.
   * @param loc the source location.
   * @return pointer to a @see SourceFileInfo object containing information about the source code file. If the given
   * source location is invalid or the referred file has not been loaded, returns nullptr.
   */
  [[nodiscard]]
  const SourceFileInfo* GetSourceFileInfo(SourceLocation loc) const {
    if (!loc.valid()) {
      return nullptr;
    }
    return GetSourceFileInfo(loc.fileId());
  }

  /**
   * @brief Get the information about the source code file referred to by the specified source range.
   * @param range the source range.
   * @return pointer to a @see SourceFileInfo object containing information about the source code file. If the given
   * source range is invalid or the referred file has not been loaded, returns nullptr.
   */
  [[nodiscard]]
  const SourceFileInfo* GetSourceFileInfo(SourceRange range) const {
    if (!range.valid()) {
      return nullptr;
    }
    return GetSourceFileInfo(range.fileId());
  }

  /**
   * @brief Get a @see SourceLocation object referring to the end of the specified file.
   * @param fileId the ID of the file.
   * @return a @see SourceLocation object referring to the end of the specified file. If the specified file has not been
   * loaded into source manager, returns an invalid @see SourceLocation object.
   */
  [[nodiscard]]
  SourceLocation GetLocForEndOfFile(int fileId) const;

  /**
   * @brief Load the source code file into the source manager.
   *
   * If the file cannot be loaded, this function will emit a fatal error through the diagnostics engine associated with
   * the compiler instance.
   *
   * @param path path to the source code file.
   * @return ID of the source code file.
   */
  int Load(const std::string& path);

private:
  CompilerInstance& _ci;
  std::unordered_map<int, SourceFileInfo> _sources;
}; // class SourceManager

} // namespace jvc

#endif // JVC_SOURCEMANAGER_H
