/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "alt.h"


int cb_word(AltState *st) {
	PRINTLEVEL
	printf("(%s)\n", st->str);
}

int cb_error(AltState *st, const char *fmt, ...) {
	printf("ERROR: %s\n", fmt); // TODO: use va_
}

int main(int argc, char **argv) {
	AltState st;
	memset (&st, 0, sizeof (AltState));
	st.cb_word = cb_word;
	st.cb_error = cb_error;
	alt_tree (&st);
	if (argc>1)
		return parse_file (&st, argv[1]);
	return parse_fd (&st, 0);
}
