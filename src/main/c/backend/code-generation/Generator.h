#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdarg.h>
#include <stdio.h>

/** Inicializa el módulo de generación de código. */
ModuleDestructor initializeGeneratorModule();

/** Genera salida final (LaTeX/Beamer). */
void executeGenerator(CompilerState * compilerState);

#endif
