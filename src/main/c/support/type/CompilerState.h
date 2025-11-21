#ifndef COMPILER_STATE_HEADER
#define COMPILER_STATE_HEADER

/**
 * Estado global del compilador. Transporta estructuras entre fases.
 */
typedef struct {
    /**
     * Nodo raíz del AST (Program *), se mantiene void* para reducir acoplamiento
     * con headers y evitar ciclos de inclusión.
     */
    void * abstractSyntaxtTree;

    /**
     * Cantidad de errores semánticos detectados (si > 0 la generación se omite).
     */
    int semanticErrors;

    /**
     * Reservado para futuras ampliaciones (tablas de símbolos, scopes, etc.).
     */
} CompilerState;

#endif
