//
// Created by Sirui Mu on 2019/12/18.
//

#include "Infrastructure/Stream.h"
#include "Frontend/CompilerInstance.h"
#include "Frontend/SourceManager.h"
#include "Frontend/Diagnostics.h"
#include "SourceFileLineBuffer.h"

namespace jvc {

const SourceFileInfo* SourceManager::GetSourceFileInfo(int id) const {
  auto i = _sources.find(id);
  if (i == _sources.end()) {
    return nullptr;
  }
  return &i->second;
}

SourceManager::SourceManager(CompilerInstance &ci)
    : _ci(ci)
{ }

SourceLocation SourceManager::GetLocForEndOfFile(int fileId) const {
  auto sourceFileInfo = GetSourceFileInfo(fileId);
  if (!sourceFileInfo) {
    return SourceLocation { };
  }

  return sourceFileInfo->GetEOFLoc();
}

int SourceManager::Load(const std::string &path) {
  auto fileId = getNextFileId();

  auto sourceFileInfo = SourceFileInfo::Load(fileId, path, _ci.GetDiagnosticsEngine());
  _sources.emplace(fileId, std::move(sourceFileInfo));

  return fileId;
}

int SourceManager::Load(const std::string &name, std::unique_ptr<InputStream> dataStream) {
  auto fileId = getNextFileId();

  auto sourceFileInfo = SourceFileInfo::Load(fileId, name, std::move(dataStream));
  _sources.emplace(fileId, std::move(sourceFileInfo));

  return fileId;
}

int SourceManager::getNextFileId() const {
  return static_cast<int>(_sources.size()) + 1;
}

} // namespace jvc
