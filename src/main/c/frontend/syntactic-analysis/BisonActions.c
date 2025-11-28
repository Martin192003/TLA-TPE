#include "BisonActions.h"
#include <string.h>


static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
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

static void _logSyntacticAnalyzerAction(const char * functionName);
static void _logSyntacticAnalyzerAction(const char * functionName) {
	(void)functionName;
}


Text * TextSemanticAction(char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Text * text = calloc(1, sizeof(Text));
	text->content = content;
	text->listType = LIST_NONE;
	return text;
}


Image * ImageSemanticAction(char * path, char * caption) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Image * image = calloc(1, sizeof(Image));
	image->path = path;
	image->caption = caption;
	image->legend = NULL;
	return image;
}

Image * ImageWithLegendSemanticAction(char * path, char * caption, char * legend) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Image * image = calloc(1, sizeof(Image));
	image->path = path;
	image->caption = caption;
	image->legend = legend;
	return image;
}

CodeBlock * CodeBlockSemanticAction(char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CodeBlock * codeBlock = calloc(1, sizeof(CodeBlock));
	codeBlock->content = content;
	return codeBlock;
}

Note * NoteSemanticAction(char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Note * note = calloc(1, sizeof(Note));
	note->content = content;
	return note;
}

Block * BlockSemanticAction(BlockType type, char * title, char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Block * block = calloc(1, sizeof(Block));
	block->type = type;
	block->title = title; 
	block->content = content; 
	return block;
}

Link * LinkSemanticAction(char * text, char * target) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Link * link = calloc(1, sizeof(Link));

	if (text == target) {
		link->text = strdup(text);
		link->target = strdup(text);
		free(text);
	} else {
		link->text = target;
		link->target = text;
	}

	return link;
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

SlideItem * LinkSlideItemSemanticAction(Link * link) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideItem * slideItem = calloc(1, sizeof(SlideItem));
	slideItem->link = link;
	slideItem->type = SLIDE_ITEM_LINK;
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
	slide->title = title; 
	slide->subtitle = subtitle; 
	slide->id = NULL;
	slide->items = items;
	slide->next = NULL;
	return slide;
}

Slide * SimpleSlideSemanticAction(char * title, SlideItemList * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Slide * slide = calloc(1, sizeof(Slide));
	slide->title = title;
	slide->subtitle = NULL;
	slide->id = NULL;
	slide->items = items;
	slide->next = NULL;
	return slide;
}

Slide * SlideWithoutTitleSemanticAction(SlideItemList * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Slide * slide = calloc(1, sizeof(Slide));
	slide->title = NULL;
	slide->subtitle = NULL;
	slide->id = NULL;
	slide->items = items;
	slide->next = NULL;
	return slide;
}

Slide * SlideWithIdSemanticAction(char * title, char * id, SlideItemList * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Slide * slide = calloc(1, sizeof(Slide));
	slide->title = title;
	slide->subtitle = NULL;
	slide->id = id;
	slide->items = items;
	slide->next = NULL;
	return slide;
}

Slide * SlideWithSubtitleAndIdSemanticAction(char * title, char * subtitle, char * id, SlideItemList * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Slide * slide = calloc(1, sizeof(Slide));
	slide->title = title;
	slide->subtitle = subtitle;
	slide->id = id;
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
