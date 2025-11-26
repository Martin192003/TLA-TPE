#ifndef FLEX_EXPORT_HEADER
#define FLEX_EXPORT_HEADER

#include "../../support/type/FlexContext.h"
#include "../../support/type/LexicalAnalyzer.h"
#include <stdbool.h>

FlexContext flexCurrentContext(LexicalAnalyzer * lexicalAnalyzer) {
	struct yyguts_t * yyg = (struct yyguts_t *) lexicalAnalyzer->scanner;
	return YYSTATE;
}

void flexEnterContext(LexicalAnalyzer * lexicalAnalyzer, FlexContext flexContext) {
	yy_push_state(flexContext, lexicalAnalyzer->scanner);
}

bool flexHasBuffer(LexicalAnalyzer * lexicalAnalyzer) {
	struct yyguts_t * yyg = (struct yyguts_t *) lexicalAnalyzer->scanner;
	return YY_CURRENT_BUFFER != NULL;
}

void flexLeaveContext(LexicalAnalyzer * lexicalAnalyzer) {
	yy_pop_state(lexicalAnalyzer->scanner);
}

#endif
