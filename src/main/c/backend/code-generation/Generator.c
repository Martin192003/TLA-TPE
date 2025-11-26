#include "Generator.h"
#include "../../support/configuration/Environment.h"
#include <string.h>
#include <ctype.h>


static Logger * _logger = NULL;
static const char _indentationCharacter = ' ';
static const char _indentationSize = 4;

static void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}


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
	size_t j = 0; for (size_t i = 0; src && src[i] && j + 2 < max; ++i) {
		if (src[i] == '_' || src[i] == '%') { dst[j++]='\\'; dst[j++]=src[i]; }
		else { dst[j++]=src[i]; }
	} dst[j]='\0';
}

static char * _sanitizeLabel(const char * raw, int index) {
	if (raw == NULL || raw[0] == '\0') {
		char buf[32]; snprintf(buf, sizeof(buf), "slide%d", index);
		return strdup(buf);
	}
	size_t len = strlen(raw);
	char * out = calloc(len * 2 + 16, 1);
	size_t j = 0; bool prevDash = false;
	for (size_t i = 0; i < len; ++i) {
		unsigned char c = (unsigned char)raw[i];
		if (isalnum(c)) {
			out[j++] = (char)tolower(c);
			prevDash = false;
		} else if (c == ' ' || c == '-' || c == '_' ) {
			if (!prevDash) { out[j++]='-'; prevDash=true; }
		} else if (c == '"') {
			continue;
		} else {
			if (!prevDash) { out[j++]='-'; prevDash=true; }
		}
	}
	if (j == 0) {
		char buf[32]; snprintf(buf, sizeof(buf), "slide%d", index);
		free(out); return strdup(buf);
	}
	if (out[j-1] == '-') out[j-1] = '\0'; else out[j]='\0';
	return out;
}

static int _findLabel(char ** labels, int count, const char * label) {
	for (int i=0;i<count;++i) if (strcmp(labels[i], label)==0) return i; return -1;
}

static char ** _collectLinkTargets(Program * program, int * targetCountRef) {
	*targetCountRef = 0;
	if (!program || !program->slides) return NULL;
	char ** targets = NULL; int tcount = 0;
	for (Slide * s = program->slides->first; s; s = s->next) {
		SlideItem * it = s->items ? s->items->first : NULL;
		while (it) {
			if (it->type == SLIDE_ITEM_LINK && it->link && it->link->target) {
				bool exists = false; for (int k=0;k<tcount;++k) if (strcmp(targets[k], it->link->target)==0) { exists=true; break; }
				if (!exists) {
					targets = realloc(targets, sizeof(char*)*(tcount+1));
					targets[tcount] = strdup(it->link->target);
					tcount++;
				}
			}
			it = it->next;
		}
	}
	*targetCountRef = tcount; return targets;
}

static void _buildLabelMap(Program * program, char *** keysRef, char *** labelRef, int * countRef, char *** slideLabelsRef, int * slideCountRef) {
	*keysRef = NULL; *labelRef = NULL; *countRef = 0; *slideLabelsRef = NULL; *slideCountRef = 0;
	if (program == NULL || program->slides == NULL) return;
	int targetCount = 0; char ** targets = _collectLinkTargets(program, &targetCount);
	/* Contar slides */
	int n=0; for (Slide * s=program->slides->first; s; s=s->next) n++;
	char ** slideLabels = calloc(n, sizeof(char*));
	char ** keys = NULL; char ** labels = NULL; int kcount=0;
	int index=0; for (Slide * s=program->slides->first; s; s=s->next, ++index) {
		bool needed = false;
		if (targetCount > 0) {
			for (int t=0;t<targetCount && !needed;++t) {
				if ((s->id && strcmp(s->id, targets[t])==0) || (s->title && strcmp(s->title, targets[t])==0)) {
					needed = true; break;
				}
			}
		}
		if (!needed) {
			slideLabels[index] = NULL; continue;
		}
		char * baseLabel = NULL;
		if (s->id) {
			baseLabel = strdup(s->id);
		} else if (s->title) {
			baseLabel = _sanitizeLabel(s->title, index+1);
		} else {
			baseLabel = _sanitizeLabel(NULL, index+1);
		}
		if (_findLabel(slideLabels, index, baseLabel) >= 0) {
			char buf[256]; int suffix=2;
			do { snprintf(buf, sizeof(buf), "%s-%d", baseLabel, suffix++); } while (_findLabel(slideLabels, index, buf) >= 0);
			free(baseLabel); baseLabel = strdup(buf);
		}
		slideLabels[index] = baseLabel;
		if (s->id) {
			keys = realloc(keys, sizeof(char*)*(kcount+1)); labels = realloc(labels, sizeof(char*)*(kcount+1));
			keys[kcount] = strdup(s->id); labels[kcount] = baseLabel; kcount++;
		}
		if (s->title) {
			keys = realloc(keys, sizeof(char*)*(kcount+1)); labels = realloc(labels, sizeof(char*)*(kcount+1));
			keys[kcount] = strdup(s->title); labels[kcount] = baseLabel; kcount++;
		}
	}
	for (int t=0;t<targetCount;++t) free(targets[t]); free(targets);
	*keysRef = keys; *labelRef = labels; *countRef = kcount; *slideLabelsRef = slideLabels; *slideCountRef = n;
}

