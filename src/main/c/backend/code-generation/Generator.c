/** Nuevo generador para el DSL de presentaciones. */
#include "Generator.h"
#include "../../support/configuration/Environment.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static const char _indentationCharacter = ' ';
static const char _indentationSize = 4;

/** Shutdown del módulo. */
static void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}

/* HELPERS */

static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

static void _out(unsigned int lvl, const char * fmt, ...) {
	va_list args; va_start(args, fmt);
	char * ind = _indentation(lvl);
	char * eff = concatenate(2, ind, fmt);
	vfprintf(stdout, eff, args);
	fflush(stdout);
	free(eff); free(ind);
	va_end(args);
}

static void _escapeLatex(char * dst, const char * src, size_t max) {
	// Versión súper básica: solo sustituye _ y %.
	size_t j = 0; for (size_t i = 0; src && src[i] && j + 2 < max; ++i) {
		if (src[i] == '_' || src[i] == '%') { dst[j++]='\\'; dst[j++]=src[i]; }
		else { dst[j++]=src[i]; }
	} dst[j]='\0';
}

/* Formatea código estilo C/JS con indentación basada en llaves. */
static char * _formatCode(const char * raw) {
	if (raw == NULL) return strdup("");
	/* Configuración vía variables de entorno. */
	const char * style = getStringOrDefault("CODE_INDENT_STYLE", "spaces");
	const char * mode  = getStringOrDefault("CODE_INDENT_MODE", "original"); /* auto | original */
	int indentSize = atoi(getStringOrDefault("CODE_INDENT_SIZE", "4"));
	const bool doubleSpace = strcmp(getStringOrDefault("CODE_DOUBLE_SPACE", "false"), "true") == 0;
	if (indentSize <= 0) indentSize = 4;
	char indentChar = (strcmp(style, "tabs") == 0) ? '\t' : ' ';

	/* Modo "original": no tocar indentación, solo garantizar que cada línea termine en '\n'. */
	if (strcmp(mode, "original") == 0) {
		/* Reservar buffer: cada línea al menos agrega un '\n'. */
		size_t len = strlen(raw);
		char * out = calloc(len * 2 + 2, 1);
		const char * p = raw;
		while (*p) {
			const char * lineStart = p;
			while (*p && *p != '\n' && *p != '\r') p++;
			size_t lineLen = (size_t)(p - lineStart);
			if (lineLen) strncat(out, lineStart, lineLen);
			/* Siempre terminar la línea con '\n' incluso si el original no la tenía. */
			strcat(out, "\n");
			if (*p == '\r') p++; /* ignorar CR */
			if (*p == '\n') p++; /* saltar LF */
		}
		return out;
	}

	size_t len = strlen(raw);
	size_t cap = len * (doubleSpace ? 3 : 2) + 256; /* capacidad inicial generosa */
	char * out = calloc(cap, 1);
	int level = 0;
	const char * p = raw;
	bool firstLineEmitted = false;
	while (*p) {
		const char * lineStart = p;
		while (*p && *p != '\n') p++;
		size_t lineLen = (size_t)(p - lineStart);
		char * line = calloc(lineLen + 1, 1);
		memcpy(line, lineStart, lineLen);
		line[lineLen] = '\0';

		/* Remover CR final (Windows). */
		if (lineLen > 0 && line[lineLen-1] == '\r') line[lineLen-1] = '\0';

		/* Ya no saltamos la primera línea vacía: se conserva exactamente si existe. */

		/* Detectar indentación original. */
		size_t originalIndentChars = 0; while (line[originalIndentChars] == ' ' || line[originalIndentChars] == '\t') originalIndentChars++;
		size_t contentStart = originalIndentChars;

		/* Si la línea (sin espacios) empieza con '}' reducimos nivel antes de imprimir cuando estamos en modo auto. */
		size_t kscan = contentStart; if (line[kscan] == '}' && level > 0 && strcmp(mode, "auto") == 0) level--;

		/* Calcular indentación efectiva. */
		int effectiveIndent;
		/* En modo auto aplicamos indent calculada, en modo original no llegamos aquí (return temprano). */
		effectiveIndent = level;

		/* Expandir buffer si va a quedar chico. */
		size_t need = strlen(out) + (lineLen + 1) * (doubleSpace ? 2 : 1) + 16 + (effectiveIndent >= 0 ? (indentChar=='\t'?effectiveIndent:effectiveIndent*indentSize) : originalIndentChars) + 8;
		if (need >= cap) { cap = need + 256; out = realloc(out, cap); }

		/* Construir indentación. */
		int indentUnits = (indentChar == '\t') ? effectiveIndent : effectiveIndent * indentSize;
		for (int i = 0; i < indentUnits; ++i) {
			if (indentChar == '\t') strcat(out, "\t");
			else { size_t l = strlen(out); out[l] = indentChar; out[l+1] = '\0'; }
		}

		/* Copiar contenido sin modificar. */
		strcat(out, line + contentStart);
		strcat(out, "\n");
		if (doubleSpace) strcat(out, "\n");

		/* Ajustar nivel según llaves (modo auto). */
		if (strcmp(mode, "auto") == 0) {
			int opens = 0, closes = 0;
			for (size_t c = contentStart; line[c]; ++c) {
				if (line[c] == '{') opens++;
				else if (line[c] == '}') closes++;
			}
			level += opens - closes;
			if (level < 0) level = 0;
		}

		firstLineEmitted = true;
		free(line);
		if (*p == '\n') p++; /* saltar el '\n' */
	}
	return out;
}

