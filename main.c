/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "alt.h"

void cb_level(AltState *st, int delta, char ch) {
	PRINTLEVEL(st->level);
	printf (delta>0? "{\n": "}\n");
}

static void strfilter (char *str, const char *src, int len) {
	int i = 0;
	for (i=0; i<len && *src; i++, src++, str++) {
		int ch = CHF (*src);
		if (ch != *src || ch == '\\') {
			*str = '\\';
			str++;
		}
		*str = ch;
	}
	*str = 0;
}

void cb_word(AltState *st, char ch) {
	char str[1024];
	PRINTLEVEL (st->level);
	strfilter (str, st->str, sizeof (str));
	printf ("(%s) '%c'\n", str, CHF (ch));
}

int cb_error(AltState *st, const char *fmt) {
	fprintf (stderr, "ERROR: %s\n", fmt);
	return 0;
}

static int _help(const char *arg0) {
	printf ("Usage: %s [-rtph] [file ...]\n"
	        "  -d   show debug\n"
	        "  -r   run script\n"
	        "  -p   show parse tree\n"
	        "  -t   show node tree\n"
	        "  -h   this help\n", arg0);
	return 0;
}

int main(int argc, char **argv) {
	int i, ret, idx = 1;
	int mode = 'p';
	AltState st;

	memset (&st, 0, sizeof (AltState));
	while (idx<argc) {
		if (argv[idx][0] != '-')
			break;
		mode = argv[idx][1];
		if (mode == 'd') st.debug = 1; else
		if (mode == 'h') return _help (argv[0]);
		idx++;
	}
	st.cb_word = cb_word;
	st.cb_level = cb_level;
	st.cb_error = cb_error;
	if (mode == 'r' || mode == 't')
		alt_tree (&st);
	if (idx>=argc)
		ret = parse_fd (&st, 0);
	else for (i=idx; i<argc; i++) {
		if (parse_file (&st, argv[i]))
			return 1;
	}
	if (mode == 'r')
		alt_script (&st);
	else if (mode == 't')
		alt_tree_walk (&st);
	alt_tree_free (&st);

	return 0;
}
