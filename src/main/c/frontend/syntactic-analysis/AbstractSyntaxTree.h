#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * Type definitions for the presentation language AST.
 */

typedef enum SlideItemType SlideItemType;
typedef enum BlockType BlockType;

typedef struct Text Text;
typedef struct Image Image;
typedef struct CodeBlock CodeBlock;
typedef struct Note Note;
typedef struct Block Block;
typedef struct SlideItem SlideItem;
typedef struct SlideItemList SlideItemList;
typedef struct Slide Slide;
typedef struct SlideList SlideList;
typedef struct Program Program;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum SlideItemType {
	SLIDE_ITEM_TEXT,
	SLIDE_ITEM_IMAGE,
	SLIDE_ITEM_CODE,
	SLIDE_ITEM_NOTE,
	SLIDE_ITEM_BLOCK
};

enum BlockType {
	BLOCK_NORMAL,
	BLOCK_ALERT,
	BLOCK_EXAMPLE
};

struct Text {
	char * content;
};

struct Image {
	char * path;
	char * caption;
	char * legend;    // Optional legend/footer text
};

struct CodeBlock {
	char * content;
};

struct Note {
	char * content;
};

struct Block {
	BlockType type;
	char * title;  // Can be NULL for blocks without title
	char * content;
};

struct SlideItem {
	union {
		Text * text;
		Image * image;
		CodeBlock * codeBlock;
		Note * note;
		Block * block;
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
	char * subtitle;  // Can be NULL for slides without subtitle
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

/**
 * Node recursive destructors.
 */

void destroyText(Text * text);
void destroyImage(Image * image);
void destroyCodeBlock(CodeBlock * codeBlock);
void destroyNote(Note * note);
void destroyBlock(Block * block);
void destroySlideItem(SlideItem * slideItem);
void destroySlideItemList(SlideItemList * slideItemList);
void destroySlide(Slide * slide);
void destroySlideList(SlideList * slideList);
void destroyProgram(Program * program);

#endif
