/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "alt.h"

void cb_level(AltState *st, int delta, char ch) {
	PRINTLEVEL(st->level);
	if (delta>0) printf("{\n");
	else printf("}\n");
}

void cb_word(AltState *st) {
	PRINTLEVEL(st->level);
	printf ("(%s)\n", st->str);
}

int cb_error(AltState *st, const char *fmt, ...) {
	printf ("ERROR: %s\n", fmt); // TODO: use va_
	return 0;
}

static int _help(const char *arg0) {
	printf ("Usage: %s [-tph] [file ...]\n"
	        "  -p   show parse tree\n"
	        "  -t   show node tree\n"
	        "  -h   this help\n", arg0);
	return 0;
}

int main(int argc, char **argv) {
	int i, ret, idx = 1, mode = 0;
	AltState st;

	while (idx<argc) {
		if (!strcmp(argv[idx], "-p")) {
			mode = 1; // show parse
		} else
		if (!strcmp(argv[idx], "-t")) {
			mode = 2; // show tree
		} else
		if (!strcmp(argv[idx], "-h"))
			return _help(argv[0]);
		else break;
		idx++;
	}

	memset (&st, 0, sizeof (AltState));
	st.cb_word = cb_word;
	st.cb_level = cb_level;
	st.cb_error = cb_error;
	if (mode == 0 || mode == 2)
		alt_tree (&st, 0);
	if (idx>=argc)
		ret = parse_fd (&st, 0);
	else
	for(i=idx;i<argc;i++) {
		ret = parse_file (&st, argv[i]);
		if (ret) return ret;
	}
	if (mode == 0)
		alt_script (&st);
	else
	if (mode == 2)
		alt_tree_walk (&st);

	return 0;
}
