#include "SemanticAnalyzer.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown del módulo. */
static void _shutdownSemanticAnalyzerModule() {
    if (_logger != NULL) {
        logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeSemanticAnalyzerModule() {
    _logger = createLogger("SemanticAnalyzer");
    return _shutdownSemanticAnalyzerModule;
}

/* PRIVATE HELPERS */

static void _logSemanticAction(const char * name) {
    logDebugging(_logger, "%s", name);
}

/** Busca id en arreglo lineal. */
static int _findId(char ** ids, int count, const char * id) {
    for (int k = 0; k < count; ++k) {
        if (strcmp(ids[k], id) == 0) return k;
    }
    return -1;
}

/** Agrega id si no existe; retorna true si duplicado. */
static bool _checkAndAddId(char *** idsRef, int * countRef, const char * id) {
    if (id == NULL) return false; // Nada que validar
    char ** ids = *idsRef; int count = *countRef;
    if (_findId(ids, count, id) >= 0) {
        return true; // duplicado
    }
    char ** resized = realloc(ids, sizeof(char*) * (count + 1));
    if (resized == NULL) return false; // sin memoria, lo omitimos (se detectará luego si es crítico)
    resized[count] = strdup(id);
    *idsRef = resized;
    *countRef = count + 1;
    return false;
}

/** Libera arreglo de ids. */
static void _freeIds(char ** ids, int count) {
    for (int k = 0; k < count; ++k) free(ids[k]);
    free(ids);
}

/** Valida items de un slide. */
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
                    // Guardamos el target para validar después.
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
                if (block->type != BLOCK_NORMAL && block->title == NULL) {
                    logError(_logger, "Bloque especial sin título (tipo=%d).", (int)block->type);
                    state->semanticErrors++;
                }
                break;
            }
            default:
                break; // Otros tipos no tienen validaciones extra (por ahora)
        }
        item = item->next;
    }
}

/** Valida que todos los link targets existan entre los ids de slides. */
static void _validateLinkTargets(char ** slideIds, int slideIdCount, char ** targets, int targetCount, CompilerState * state) {
    for (int k = 0; k < targetCount; ++k) {
        if (_findId(slideIds, slideIdCount, targets[k]) < 0) {
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

    // Estructuras auxiliares.
    char ** slideIds = NULL; int slideIdCount = 0;
    char ** pendingLinkTargets = NULL; int pendingLinkCount = 0;

    // Recorrido de slides.
    Slide * current = program->slides->first;
    int index = 0;
    while (current != NULL) {
        // Validación de título opcional: solo log informativo si falta.
        if (current->title == NULL) {
            logDebugging(_logger, "Slide %d sin título (permitido).", index);
        }
        // Id duplicado.
        if (current->id != NULL) {
            bool duplicated = _checkAndAddId(&slideIds, &slideIdCount, current->id);
            if (duplicated) {
                logError(_logger, "Id de slide duplicado: %s", current->id);
                compilerState->semanticErrors++;
            }
        }
        // Validar items.
        _validateSlideItems(current, &pendingLinkTargets, &pendingLinkCount, compilerState);
        current = current->next; ++index;
    }

    // Validar targets de links contra ids de slides.
    _validateLinkTargets(slideIds, slideIdCount, pendingLinkTargets, pendingLinkCount, compilerState);

    _freeIds(slideIds, slideIdCount);
    _freeIds(pendingLinkTargets, pendingLinkCount);

    if (compilerState->semanticErrors > 0) {
        logError(_logger, "Errores semánticos: %d", compilerState->semanticErrors);
        return FAILED;
    }
    logDebugging(_logger, "Análisis semántico OK.");
    return SUCCEEDED;
}