static void _genLatexSlide(Slide * slide) {
	char titleBuf[1024]; char subBuf[1024];
	if (slide->title) _escapeLatex(titleBuf, slide->title, sizeof(titleBuf)); else strcpy(titleBuf, "");
	if (slide->subtitle) _escapeLatex(subBuf, slide->subtitle, sizeof(subBuf)); else strcpy(subBuf, "");
	if (slide->title && slide->subtitle) {
		_out(1, "\\begin{frame}{%s}{%s}%s\n", titleBuf, subBuf, slide->id?"":"");
	} else if (slide->title) {
		_out(1, "\\begin{frame}{%s}%s\n", titleBuf, slide->id?"":"");
	} else {
		_out(1, "\\begin{frame}%s\n", slide->id?"":"");
	}
	if (slide->id) {
		_out(2, "\\label{%s}\n", slide->id);
	}
	SlideItem * item = slide->items ? slide->items->first : NULL;
	int numberedCount = 0; /* contador para continuar enumeraciones a través de listas mixtas */
	while (item) {
		/* Agrupación de listas consecutivas */
		if (item->type == SLIDE_ITEM_TEXT && item->text && item->text->listType != LIST_NONE) {
			ListType currentListType = item->text->listType;
			_out(2, "\\begin{%s}\n", currentListType == LIST_NUMBERED ? "enumerate" : "itemize");
			if (currentListType == LIST_NUMBERED && numberedCount > 0) {
				/* Continuar numeración previa: enumi = numberedCount */
				_out(3, "\\setcounter{enumi}{%d}\n", numberedCount);
			}
			while (item && item->type == SLIDE_ITEM_TEXT && item->text && item->text->listType == currentListType) {
				_out(3, "\\item %s\n", item->text->content);
				if (currentListType == LIST_NUMBERED) { numberedCount++; }
				item = item->next;
			}
			_out(2, "\\end{%s}\n", currentListType == LIST_NUMBERED ? "enumerate" : "itemize");
			continue; /* continuamos con el siguiente item ya avanzado */
		}
		switch (item->type) {
			case SLIDE_ITEM_TEXT:
				if (item->text && item->text->content)
					_out(2, "%s\\par\n", item->text->content);
				break;
			case SLIDE_ITEM_IMAGE:
				if (item->image) {
					_out(2, "\\begin{figure}[h]\n");
					_out(3, "\\centering\\includegraphics[width=0.7\\textwidth]{%s}\n", item->image->path);
					if (item->image->caption)
						_out(3, "\\caption{%s}\n", item->image->caption);
					if (item->image->legend)
						_out(3, "\\footnotesize %s\\par\n", item->image->legend);
					_out(2, "\\end{figure}\n");
				}
				break;
			case SLIDE_ITEM_CODE:
				if (item->codeBlock && item->codeBlock->content) {
					char * formatted = _formatCode(item->codeBlock->content);
					_out(2, "\\begin{verbatim}\n");
					const char * lineStart = formatted;
					while (*lineStart) {
						const char * lineEnd = lineStart;
						while (*lineEnd && *lineEnd != '\n') lineEnd++;
						const char * p = lineStart;
						while (p < lineEnd) {
							const char * start = p;
							while (p < lineEnd && *p != '\t' && *p != '{' && *p != '}') p++;
							if (p > start) {
								char * chunk = strndup(start, p - start);
								_out(0, "%s", chunk);
								free(chunk);
							}
							if (p < lineEnd) {
								if (*p == '{') _out(0, "\\{");
								else if (*p == '}') _out(0, "\\}");
								else if (*p == '\t') _out(0, "\\qquad ");
								p++;
							}
						}
						if (*lineEnd == '\n') {
							bool firstBlankLine = (lineEnd == lineStart && lineStart == formatted); /* línea vacía inicial */
							const char * nextLine = lineEnd + 1;
							while (*nextLine && (*nextLine == ' ' || *nextLine == '\t' || *nextLine == '\r' || *nextLine == '\n')) nextLine++;
							if (firstBlankLine) {
								/* No agregar \\ en la primera línea vacía del bloque */
								_out(0, "\n");
							} else if (*nextLine) {
								_out(0, "\\\\\n");
							} else {
								_out(0, "\n");
							}
						}
						lineStart = lineEnd;
						if (*lineStart == '\n') lineStart++;
					}
					free(formatted);
					_out(2, "\\end{verbatim}\n");
				}
				break;
			case SLIDE_ITEM_NOTE:
				if (item->note && item->note->content)
					_out(2, "\\note{%s}\n", item->note->content);
				break;
			case SLIDE_ITEM_BLOCK: {
				Block * b = item->block;
				const char * env = "block";
				if (b->type == BLOCK_ALERT) env = "alertblock"; else if (b->type == BLOCK_EXAMPLE) env = "exampleblock";
				if (b->title)
					_out(2, "\\begin{%s}{%s}\n%s\n\\end{%s}\n", env, b->title, b->content?b->content:"", env);
				else
					_out(2, "\\begin{%s}\n%s\n\\end{%s}\n", env, b->content?b->content:"", env);
				break; }
			case SLIDE_ITEM_LINK:
				if (item->link && item->link->text && item->link->target)
					_out(2, "\\hyperlink{%s}{%s}\n", item->link->target, item->link->text);
				break;
		}
		item = item->next;
	}
	_out(1, "\\end{frame}\n\n");
}

