#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions for presentation language.
 */

// Text and content actions
Text * TextSemanticAction(char * content);
Image * ImageSemanticAction(char * path, char * caption);
Image * ImageWithLegendSemanticAction(char * path, char * caption, char * legend);
CodeBlock * CodeBlockSemanticAction(char * content);
Note * NoteSemanticAction(char * content);
Block * BlockSemanticAction(BlockType type, char * title, char * content);

// Slide item actions
SlideItem * TextSlideItemSemanticAction(Text * text);
SlideItem * ImageSlideItemSemanticAction(Image * image);
SlideItem * CodeSlideItemSemanticAction(CodeBlock * codeBlock);
SlideItem * NoteSlideItemSemanticAction(Note * note);
SlideItem * BlockSlideItemSemanticAction(Block * block);

// List actions
SlideItemList * EmptySlideItemListSemanticAction();
SlideItemList * SlideItemListSemanticAction(SlideItemList * list, SlideItem * item);

// Slide actions
Slide * SlideSemanticAction(char * title, char * subtitle, SlideItemList * items);
Slide * SimpleSlideSemanticAction(char * title, SlideItemList * items);
Slide * SlideWithoutTitleSemanticAction(SlideItemList * items);

// Slide list actions
SlideList * EmptySlideListSemanticAction();
SlideList * SlideListSemanticAction(SlideList * list, Slide * slide);

// Program action
Program * ProgramSemanticAction(SlideList * slides);

#endif
