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

TEST(OutputStream, CreateFromSTL) {
  std::stringstream output { };
  auto stream = jvc::OutputStream::FromSTL(output);
  ASSERT_TRUE(stream) << "FromSTL function returns nullptr.";

  stream->Write("hello", 5);

  auto str = output.str();
  ASSERT_EQ(str, "hello");
}

TEST(OutputStream, Write) {
  std::stringstream output { };
  auto stream = jvc::OutputStream::FromSTL(output);

  ASSERT_EQ(stream->Write("hello", 5), 5) << "Write function does not return the size of the input buffer.";
  ASSERT_EQ(stream->Write("world", 5), 5) << "Write function does not return the size of the input buffer.";

  auto str = output.str();
  ASSERT_EQ(str, "helloworld") << "Write function does not properly write contents into the inner STL stream.";
}

TEST(StreamWriter, WriteChar) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer.WriteChar('h');
  writer.WriteChar('e');

  auto str = output.str();
  ASSERT_EQ(str, "he") << "WriteChar function does not property write characters into the inner stream.";
}

TEST(StreamWriter, WriteString) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer.Write("hello");
  writer.Write(std::string("world"));

  std::string tmp = "msr";
  writer.Write(std::string_view { tmp });

  auto str = output.str();
  ASSERT_EQ(str, "helloworldmsr") << "Write functions does not property write strings into the inner stream.";
}

TEST(StreamWriter, WriteLine) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer.WriteLine("hello");
  writer.WriteLine(std::string("world"));

  std::string tmp = "msr";
  writer.WriteLine(std::string_view { tmp });

  auto str = output.str();
  ASSERT_EQ(str, "hello\nworld\nmsr\n")
      << "Write function does not property write strings into the inner stream.";
}

TEST(StreamWriter, OutputBool) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer << true << false << true;

  auto str = output.str();
  ASSERT_EQ(str, "truefalsetrue")
      << "Output operator does not properly write boolean values into the inner stream.";
}

TEST(StreamWriter, OutputChar) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer << 'h' << 'e';

  auto str = output.str();
  ASSERT_EQ(str, "he")
                << "Output operator does not properly write characters into the inner stream.";
}

TEST(StreamWriter, OutputString) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  std::string tmp = "msr";
  writer << "hello" << std::string("world") << std::string_view { tmp };

  auto str = output.str();
  ASSERT_EQ(str, "helloworldmsr")
      << "Output operator does not properly write strings into the inner stream.";
}

TEST(StreamWriter, OutputInteger) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer << 10 << 20;

  auto str = output.str();
  ASSERT_EQ(str, "1020")
      << "Output operator does not properly write integers into the inner stream.";
}

TEST(StreamWriter, SingleIndent) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer.Write("hello");
  {
    auto indent = writer.PushIndent();
    writer.WriteLine("world");
    writer.WriteLine("msr\n\ntest");
  }
  writer.WriteLine("java");
  writer.WriteLine("compiler");

  auto str = output.str();
  ASSERT_EQ(str, "helloworld\n  msr\n\n  test\njava\ncompiler\n")
      << "Writer does not properly handle single level of indent.";
}

TEST(StreamWriter, MultipleLevelsOfIndent) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer.WriteLine("hello");
  {
    auto indent1 = writer.PushIndent();
    writer.WriteLine("world");
    {
      auto indent2 = writer.PushIndent();
      writer.WriteLine("java");
    }
    writer.WriteLine("compiler");
  }

  auto str = output.str();
  ASSERT_EQ(str, "hello\n  world\n    java\n  compiler\n")
      << "Writer does not properly handle multiple levels of indent.";
}

TEST(StreamWriter, MultipleLevelsOfIndentWithGuardRelease) {
  std::stringstream output { };
  jvc::StreamWriter writer { jvc::OutputStream::FromSTL(output) };

  writer.WriteLine("hello");
  {
    auto indent1 = writer.PushIndent();
    writer.WriteLine("world");
    {
      auto indent2 = writer.PushIndent();
      writer.WriteLine("java");
      indent2.pop();
      writer.WriteLine("compiler");
      indent2.pop();
      writer.WriteLine("jvc");
    }
    writer.WriteLine("msr");
  }

  auto str = output.str();
  ASSERT_EQ(str, "hello\n  world\n    java\n  compiler\n  jvc\n  msr\n")
                << "Writer does not properly handle multiple levels of indent when some guards are popped manually";
}

#pragma clang diagnostic pop
