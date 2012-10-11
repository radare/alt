#include <stdio.h>

struct {
	int idx;
	const char *name;
	const char *list;
	const char *buf;
} test[] = {
	{ -1, "type", "|'void\0|'char\0|'int\0" },
	{ -1, "vardef", "'var\0type\0$name\0;\0" },
	{ -1, "varset", "'var'\ttype\t$name\t'='\tvalue\t;" },
	{ -1, NULL }
};

void reset () {
	int i;
	for (i=0; test[i].name; i++) {
		test[i].idx = -1;
	}
}

typedef struct {
	int depth;
} AltParserState;

void match () {
}

void reps () {
}

int depth = 0;

main() {
	int i;
	for (i=0; test[i].name; i++) {
		printf ("%s\n", test[i].name);
	}
}

#if 0
void onword (AltState *a, char w) {
	int do_or = 0;
	char *word = st->str;
	if (*word=='|') {
		do_or = 1;
		word++;
	}
	for (i=0;test[i].name; i++) {
		if (test[i].idx==-2) continue;
		if (!test[i].ptr) test[i].ptr = test[i].list;
		switch (test[i].ptr) {
		case '\'':
			// compare string
			if (!strcmp (word, test[i].ptr+1)) {
				test[i].ptr += strlen (test[i]) + 1;
				if (test[i].ptr[0])
					test[i].idx++;
				else test[i].idx = -2;
			} else {
				if (do_or) {
					test[i].ptr += strlen (test[i]) + 1;
					if (test[i].ptr[0])
						test[i].idx++;
				} else test[i].idx = -2;
			}
			break;
		case ';':
			// finnish the statement
			break;
		case '$':
			// set value name
//			setvar (
			break;
		default:
			// reference another rule
			break;
		}
	}
{
	int i;
	int max = 0;
	int maxi = 0;
	for (i=0; test[i].name; i++) {
		if (!test[i].ptr[0] && test[i].idx>max) {
			maxi = i;
			max = test[i].idx;
		}
	}
	// matching rule is 'maxi'
}
}

for () {
	getkeyword ()
}
#endif
