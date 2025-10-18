%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus HashLexemeAction();
extern CompilationStatus DoubleHashLexemeAction();
extern CompilationStatus MinusLexemeAction();
extern CompilationStatus TripleMinusLexemeAction();
extern CompilationStatus AtImgLexemeAction();
extern CompilationStatus AtCodeLexemeAction();
extern CompilationStatus AtEndLexemeAction();
extern CompilationStatus AtNoteLexemeAction();
extern CompilationStatus AtBlockLexemeAction();
extern CompilationStatus TipoEqualsLexemeAction();
extern CompilationStatus BlockTypeLexemeAction(TokenLabel label);
extern CompilationStatus StringLexemeAction();
extern CompilationStatus TextContentLexemeAction();
extern CompilationStatus CodeContentLexemeAction();
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus UnknownLexemeAction();

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x CODE_BLOCK
%x TEXT_LINE
%x BLOCK_CONTENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
string				\"[^\"]*\"
text_content		[^\n\r]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"## "								{ return DoubleHashLexemeAction(); }
"# "								{ return HashLexemeAction(); }

"---"								{ return TripleMinusLexemeAction(); }
"- "								{ BEGIN(TEXT_LINE); return MinusLexemeAction(); }
<TEXT_LINE>{text_content}			{ BEGIN(INITIAL); return TextContentLexemeAction(); }
<TEXT_LINE>[\n\r]					{ BEGIN(INITIAL); return IgnoredLexemeAction(); }

"@img"								{ return AtImgLexemeAction(); }
"@code"								{ BEGIN(CODE_BLOCK); return AtCodeLexemeAction(); }
<CODE_BLOCK>"@end"					{ BEGIN(INITIAL); return AtEndLexemeAction(); }
<CODE_BLOCK>[^@]+					{ return CodeContentLexemeAction(); }
<CODE_BLOCK>"@"[^e]					{ return CodeContentLexemeAction(); }
<CODE_BLOCK>"@e"[^n]				{ return CodeContentLexemeAction(); }
<CODE_BLOCK>"@en"[^d]				{ return CodeContentLexemeAction(); }

"@note"								{ return AtNoteLexemeAction(); }
"@block"							{ return AtBlockLexemeAction(); }
"@end"								{ return AtEndLexemeAction(); }

"tipo="								{ return TipoEqualsLexemeAction(); }
"normal"							{ return BlockTypeLexemeAction(NORMAL); }
"alert"								{ return BlockTypeLexemeAction(ALERT); }
"example"							{ return BlockTypeLexemeAction(EXAMPLE); }

{string}							{ return StringLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
[A-Z][A-Za-z ][^\n\r]*				{ return TextContentLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
