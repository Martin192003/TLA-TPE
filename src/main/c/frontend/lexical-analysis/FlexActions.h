#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../support/configuration/Environment.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/LexicalAnalyzer.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/Token.h"
#include "../../support/type/TokenLabel.h"
#include "../Frontend.h"

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer);

CompilationStatus HashLexemeAction();
CompilationStatus DoubleHashLexemeAction();
CompilationStatus MinusLexemeAction();
CompilationStatus NumberedListLexemeAction();
CompilationStatus TripleMinusLexemeAction();
CompilationStatus AtImgLexemeAction();
CompilationStatus AtCodeLexemeAction();
CompilationStatus AtEndLexemeAction();
CompilationStatus AtNoteLexemeAction();
CompilationStatus AtBlockLexemeAction();
CompilationStatus AtLinkLexemeAction();
CompilationStatus AtIdLexemeAction();
CompilationStatus TipoEqualsLexemeAction();
CompilationStatus BlockTypeLexemeAction(TokenLabel label);
CompilationStatus StringLexemeAction();
CompilationStatus TextContentLexemeAction();
CompilationStatus CodeContentLexemeAction();
CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
CompilationStatus LeaveMultilineCommentLexemeAction();
CompilationStatus EOFLexemeAction();
CompilationStatus IgnoredLexemeAction();
CompilationStatus UnknownLexemeAction();

#endif
