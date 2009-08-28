/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "alt.h"

static int parse_is_operator(char ch) {
	if (ch=='*') return 1;
	return (ch=='+'||ch=='-'||ch=='&'||ch=='='||ch=='&'||
		ch=='|'||ch=='^'||ch=='/'||ch=='%'||ch=='*');
}

void parse_pushword(AltState *st, int force) {
	if (!force && st->stridx == 0) // XXX CONFLICTIVE
		return;
	//printf("LEVELS(%d)\n", st->levels[st->level]);
	st->str[st->stridx] = 0;
	st->cb_word (st);
	memset(&st->str, 0, sizeof(st->str));
	st->stridx = 0;
}

int parse_concatchar(AltState *st, char ch) {
	st->word = 1;
	st->str[st->stridx++] = ch;
	if (st->stridx>=ALT_MAX_LEVEL)  {
		printf("ST(%s)\n", st->str);
		return st->cb_error (st, "Too long string\n");
	}
	return 1; // return 0 makes it fail
}

int parse_char(AltState *st, char ch) {
	int ret = 1;
	if (ch == 0)
		return 0;

	if (st->skipuntil) {
		if (ch == st->skipuntil)
			st->skipuntil = 0;
		else return 1;
	}

	st->curchar = ch;
	switch (st->mode) {
	case MODE_PARSE:
		switch (ch) {
		case '"':
		case '\'':
			if (st->lastchar == '\\')
				st->str[st->stridx++] = ch;
			else st->mode = MODE_STRING;
			st->endch = ch;
			break;
		case '#':
			st->skipuntil = '\n';
			break;
		case '[':
		case '(':
		case '{':
			parse_pushword (st, 1);
			st->cb_level (st, 1, ch);
			switch(ch) {
			case '{': st->levels[st->level] = '}'; break;
			case '(': st->levels[st->level] = ')'; break;
			case '[': st->levels[st->level] = ']'; break;
			}
			st->level++;
			if (st->level>ALT_MAX_LEVEL)
				return st->cb_error (st, "Too much recursivity\n");
			break;
		case ']':
		case ')':
		case '}':
			parse_pushword (st, 0);
			st->level--;
			if (st->level<0)
				return st->cb_error (st, "Level underflow\n");
			if (st->levels[st->level] != ch)
				return st->cb_error (st,
					"Unexpected closing parentesis.\n");
			st->cb_level (st, -1, ch);
			break;
		case '\n':
			st->line++;
		case ' ':
		case ',':
		case ';':
		case ':':
		//case '\'':
		case '\t':
		case '\r':
			parse_pushword (st, 0);
			break;
		default:
			if (st->lastchar == '/') {
				switch(ch) {
				case '*':
					st->mode = MODE_COMMENT;
					break;
				case '/':
					st->skipuntil = '\n';
					break;
			} else {
				if (parse_is_operator (ch)) {
					parse_pushword (st, 0);
					st->mode = MODE_OPERATOR;
					return parse_char (st, ch);
					//st->str[st->stridx++] = ch;
				} else parse_concatchar (st, ch);
			}
			break;
		}
		break;
	case MODE_OPERATOR:
		if (!parse_is_operator(ch)) {
			st->mode = MODE_PARSE;
			parse_pushword (st, 0);
			// XXX: check if return here is ok
			return parse_char (st, ch);
		//	st->str[st->stridx++] = ch;
		} else st->str[st->stridx++] = ch; //return parse_char (st, ch);
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
			case 'x': ch = 'x'; break; // TODO: fix support for binary stuff
			case '\\': ch = '\\'; break;
			case '\'': ch = '\''; break;
			case '\"': ch = '\"'; break;
			default: //return st->cb_error(st,
				printf("Invalid escaped char '%c'\n", ch);
			}
			st->str[st->stridx-1] = ch;
			if (ch == '\\') ch = 0;
		} else if (ch == st->endch) {
			parse_pushword (st, 0);
			st->mode = MODE_PARSE;
		} else ret = parse_concatchar(st, ch);
		break;
	}
	st->lastchar = ch;
	return ret;
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
		if (ret>0)
			ret = parse_char (st, ch);
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
