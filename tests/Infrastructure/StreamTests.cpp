//
// Created by Sirui Mu on 2019/12/21.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include "gtest/gtest.h"

#include "Infrastructure/Stream.h"

#include <cstring>
#include <string>

TEST(InputStream, CreateFromSTL) {
  std::string str = "hello";
  std::stringstream ss { str };

  auto stream = jvc::InputStream::FromSTL(ss);
  ASSERT_TRUE(stream) << "FromSTL returns nullptr.";

  char buffer[5] = { 0 };
  ASSERT_EQ(stream->Read(buffer, 5), 5) << "Read function does not return the size of the output buffer.";
  ASSERT_TRUE(std::strncmp(buffer, "hello", 5) == 0)
      << "Read function returns bad content. "
      << "Should return: `hello`, but return: `" << buffer << "`";
}

TEST(InputStream, CreateFromBuffer) {
  const char *inputBuffer = "hello";

  auto stream = jvc::InputStream::FromBuffer(inputBuffer, 5);
  ASSERT_TRUE(stream) << "FromSTL returns nullptr.";

  char outputBuffer[5] = { 0 };
  ASSERT_EQ(stream->Read(outputBuffer, 5), 5)
      << "Read function does not return the size of the output buffer.";
  ASSERT_TRUE(std::strncmp(outputBuffer, inputBuffer, 5) == 0)
      << "Read function returns bad content. "
      << "Should return: `hello`, but return: `" << outputBuffer << "`";
}

TEST(InputStream, Read) {
  const char *inputBuffer = "hello";

  auto stream = jvc::InputStream::FromBuffer(inputBuffer, 5);
  char outputBuffer[5] = { 0 };

  ASSERT_EQ(stream->Read(outputBuffer, 2), 2)
      << "Read function does not return the size of the output buffer.";
  ASSERT_TRUE(std::strncmp("he", outputBuffer, 2) == 0)
      << "Read function returns bad content. "
      << "Should return: `he`, but return: `" << outputBuffer << "`";

  memset(outputBuffer, 0, sizeof(outputBuffer));
  ASSERT_EQ(stream->Read(outputBuffer, 2), 2)
      << "Read function does not return the size of the output buffer.";
  ASSERT_TRUE(std::strncmp("ll", outputBuffer, 2) == 0)
      << "Read function returns bad content. "
      << "Should return: `ll`, but return: `" << outputBuffer << "`";

  memset(outputBuffer, 0, sizeof(outputBuffer));
  ASSERT_EQ(stream->Read(outputBuffer, 2), 1)
      << "Read function does not properly handle partial reads.";
  ASSERT_TRUE(std::strncmp("o", outputBuffer, 1) == 0)
      << "Read function returns bad content. "
      << "Should return: `o`, but return: `" << outputBuffer << "`";
}

#pragma clang diagnostic pop
