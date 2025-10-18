#include "AbstractSyntaxTree.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyText(Text * text) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (text != NULL) {
		if (text->content != NULL) {
			free(text->content);
		}
		free(text);
	}
}

void destroyImage(Image * image) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (image != NULL) {
		if (image->path != NULL) {
			free(image->path);
		}
		if (image->caption != NULL) {
			free(image->caption);
		}
		free(image);
	}
}

void destroyCodeBlock(CodeBlock * codeBlock) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (codeBlock != NULL) {
		if (codeBlock->content != NULL) {
			free(codeBlock->content);
		}
		free(codeBlock);
	}
}

void destroyNote(Note * note) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (note != NULL) {
		if (note->content != NULL) {
			free(note->content);
		}
		free(note);
	}
}

void destroyBlock(Block * block) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (block != NULL) {
		if (block->title != NULL) {
			free(block->title);
		}
		if (block->content != NULL) {
			free(block->content);
		}
		free(block);
	}
}

void destroySlideItem(SlideItem * slideItem) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (slideItem != NULL) {
		switch (slideItem->type) {
			case SLIDE_ITEM_TEXT:
				destroyText(slideItem->text);
				break;
			case SLIDE_ITEM_IMAGE:
				destroyImage(slideItem->image);
				break;
			case SLIDE_ITEM_CODE:
				destroyCodeBlock(slideItem->codeBlock);
				break;
			case SLIDE_ITEM_NOTE:
				destroyNote(slideItem->note);
				break;
			case SLIDE_ITEM_BLOCK:
				destroyBlock(slideItem->block);
				break;
		}
		free(slideItem);
	}
}

void destroySlideItemList(SlideItemList * slideItemList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (slideItemList != NULL) {
		SlideItem * current = slideItemList->first;
		while (current != NULL) {
			SlideItem * next = current->next;
			destroySlideItem(current);
			current = next;
		}
		free(slideItemList);
	}
}

void destroySlide(Slide * slide) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (slide != NULL) {
		if (slide->title != NULL) {
			free(slide->title);
		}
		if (slide->subtitle != NULL) {
			free(slide->subtitle);
		}
		destroySlideItemList(slide->items);
		free(slide);
	}
}

void destroySlideList(SlideList * slideList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (slideList != NULL) {
		Slide * current = slideList->first;
		while (current != NULL) {
			Slide * next = current->next;
			destroySlide(current);
			current = next;
		}
		free(slideList);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroySlideList(program->slides);
		free(program);
	}
}
