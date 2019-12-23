//
// Created by Sirui Mu on 2019/12/23.
//

#include "Frontend/CompilerOptions.h"
#include "Frontend/CompilerInstance.h"
#include "Frontend/FrontendAction.h"

#include <cstdlib>
#include <iostream>
#include <vector>

#include "tclap/CmdLine.h"

namespace {

struct CommandLineArgs {
  bool LexOnly;
  bool HasOutputFile;
  std::string OutputFile;
  std::vector<std::string> InputFiles;
};

CommandLineArgs ParseCommandLine(int argc, char* argv[]) {
  try {
    TCLAP::CmdLine cmd { "Minimal Java Compiler by Sirui Mu", ' ', "0.1" };

    TCLAP::ValueArg<std::string> outputFile {
        "o", "output", "Path to the output file", false, "", "string", cmd };

    TCLAP::SwitchArg lexOnlySwitch {
      "", "lex-only", "Execute lexer only.", cmd, false };

    TCLAP::UnlabeledMultiArg<std::string> inputFiles {
      "input", "Input files", true, "string", cmd, true };

    cmd.parse(argc, argv);

    CommandLineArgs args { };
    args.LexOnly = lexOnlySwitch.getValue();
    args.HasOutputFile = outputFile.isSet();
    if (args.HasOutputFile) {
      args.OutputFile = outputFile.getValue();
    }
    for (const auto& inFile : inputFiles) {
      args.InputFiles.push_back(inFile);
    }

    return args;
  } catch (TCLAP::ArgException& e) {
    std::cerr << "fatal error: " << e.error() << " for arg " << e.argId() << std::endl;
    std::exit(1);
  }
}

jvc::FrontendActionKind GetFrontendActionKind(const CommandLineArgs& args) {
  if (args.LexOnly) {
    return jvc::FrontendActionKind::LexOnly;
  }

  return jvc::FrontendActionKind::EmitLLVM;
}

} // namespace <anonymous>

int main(int argc, char* argv[]) {
  auto args = ParseCommandLine(argc, argv);

  jvc::CompilerOptions compilerOptions { };
  compilerOptions.HasOutputFile = args.HasOutputFile;
  if (args.HasOutputFile) {
    compilerOptions.OutputFilePath = std::move(args.OutputFile);
  }

  auto compiler = std::make_unique<jvc::CompilerInstance>(std::move(compilerOptions));
  for (const auto& inputFile : args.InputFiles) {
    compiler->GetSourceManager().Load(inputFile);
  }

  auto frontendActionKind = GetFrontendActionKind(args);
  auto frontendAction = jvc::FrontendAction::CreateAction(frontendActionKind, compiler->GetDiagnosticsEngine());
  frontendAction->ExecuteAction(*compiler);

  return 0;
}
