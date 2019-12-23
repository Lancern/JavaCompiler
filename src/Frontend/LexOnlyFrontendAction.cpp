//
// Created by Sirui Mu on 2019/12/23.
//

#include "Infrastructure/Stream.h"
#include "Frontend/FrontendAction.h"
#include "Frontend/CompilerInstance.h"
#include "Lex/Lexer.h"
#include "BuiltinFrontendActions.h"

namespace jvc {

void LexOnlyFrontendAction::ExecuteAction(CompilerInstance& ci) {
  std::unique_ptr<StreamWriter> writer;
  StreamWriter* o;
  if (ci.options().HasOutputFile) {
    auto outputFileStream = OutputStream::FromFile(ci.options().OutputFilePath);
    writer = std::make_unique<StreamWriter>(std::move(outputFileStream));
    o = writer.get();
  } else {
    o = &outs();
  }

  for (size_t i = 1; i <= ci.GetSourceManager().size(); ++i) {
    auto lexer = Lexer::Create(ci, i);

    auto sourceFile = ci.GetSourceManager().GetSourceFileInfo(i);
    *o << "Tokenization of source file: " << sourceFile->path() << "\n";

    auto indentGuard = o->PushIndent();
    while (auto token = lexer->ReadNextToken()) {
      token->Dump(*o);
      *o << '\n';
    }
    *o << '\n';
  }
}

} // namespace jvc
