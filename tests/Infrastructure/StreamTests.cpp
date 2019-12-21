//
// Created by Sirui Mu on 2019/12/21.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include "gtest/gtest.h"

#include "Infrastructure/Stream.h"

#include <cstring>
#include <string>

TEST(Stream, CreateFromSTL) {
  std::string str = "hello";
  std::stringstream ss { str };

  auto stream = jvc::InputStream::FromSTL(ss);
  ASSERT_TRUE(stream) << "FromSTL returns nullptr.";

  char buffer[5];
  ASSERT_EQ(stream->Read(buffer, 5), 5) << "Read function does not return the size of the output buffer.";
  ASSERT_TRUE(std::strncmp(buffer, "hello", 5) == 0) << "Read function returns bad content.";
}

TEST(Stream, CreateFromBuffer) {
  const char *inputBuffer = "hello";

  auto stream = jvc::InputStream::FromBuffer(inputBuffer, 5);
  ASSERT_TRUE(stream) << "FromSTL returns nullptr.";

  char outputBuffer[5];
  ASSERT_EQ(stream->Read(outputBuffer, 5), 5)
      << "Read function does not return the size of the output buffer.";
  ASSERT_TRUE(std::strncmp(outputBuffer, inputBuffer, 5) == 0) << "Read function returns bad content.";
}

#pragma clang diagnostic pop
