//
// Created by Sirui Mu on 2019/12/22.
//

#include "gtest/gtest.h"

#include "Infrastructure/Stream.h"
#include "Frontend/SourceManager.h"

class SourceFileInfoTests : public ::testing::Test {
protected:
  void SetUp() override {
    std::string source = "first line\nsecond line\nthird line";
    int fileId = 325;
    std::string path = "source/file/path";

    auto sourceStream = jvc::InputStream::FromBuffer(source.data(), source.size());

    info = std::make_unique<jvc::SourceFileInfo>(jvc::SourceFileInfo::Load(fileId, path, std::move(sourceStream)));
  }

  std::unique_ptr<jvc::SourceFileInfo> info;
};

TEST_F(SourceFileInfoTests, GetId) {
  ASSERT_EQ(info->id(), 325) << "SourceFileInfo gives wrong file ID.";
}

TEST_F(SourceFileInfoTests, GetPath) {
  ASSERT_EQ(info->path(), "source/file/path") << "SourceFileInfo gives wrong path.";
}

TEST_F(SourceFileInfoTests, GetContent) {
  ASSERT_EQ(info->GetContent(), "first line\nsecond line\nthird line") << "SourceFileInfo gives wrong content.";
}

TEST_F(SourceFileInfoTests, GetEOFLoc) {
  jvc::SourceLocation eof { 325, 3, 11 };
  ASSERT_EQ(info->GetEOFLoc(), eof) << "SourceFileInfo gives wrong EOF location.";
}

TEST_F(SourceFileInfoTests, GetViewAtLoc) {
  jvc::SourceLocation loc { 325, 2, 8 };
  auto view = info->GetViewAtLoc(loc);

  ASSERT_EQ(view, "second line\n") << "SourceFileInfo gives wrong location view.";
}

TEST_F(SourceFileInfoTests, GetViewAtLocInvalidFileID) {
  jvc::SourceLocation loc { 1, 2, 8 };
  auto view = info->GetViewAtLoc(loc);

  ASSERT_TRUE(view.empty()) << "SourceFileInfo gives non-empty location view when file ID is wrong.";
}

TEST_F(SourceFileInfoTests, GetViewAtLocInvalidLineNumber) {
  jvc::SourceLocation loc { 325, 10, 1 };
  auto view = info->GetViewAtLoc(loc);

  ASSERT_TRUE(view.empty()) << "SourceFileInfo gives non-empty location view when line number is invalid.";
}

TEST_F(SourceFileInfoTests, GetViewAtLocInvalid) {
  jvc::SourceLocation loc;
  auto view = info->GetViewAtLoc(loc);

  ASSERT_TRUE(view.empty()) << "SourceFileInfo gives non-empty location view when source location is invalid.";
}

TEST_F(SourceFileInfoTests, GetViewInRange) {
  jvc::SourceRange range {
    jvc::SourceLocation { 325, 1, 3 },
    jvc::SourceLocation { 325, 2, 8 }
  };
  auto view = info->GetViewInRange(range);

  ASSERT_EQ(view, "first line\nsecond line\n") << "SourceFileInfo gives wrong range view.";
}

TEST_F(SourceFileInfoTests, GetViewInRangeInvalidFileId) {
  jvc::SourceRange range {
      jvc::SourceLocation { 1, 1, 3 },
      jvc::SourceLocation { 325, 2, 8 }
  };
  auto view = info->GetViewInRange(range);

  ASSERT_TRUE(view.empty()) << "SourceFileInfo gives non-empty range view when file ID is invaid.";
}

TEST_F(SourceFileInfoTests, GetViewInRangeInvalid) {
  jvc::SourceRange range { };
  auto view = info->GetViewInRange(range);

  ASSERT_TRUE(view.empty()) << "SourceFileInfo gives non-empty range view when range is invaid.";
}
