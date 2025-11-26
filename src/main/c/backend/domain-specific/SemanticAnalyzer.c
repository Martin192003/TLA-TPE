#include "SemanticAnalyzer.h"
#include <string.h>


static Logger * _logger = NULL;

static void _shutdownSemanticAnalyzerModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeSemanticAnalyzerModule() {
    _logger = createLogger("SemanticAnalyzer");
    return _shutdownSemanticAnalyzerModule;
}


static void _logSemanticAction(const char * name) {
    (void)name; 
}

static int _findId(char ** ids, int count, const char * id) {
    for (int k = 0; k < count; ++k) {
        if (strcmp(ids[k], id) == 0) return k;
    }
    return -1;
}

static bool _checkAndAddId(char *** idsRef, int * countRef, const char * id) {
    if (id == NULL) return false; // Nada que validar
    char ** ids = *idsRef; int count = *countRef;
    if (_findId(ids, count, id) >= 0) {
        return true; // duplicado
    }
    char ** resized = realloc(ids, sizeof(char*) * (count + 1));
    if (resized == NULL) return false; 
    resized[count] = strdup(id);
    *idsRef = resized;
    *countRef = count + 1;
    return false;
}

static void _freeIds(char ** ids, int count) {
    for (int k = 0; k < count; ++k) free(ids[k]);
    free(ids);
}

static void _validateSlideItems(Slide * slide, char *** pendingLinkTargetsRef, int * pendingCountRef, CompilerState * state) {
    SlideItem * item = slide->items ? slide->items->first : NULL;
    while (item != NULL) {
        switch (item->type) {
            case SLIDE_ITEM_IMAGE: {
                Image * image = item->image;
                if (image->caption == NULL) {
                    logError(_logger, "Imagen sin caption en slide (title=%s).", slide->title ? slide->title : "<sin título>");
                    state->semanticErrors++;
                }
                break;
            }
            case SLIDE_ITEM_LINK: {
                Link * link = item->link;
                if (link->target == NULL) {
                    logError(_logger, "Link sin target definido.");
                    state->semanticErrors++;
                } else {
                    char ** arr = *pendingLinkTargetsRef; int n = *pendingCountRef;
                    char ** resized = realloc(arr, sizeof(char*) * (n + 1));
                    if (resized != NULL) {
                        resized[n] = strdup(link->target);
                        *pendingLinkTargetsRef = resized;
                        *pendingCountRef = n + 1;
                    } else {
                        logError(_logger, "Sin memoria para registrar link target.");
                        state->semanticErrors++;
                    }
                }
                break;
            }
            case SLIDE_ITEM_BLOCK: {
                Block * block = item->block;
                (void)block;
                break;
            }
            default:
                break;
        }
        item = item->next;
    }
}

static void _validateLinkTargets(char ** slideIds, int slideIdCount, char ** slideTitles, int slideTitleCount, char ** targets, int targetCount, CompilerState * state) {
    for (int k = 0; k < targetCount; ++k) {
        if (_findId(slideIds, slideIdCount, targets[k]) < 0 && _findId(slideTitles, slideTitleCount, targets[k]) < 0) {
            logError(_logger, "Link a id inexistente: %s", targets[k]);
            state->semanticErrors++;
        }
    }
}

CompilationStatus executeSemanticAnalysis(CompilerState * compilerState) {
    _logSemanticAction(__FUNCTION__);
    compilerState->semanticErrors = 0;
    Program * program = (Program*) compilerState->abstractSyntaxtTree;
    if (program == NULL || program->slides == NULL) {
        logError(_logger, "Programa vacío: no hay slides.");
        return FAILED;
    }

    char ** slideIds = NULL; int slideIdCount = 0;
    char ** slideTitles = NULL; int slideTitleCount = 0;
    char ** pendingLinkTargets = NULL; int pendingLinkCount = 0;

    Slide * current = program->slides->first;
    int index = 0;
    while (current != NULL) {
        if (current->title != NULL) {
            char ** resizedT = realloc(slideTitles, sizeof(char*) * (slideTitleCount + 1));
            if (resizedT != NULL) {
                resizedT[slideTitleCount] = strdup(current->title);
                slideTitles = resizedT; slideTitleCount++;
            }
        }
        if (current->id != NULL) {
            bool duplicated = _checkAndAddId(&slideIds, &slideIdCount, current->id);
            if (duplicated) {
                logError(_logger, "Id de slide duplicado: %s", current->id);
                compilerState->semanticErrors++;
            }
        }
        _validateSlideItems(current, &pendingLinkTargets, &pendingLinkCount, compilerState);
        current = current->next; ++index;
    }

    _validateLinkTargets(slideIds, slideIdCount, slideTitles, slideTitleCount, pendingLinkTargets, pendingLinkCount, compilerState);

    _freeIds(slideIds, slideIdCount);
    _freeIds(slideTitles, slideTitleCount);
    _freeIds(pendingLinkTargets, pendingLinkCount);

    if (compilerState->semanticErrors > 0) {
        logError(_logger, "Errores semánticos: %d", compilerState->semanticErrors);
        return FAILED;
    }
    return SUCCEEDED;
}