static void _generateLatex(Program * program) {
	_out(0, "\\documentclass{beamer}\n");
	_out(0, "\\usepackage[utf8]{inputenc}\n\\usepackage[T1]{fontenc}\n\\usepackage{graphicx}\n\\usepackage{hyperref}\n");
	/* Asegurar que enumerate muestre números explícitos (no bullets de tema). */
	_out(0, "\\setbeamertemplate{enumerate items}[default]\n\n");
	_out(0, "\\begin{document}\n\n");
	Slide * s = program->slides->first; while (s) { _genLatexSlide(s); s = s->next; }
	_out(0, "\\end{document}\n");
}

static void _generateHtml(Program * program) {
	_out(0, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\"/>\n<title>Presentation</title>\n</head>\n<body>\n<div class=\"slides\">\n");
	Slide * s = program->slides->first; while (s) {
		_out(1, "<section>\n");
		if (s->title) _out(2, "<h2>%s</h2>\n", s->title);
		if (s->subtitle) _out(2, "<h3>%s</h3>\n", s->subtitle);
		SlideItem * item = s->items ? s->items->first : NULL;
		int numberedCount = 0; /* contador para continuar numeración en listas mixtas */
		while (item) {
			if (item->type == SLIDE_ITEM_TEXT && item->text && item->text->listType != LIST_NONE) {
				ListType currentListType = item->text->listType;
				if (currentListType == LIST_NUMBERED && numberedCount > 0) {
					_out(2, "<ol start=\"%d\">\n", numberedCount + 1);
				} else {
					_out(2, "<%s>\n", currentListType == LIST_NUMBERED ? "ol" : "ul");
				}
				while (item && item->type == SLIDE_ITEM_TEXT && item->text && item->text->listType == currentListType) {
					_out(3, "<li>%s</li>\n", item->text->content);
					if (currentListType == LIST_NUMBERED) { numberedCount++; }
					item = item->next;
				}
				_out(2, "</%s>\n", currentListType == LIST_NUMBERED ? "ol" : "ul");
				continue;
			}
			switch (item->type) {
				case SLIDE_ITEM_TEXT:
					if (item->text && item->text->content) _out(2, "<p>%s</p>\n", item->text->content);
					break;
				case SLIDE_ITEM_IMAGE:
					if (item->image) {
						_out(2, "<figure>\n");
						_out(3, "<img src=\"%s\" alt=\"%s\" style=\"max-width:70%%\"/>\n", item->image->path, item->image->caption?item->image->caption:"image");
						if (item->image->caption) _out(3, "<figcaption>%s</figcaption>\n", item->image->caption);
						if (item->image->legend) _out(3, "<small>%s</small>\n", item->image->legend);
						_out(2, "</figure>\n");
					}
					break;
				case SLIDE_ITEM_CODE:
					if (item->codeBlock && item->codeBlock->content) _out(2, "<pre><code>%s</code></pre>\n", item->codeBlock->content);
					break;
				case SLIDE_ITEM_BLOCK:
					if (item->block) {
						const char * cls = "block";
						if (item->block->type == BLOCK_ALERT) cls = "alert"; else if (item->block->type == BLOCK_EXAMPLE) cls = "example";
						if (item->block->title) _out(2, "<div class=\"%s\"><strong>%s</strong><p>%s</p></div>\n", cls, item->block->title, item->block->content?item->block->content:""
						); else _out(2, "<div class=\"%s\"><p>%s</p></div>\n", cls, item->block->content?item->block->content:"");
					}
					break;
				case SLIDE_ITEM_NOTE:
					if (item->note && item->note->content) _out(2, "<!-- speaker note: %s -->\n", item->note->content);
					break;
				case SLIDE_ITEM_LINK:
					if (item->link && item->link->text && item->link->target) _out(2, "<a href=\"#%s\">%s</a>\n", item->link->target, item->link->text);
					break;
			}
			item = item->next;
		}
		_out(1, "</section>\n");
		s = s->next;
	}
	_out(0, "</div>\n</body>\n</html>\n");
}

void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	Program * program = (Program*) compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "No hay AST para generar.");
		return;
	}
	const char * format = getStringOrDefault("OUTPUT_FORMAT", "latex");
	if (strcmp(format, "html") == 0) {
		_generateHtml(program);
	} else {
		_generateLatex(program);
	}
	logDebugging(_logger, "Generación terminada.");
}
