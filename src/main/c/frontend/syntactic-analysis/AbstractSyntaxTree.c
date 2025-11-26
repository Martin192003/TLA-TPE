#include "AbstractSyntaxTree.h"
#include <string.h>


static Logger * _logger = NULL;

void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}


void destroyText(Text * text) {
	if (text != NULL) {
		if (text->content != NULL) {
			free(text->content);
		}
		free(text);
	}
}

void destroyImage(Image * image) {
	if (image != NULL) {
		if (image->path != NULL) {
			free(image->path);
		}
		if (image->caption != NULL) {
			free(image->caption);
		}
		if (image->legend != NULL) {
			free(image->legend);
		}
		free(image);
	}
}

void destroyCodeBlock(CodeBlock * codeBlock) {
	if (codeBlock != NULL) {
		if (codeBlock->content != NULL) {
			free(codeBlock->content);
		}
		free(codeBlock);
	}
}

void destroyNote(Note * note) {
	if (note != NULL) {
		if (note->content != NULL) {
			free(note->content);
		}
		free(note);
	}
}

void destroyBlock(Block * block) {
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

void destroyLink(Link * link) {
	if (link != NULL) {
		if (link->text != NULL) {
			free(link->text);
		}
		if (link->target != NULL) {
			free(link->target);
		}
		free(link);
	}
}

void destroySlideItem(SlideItem * slideItem) {
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
			case SLIDE_ITEM_LINK:
				destroyLink(slideItem->link);
				break;
		}
		free(slideItem);
	}
}

void destroySlideItemList(SlideItemList * slideItemList) {
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
	if (slide != NULL) {
		if (slide->title != NULL) {
			free(slide->title);
		}
		if (slide->subtitle != NULL) {
			free(slide->subtitle);
		}
		if (slide->id != NULL) {
			free(slide->id);
		}
		destroySlideItemList(slide->items);
		free(slide);
	}
}

void destroySlideList(SlideList * slideList) {
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
	if (program != NULL) {
		destroySlideList(program->slides);
		free(program);
	}
}