static const char * _resolveLabel(char ** keys, char ** labels, int count, const char * target) {
	if (!target) return NULL;
	for (int i=0;i<count;++i) if (strcmp(keys[i], target)==0) return labels[i];
	return NULL;
}

static void _freeLabelMap(char ** keys, char ** labels, int count, char ** slideLabels, int slideCount) {
	for (int i=0;i<count;++i) free(keys[i]); free(keys); free(labels);
	for (int i=0;i<slideCount;++i) free(slideLabels[i]); free(slideLabels);
}

static char * _formatCode(const char * raw) {
	if (raw == NULL) return strdup("");
	const char * style = getStringOrDefault("CODE_INDENT_STYLE", "spaces");
	const char * mode  = getStringOrDefault("CODE_INDENT_MODE", "original");
	int indentSize = atoi(getStringOrDefault("CODE_INDENT_SIZE", "4"));
	const bool doubleSpace = strcmp(getStringOrDefault("CODE_DOUBLE_SPACE", "false"), "true") == 0;
	if (indentSize <= 0) indentSize = 4;
	char indentChar = (strcmp(style, "tabs") == 0) ? '\t' : ' ';

	if (strcmp(mode, "original") == 0) {
		size_t len = strlen(raw);
		char * out = calloc(len * 2 + 2, 1);
		const char * p = raw;
		while (*p) {
			const char * lineStart = p;
			while (*p && *p != '\n' && *p != '\r') p++;
			size_t lineLen = (size_t)(p - lineStart);
			if (lineLen) strncat(out, lineStart, lineLen);
			strcat(out, "\n");
			if (*p == '\r') p++;
			if (*p == '\n') p++;
		}
		return out;
	}

	size_t len = strlen(raw);
	size_t cap = len * (doubleSpace ? 3 : 2) + 256;
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

		if (lineLen > 0 && line[lineLen-1] == '\r') line[lineLen-1] = '\0';
		size_t originalIndentChars = 0; while (line[originalIndentChars] == ' ' || line[originalIndentChars] == '\t') originalIndentChars++;
		size_t contentStart = originalIndentChars;

		size_t kscan = contentStart; if (line[kscan] == '}' && level > 0 && strcmp(mode, "auto") == 0) level--;

		int effectiveIndent;
		effectiveIndent = level;

		size_t need = strlen(out) + (lineLen + 1) * (doubleSpace ? 2 : 1) + 16 + (effectiveIndent >= 0 ? (indentChar=='\t'?effectiveIndent:effectiveIndent*indentSize) : originalIndentChars) + 8;
		if (need >= cap) { cap = need + 256; out = realloc(out, cap); }

		int indentUnits = (indentChar == '\t') ? effectiveIndent : effectiveIndent * indentSize;
		for (int i = 0; i < indentUnits; ++i) {
			if (indentChar == '\t') strcat(out, "\t");
			else { size_t l = strlen(out); out[l] = indentChar; out[l+1] = '\0'; }
		}

		strcat(out, line + contentStart);
		strcat(out, "\n");
		if (doubleSpace) strcat(out, "\n");

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
		if (*p == '\n') p++;
	}
	return out;
}

