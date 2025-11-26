#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

ModuleDestructor initializeAbstractSyntaxTreeModule();

typedef enum SlideItemType SlideItemType;
typedef enum BlockType BlockType;

typedef struct Text Text;
typedef struct Image Image;
typedef struct CodeBlock CodeBlock;
typedef struct Note Note;
typedef struct Block Block;
typedef struct Link Link;
typedef struct SlideItem SlideItem;
typedef struct SlideItemList SlideItemList;
typedef struct Slide Slide;
typedef struct SlideList SlideList;
typedef struct Program Program;

enum SlideItemType {
	SLIDE_ITEM_TEXT,
	SLIDE_ITEM_IMAGE,
	SLIDE_ITEM_CODE,
	SLIDE_ITEM_NOTE,
	SLIDE_ITEM_BLOCK,
	SLIDE_ITEM_LINK
};

enum BlockType {
	BLOCK_NORMAL,
	BLOCK_ALERT,
	BLOCK_EXAMPLE
};

typedef enum ListType ListType;
enum ListType {
    LIST_NONE,
    LIST_BULLET,
    LIST_NUMBERED
};

struct Text {
	char * content;
	ListType listType;
};

struct Image {
	char * path;
	char * caption;
	char * legend;
};

struct CodeBlock {
	char * content;
};

struct Note {
	char * content;
};

struct Block {
	BlockType type;
	char * title;
	char * content;
};

struct Link {
	char * text;
	char * target;
};

struct SlideItem {
	union {
		Text * text;
		Image * image;
		CodeBlock * codeBlock;
		Note * note;
		Block * block;
		Link * link;
	};
	SlideItemType type;
	SlideItem * next;
};

struct SlideItemList {
	SlideItem * first;
	SlideItem * last;
};

struct Slide {
	char * title;
	char * subtitle;
	char * id;
	SlideItemList * items;
	Slide * next;
};

struct SlideList {
	Slide * first;
	Slide * last;
};

struct Program {
	SlideList * slides;
};

void destroyText(Text * text);
void destroyImage(Image * image);
void destroyCodeBlock(CodeBlock * codeBlock);
void destroyNote(Note * note);
void destroyBlock(Block * block);
void destroyLink(Link * link);
void destroySlideItem(SlideItem * slideItem);
void destroySlideItemList(SlideItemList * slideItemList);
void destroySlide(Slide * slide);
void destroySlideList(SlideList * slideList);
void destroyProgram(Program * program);

#endif
