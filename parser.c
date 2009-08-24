/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "alt.h"


static int parse_operator(AltState *st, char *str) {
	/* compare */
#if 0
>=
<=
+=
-=
#endif
	switch (str[1]) {
	case '>':
	case '<':
	case '=':
		break;
	}
	/* assign */
	switch (*str) {
	case '=':
		break;
	case '+':
		break;
	case '-':
		break;
	}
	return 0;
}

int parse_char(AltState *st, char ch) {
	int i;
	if (ch == 0)
		return 0;

	if (st->skipuntil) {
		if (ch == st->skipuntil) {
			st->skipuntil = 0;
		} else return 1;
	}

	switch (st->mode) {
	case MODE_PARSE:
		switch (ch) {
		case '"':
			if (st->lastchar == '\\')
				st->str[st->stridx++] = ch;
			else st->mode = MODE_STRING;
			break;
		case '#':
			st->skipuntil = '\n';
			break;
		case '*':
			if (st->lastchar == '/')
				st->mode = MODE_COMMENT;
			break;
		case '/':
			break;
		case '{':
			st->level++;
			break;
		case '}':
			st->level--;
			if (st->level<0) {
				st->cb_error(st, "Error: underflow\n");
				return 0;
			}
			break;
		case '\n':
			st->line++;
		case ' ':
		case '\t':
		case '\r':
			if (st->stridx) {
				st->str[st->stridx] = 0;
				st->cb_word (st);
				memset(&st->str, 0, sizeof(st->str));
				st->stridx = 0;
			}
			break;
		default:
			st->word = 1;
			st->str[st->stridx++] = ch;
			break;
		}
		break;
	case MODE_COMMENT:
		switch (ch) {
		case '/':
			if (st->lastchar == '*') { // */ //
				st->mode = MODE_PARSE;
				st->stridx = 0;
			}
			break;
		}
		break;
	case MODE_STRING:
		if (st->lastchar == '\\') {
			switch (ch) {
			case 'n': ch = '\n'; break;
			case 't': ch = '\t'; break;
			case 'r': ch = '\r'; break;
			case 'e': ch = '\x1b'; break;
			default: st->cb_error(st, "Invalid escaped char '%c'\n", ch);
				return 1;
			}
			st->str[st->stridx-1] = ch;
		} else if (ch == '"') {
			st->mode = MODE_PARSE;
			st->str[st->stridx] = 0;
			cb_word(st);
			st->stridx = 0;
		} else st->str[st->stridx++] = ch;
		break;
	}

	st->lastchar = ch;
	return 1;
}

int parse_str(AltState *st, char *str) {
	while (parse_char (st, *str))
		str++;
	return 0;
}

int parse_fd(AltState *st, int fd) {
	int ret;
	char ch;
	do {
		ret = read (fd, &ch, 1);
		parse_char (st, ch);
	} while (ret>0);
	return (ret==-1);
}

int parse_file(AltState *st, const char *file) {
	int fd = open (file, O_RDONLY);
	if (fd != -1) {
		parse_fd (st, fd);
		close (fd);
	}
	return (fd==-1);
}