static void _genLatexSlide(Slide * slide, const char * slideLabel, char ** mapKeys, char ** mapLabels, int mapCount) {
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
	if (slideLabel) {
		_out(2, "\\label{%s}\n", slideLabel);
	}
	SlideItem * item = slide->items ? slide->items->first : NULL;
	int numberedCount = 0;
	while (item) {
		if (item->type == SLIDE_ITEM_TEXT && item->text && item->text->listType != LIST_NONE) {
			ListType currentListType = item->text->listType;
			_out(2, "\\begin{%s}\n", currentListType == LIST_NUMBERED ? "enumerate" : "itemize");
			if (currentListType == LIST_NUMBERED && numberedCount > 0) {
				_out(3, "\\setcounter{enumi}{%d}\n", numberedCount);
			}
			while (item && item->type == SLIDE_ITEM_TEXT && item->text && item->text->listType == currentListType) {
				_out(3, "\\item %s\n", item->text->content);
				if (currentListType == LIST_NUMBERED) { numberedCount++; }
				item = item->next;
			}
			_out(2, "\\end{%s}\n", currentListType == LIST_NUMBERED ? "enumerate" : "itemize");
			continue;
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
							bool firstBlankLine = (lineEnd == lineStart && lineStart == formatted);
							const char * nextLine = lineEnd + 1;
							while (*nextLine && (*nextLine == ' ' || *nextLine == '\t' || *nextLine == '\r' || *nextLine == '\n')) nextLine++;
							if (firstBlankLine) {
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
				bool hasTitle = (b->title && b->title[0] != '\0');
				if (hasTitle) {
					_out(2, "\\begin{%s}{%s}\n%s\n\\end{%s}\n", env, b->title, b->content?b->content:"", env);
				} else {
					if (b->type == BLOCK_EXAMPLE) {
						_out(2, "\\begin{%s} {}\n%s\n\\end{%s}\n", env, b->content?b->content:"", env);
					} else if (b->type == BLOCK_ALERT) {
						_out(2, "\\begin{%s} {}\n%s\n\\end{%s}\n", env, b->content?b->content:"", env);
					} else {
						_out(2, "\\begin{%s}\n%s\n\\end{%s}\n", env, b->content?b->content:"", env);
					}
				}
				break; }
			case SLIDE_ITEM_LINK:
				if (item->link && item->link->text && item->link->target) {
					const char * resolved = _resolveLabel(mapKeys, mapLabels, mapCount, item->link->target);
					_out(2, "\\hyperlink{%s}{%s}\n", resolved?resolved:item->link->target, item->link->text);
				}
				break;
		}
		item = item->next;
	}
	_out(1, "\\end{frame}\n\n");
}

static void _generateLatex(Program * program) {
	_out(0, "\\documentclass{beamer}\n");
	_out(0, "\\usepackage[utf8]{inputenc}\n\\usepackage[T1]{fontenc}\n\\usepackage{graphicx}\n\\usepackage{hyperref}\n");
	_out(0, "\\usetheme{Madrid}\n");
	_out(0, "\\setbeamertemplate{enumerate items}[default]\n\n");
	char ** mapKeys=NULL, ** mapLabels=NULL, ** slideLabels=NULL; int mapCount=0, slideCount=0;
	_buildLabelMap(program, &mapKeys, &mapLabels, &mapCount, &slideLabels, &slideCount);
	_out(0, "\\begin{document}\n\n");
	Slide * s = program->slides->first; int idx=0; while (s) { _genLatexSlide(s, slideLabels[idx], mapKeys, mapLabels, mapCount); s = s->next; idx++; }
	_freeLabelMap(mapKeys, mapLabels, mapCount, slideLabels, slideCount);
	_out(0, "\\end{document}\n");
}


void executeGenerator(CompilerState * compilerState) {
	Program * program = (Program*) compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "No hay AST para generar.");
		return;
	}
	_generateLatex(program);
}
