#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"

/** Inicializa el módulo de análisis semántico. */
ModuleDestructor initializeSemanticAnalyzerModule();

/** Ejecuta las validaciones semánticas sobre el AST. */
CompilationStatus executeSemanticAnalysis(CompilerState * compilerState);

#endif