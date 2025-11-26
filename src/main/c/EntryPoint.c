#include "backend/code-generation/Generator.h"
#include "backend/domain-specific/SemanticAnalyzer.h"
#include "frontend/Frontend.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "support/logging/Logger.h"
#include "support/type/CompilationStatus.h"
#include "support/type/CompilerState.h"
#include "support/type/ModuleDestructor.h"

const int main(const int length, const char ** arguments) {
	LexicalAnalyzer * lexicalAnalyzer = createLexicalAnalyzer();
	Logger * logger = createLogger("EntryPoint");
	for (int k = 0; k < length; ++k) {
		(void)logger; (void)arguments;
	}
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.semanticErrors = 0
	};
	ModuleDestructor moduleDestructors[] = {
		initializeAbstractSyntaxTreeModule(),
		initializeFlexActionsModule(lexicalAnalyzer),
		initializeBisonActionsModule(&compilerState),
		initializeFrontendModule(lexicalAnalyzer),
		initializeSemanticAnalyzerModule(),
		initializeGeneratorModule()
	};
	CompilationStatus compilationStatus = executeSyntacticAnalysis();
	Program * program = compilerState.abstractSyntaxtTree;
	if (compilationStatus == SUCCEEDED) {
		CompilationStatus semanticStatus = executeSemanticAnalysis(&compilerState);
		if (semanticStatus == SUCCEEDED) {
			executeGenerator(&compilerState);
		} else {
			logError(logger, "La fase semántica rechaza el programa de entrada.");
			compilationStatus = FAILED;
		}
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}
	destroyProgram(program);
	for (int k = (sizeof(moduleDestructors)/sizeof(ModuleDestructor)) - 1; 0 <= k; --k) {
		moduleDestructors[k]();
	}
	destroyLogger(logger);
	destroyLexicalAnalyzer(lexicalAnalyzer);
	return compilationStatus;
}
