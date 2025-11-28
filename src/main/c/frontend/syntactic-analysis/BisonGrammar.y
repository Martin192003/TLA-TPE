%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */
	char * string;
	TokenLabel token;

	/** Non-terminals. */
	Text * text;
	Image * image;
	CodeBlock * codeBlock;
	Note * note;
	Block * block;
	Link * link;
	SlideItem * slideItem;
	SlideItemList * slideItemList;
	Slide * slide;
	SlideList * slideList;
	Program * program;
	BlockType blockType;
	SlideExtras slideExtras;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { if ($$) free($$); } STRING TEXT_CONTENT CODE_CONTENT
%destructor { destroyText($$); } <text>
%destructor { destroyImage($$); } <image>
%destructor { destroyCodeBlock($$); } <codeBlock>
%destructor { destroyNote($$); } <note>
%destructor { destroyBlock($$); } <block>
%destructor { destroyLink($$); } <link>
%destructor { destroySlideItem($$); } <slideItem>
%destructor { destroySlideItemList($$); } slideItems
%destructor { destroySlide($$); } <slide>
%destructor { destroySlideList($$); } slides
%destructor { if ($$) free($$); } imageLegend linkUrl blockTitle
%destructor { destroySlideItemList($$); } nonEmptySlideItems
%destructor { 
	if ($$.subtitle) free($$.subtitle); 
	if ($$.id) free($$.id); 
} <slideExtras>
%destructor { destroySlide($$); } slideWithTitle slideWithoutTitle

/** Terminals. */
%token <string> STRING
%token <string> TEXT_CONTENT
%token <string> CODE_CONTENT
%token <token> HASH
%token <token> DOUBLE_HASH
%token <token> MINUS
%token <token> NUMBERED_LIST
%token <token> TRIPLE_MINUS
%token <token> AT_IMG
%token <token> AT_CODE
%token <token> AT_END
%token <token> AT_NOTE
%token <token> AT_BLOCK
%token <token> AT_LINK
%token <token> AT_ID
%token <token> TIPO_EQUALS
%token <token> NORMAL
%token <token> ALERT
%token <token> EXAMPLE
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <text> text
%type <image> image
%type <codeBlock> codeBlock
%type <note> note
%type <block> block
%type <link> link
%type <blockType> blockType
%type <slideItem> slideItem
%type <slideItemList> slideItems nonEmptySlideItems
%type <slide> slide slideWithTitle slideWithoutTitle
%type <slideList> slides
%type <program> program
%type <slideExtras> slideExtras
%type <string> imageLegend linkUrl blockTitle

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: slides													{ $$ = ProgramSemanticAction($1); }
	;

slides: optionalSeparator slide									{ $$ = SlideListSemanticAction(EmptySlideListSemanticAction(), $2); }
	| slides slide												{ $$ = SlideListSemanticAction($1, $2); }
	| slides TRIPLE_MINUS slide									{ $$ = SlideListSemanticAction($1, $3); }
	;

optionalSeparator: %empty										{ /* No action needed */ }
	| TRIPLE_MINUS												{ /* No action needed */ }
	;

slide: slideWithTitle											{ $$ = $1; }
	| slideWithoutTitle											{ $$ = $1; }
	;

slideWithTitle: HASH STRING slideExtras slideItems				{
																	if ($3.hasSubtitle && $3.hasId) {
																		$$ = SlideWithSubtitleAndIdSemanticAction($2, $3.subtitle, $3.id, $4);
																	} else if ($3.hasSubtitle) {
																		$$ = SlideSemanticAction($2, $3.subtitle, $4);
																	} else if ($3.hasId) {
																		$$ = SlideWithIdSemanticAction($2, $3.id, $4);
																	} else {
																		$$ = SimpleSlideSemanticAction($2, $4);
																	}
																}
	;

slideWithoutTitle: nonEmptySlideItems							{ $$ = SlideWithoutTitleSemanticAction($1); }
	;

slideExtras: %empty												{
																	$$.hasSubtitle = false;
																	$$.hasId = false;
																	$$.subtitle = NULL;
																	$$.id = NULL;
																}
	| DOUBLE_HASH STRING										{
																	$$.hasSubtitle = true;
																	$$.hasId = false;
																	$$.subtitle = $2;
																	$$.id = NULL;
																}
	| AT_ID STRING												{
																	$$.hasSubtitle = false;
																	$$.hasId = true;
																	$$.subtitle = NULL;
																	$$.id = $2;
																}
	| DOUBLE_HASH STRING AT_ID STRING							{
																	$$.hasSubtitle = true;
																	$$.hasId = true;
																	$$.subtitle = $2;
																	$$.id = $4;
																}
	;

slideItems: %empty												{ $$ = EmptySlideItemListSemanticAction(); }
	| slideItems slideItem										{ $$ = SlideItemListSemanticAction($1, $2); }
	;

nonEmptySlideItems: slideItem									{ $$ = EmptySlideItemListSemanticAction(); $$ = SlideItemListSemanticAction($$, $1); }
	| nonEmptySlideItems slideItem								{ $$ = SlideItemListSemanticAction($1, $2); }
	;

slideItem: text													{ $$ = TextSlideItemSemanticAction($1); }
	| image														{ $$ = ImageSlideItemSemanticAction($1); }
	| codeBlock													{ $$ = CodeSlideItemSemanticAction($1); }
	| note														{ $$ = NoteSlideItemSemanticAction($1); }
	| block														{ $$ = BlockSlideItemSemanticAction($1); }
	| link														{ $$ = LinkSlideItemSemanticAction($1); }
	;

text: MINUS TEXT_CONTENT										{ $$ = TextSemanticAction($2); $$->listType = LIST_BULLET; }
	| NUMBERED_LIST TEXT_CONTENT								{ $$ = TextSemanticAction($2); $$->listType = LIST_NUMBERED; }
	;

image: AT_IMG STRING STRING imageLegend							{
																	if ($4 != NULL) {
																		$$ = ImageWithLegendSemanticAction($2, $3, $4);
																	} else {
																		$$ = ImageSemanticAction($2, $3);
																	}
																}
	;

imageLegend: %empty												{ $$ = NULL; }
	| STRING													{ $$ = $1; }
	;

codeBlock: AT_CODE CODE_CONTENT AT_END							{ $$ = CodeBlockSemanticAction($2); }
	;

note: AT_NOTE STRING											{ $$ = NoteSemanticAction($2); }
	;

link: AT_LINK STRING linkUrl									{
																	if ($3 != NULL) {
																		$$ = LinkSemanticAction($2, $3);
																	} else {
																		$$ = LinkSemanticAction($2, $2);
																	}
																}
	;

linkUrl: %empty													{ $$ = NULL; }
	| STRING													{ $$ = $1; }
	;

block: AT_BLOCK TIPO_EQUALS blockType blockTitle TEXT_CONTENT AT_END	{ $$ = BlockSemanticAction($3, $4, $5); }
	;

blockTitle: %empty												{ $$ = NULL; }
	| STRING													{ $$ = $1; }
	;

blockType: NORMAL												{ $$ = BLOCK_NORMAL; }
	| ALERT														{ $$ = BLOCK_ALERT; }
	| EXAMPLE													{ $$ = BLOCK_EXAMPLE; }
	;

%%
