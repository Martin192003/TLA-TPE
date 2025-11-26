#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

ModuleDestructor initializeBisonActionsModule();

Text * TextSemanticAction(char * content);
Image * ImageSemanticAction(char * path, char * caption);
Image * ImageWithLegendSemanticAction(char * path, char * caption, char * legend);
CodeBlock * CodeBlockSemanticAction(char * content);
Note * NoteSemanticAction(char * content);
Block * BlockSemanticAction(BlockType type, char * title, char * content);
Link * LinkSemanticAction(char * text, char * target);

SlideItem * TextSlideItemSemanticAction(Text * text);
SlideItem * ImageSlideItemSemanticAction(Image * image);
SlideItem * CodeSlideItemSemanticAction(CodeBlock * codeBlock);
SlideItem * NoteSlideItemSemanticAction(Note * note);
SlideItem * BlockSlideItemSemanticAction(Block * block);
SlideItem * LinkSlideItemSemanticAction(Link * link);

SlideItemList * EmptySlideItemListSemanticAction();
SlideItemList * SlideItemListSemanticAction(SlideItemList * list, SlideItem * item);

Slide * SlideSemanticAction(char * title, char * subtitle, SlideItemList * items);
Slide * SimpleSlideSemanticAction(char * title, SlideItemList * items);
Slide * SlideWithoutTitleSemanticAction(SlideItemList * items);
Slide * SlideWithIdSemanticAction(char * title, char * id, SlideItemList * items);
Slide * SlideWithSubtitleAndIdSemanticAction(char * title, char * subtitle, char * id, SlideItemList * items);

SlideList * EmptySlideListSemanticAction();
SlideList * SlideListSemanticAction(SlideList * list, Slide * slide);

Program * ProgramSemanticAction(SlideList * slides);

#endif
