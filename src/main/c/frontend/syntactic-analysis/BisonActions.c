#include "BisonActions.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);
static char * _removeQuotes(const char * quotedString);

/**
 * Removes quotes from a quoted string and returns a duplicated string without quotes.
 * FREES the original string to prevent memory leaks.
 */
static char * _removeQuotes(const char * quotedString) {
	if (quotedString == NULL) return NULL;
	
	int len = strlen(quotedString);
	char * result;
	
	if (len >= 2 && quotedString[0] == '"' && quotedString[len-1] == '"') {
		result = malloc(len - 1);
		strncpy(result, quotedString + 1, len - 2);
		result[len - 2] = '\0';
	} else {
		result = strdup(quotedString);
	}
	
	free((char*)quotedString);
	return result;
}

/**
 * Converts a string to BlockType enum.
 */
static BlockType _stringToBlockType(const char * blockTypeStr) {
	char * cleanStr = _removeQuotes(blockTypeStr);
	BlockType result = BLOCK_NORMAL; // default
	
	if (strcmp(cleanStr, "alert") == 0) {
		result = BLOCK_ALERT;
	} else if (strcmp(cleanStr, "example") == 0) {
		result = BLOCK_EXAMPLE;
	}
	
	free(cleanStr);
	return result;
}

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Text * TextSemanticAction(char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Text * text = calloc(1, sizeof(Text));
	text->content = _removeQuotes(content);
	return text;
}

Image * ImageSemanticAction(char * path, char * caption) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Image * image = calloc(1, sizeof(Image));
	image->path = _removeQuotes(path);
	image->caption = _removeQuotes(caption);
	return image;
}

CodeBlock * CodeBlockSemanticAction(char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CodeBlock * codeBlock = calloc(1, sizeof(CodeBlock));
	codeBlock->content = _removeQuotes(content);
	return codeBlock;
}

Note * NoteSemanticAction(char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Note * note = calloc(1, sizeof(Note));
	note->content = _removeQuotes(content);
	return note;
}

Block * BlockSemanticAction(BlockType type, char * title, char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Block * block = calloc(1, sizeof(Block));
	block->type = type;
	block->title = title ? _removeQuotes(title) : NULL;
	block->content = _removeQuotes(content);
	return block;
}

SlideItem * TextSlideItemSemanticAction(Text * text) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItem * slideItem = calloc(1, sizeof(SlideItem));
	slideItem->text = text;
	slideItem->type = SLIDE_ITEM_TEXT;
	slideItem->next = NULL;
	return slideItem;
}

SlideItem * ImageSlideItemSemanticAction(Image * image) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItem * slideItem = calloc(1, sizeof(SlideItem));
	slideItem->image = image;
	slideItem->type = SLIDE_ITEM_IMAGE;
	slideItem->next = NULL;
	return slideItem;
}

SlideItem * CodeSlideItemSemanticAction(CodeBlock * codeBlock) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItem * slideItem = calloc(1, sizeof(SlideItem));
	slideItem->codeBlock = codeBlock;
	slideItem->type = SLIDE_ITEM_CODE;
	slideItem->next = NULL;
	return slideItem;
}

SlideItem * NoteSlideItemSemanticAction(Note * note) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItem * slideItem = calloc(1, sizeof(SlideItem));
	slideItem->note = note;
	slideItem->type = SLIDE_ITEM_NOTE;
	slideItem->next = NULL;
	return slideItem;
}

SlideItem * BlockSlideItemSemanticAction(Block * block) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItem * slideItem = calloc(1, sizeof(SlideItem));
	slideItem->block = block;
	slideItem->type = SLIDE_ITEM_BLOCK;
	slideItem->next = NULL;
	return slideItem;
}

SlideItemList * EmptySlideItemListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItemList * list = calloc(1, sizeof(SlideItemList));
	list->first = NULL;
	list->last = NULL;
	return list;
}

SlideItemList * SlideItemListSemanticAction(SlideItemList * list, SlideItem * item) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (list->first == NULL) {
		list->first = item;
		list->last = item;
	} else {
		list->last->next = item;
		list->last = item;
	}
	return list;
}

Slide * SlideSemanticAction(char * title, char * subtitle, SlideItemList * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Slide * slide = calloc(1, sizeof(Slide));
	slide->title = _removeQuotes(title);
	slide->subtitle = subtitle ? _removeQuotes(subtitle) : NULL;
	slide->items = items;
	slide->next = NULL;
	return slide;
}

Slide * SimpleSlideSemanticAction(char * title, SlideItemList * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Slide * slide = calloc(1, sizeof(Slide));
	slide->title = _removeQuotes(title);
	slide->subtitle = NULL;
	slide->items = items;
	slide->next = NULL;
	return slide;
}

SlideList * EmptySlideListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideList * list = calloc(1, sizeof(SlideList));
	list->first = NULL;
	list->last = NULL;
	return list;
}

SlideList * SlideListSemanticAction(SlideList * list, Slide * slide) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (list->first == NULL) {
		list->first = slide;
		list->last = slide;
	} else {
		list->last->next = slide;
		list->last = slide;
	}
	return list;
}

Program * ProgramSemanticAction(SlideList * slides) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->slides = slides;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}
