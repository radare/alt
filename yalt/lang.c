#include "alt.h"
#include <stdio.h>
#include <string.h>

#define I(x...) ((const char **)&((const char *[]){x, NULL}))

int strack_len = 0;
const char *strack[128];
const char *strack_name = NULL;

struct {
	int or;
	const char *name;
	const char **list;
	int idx;
} test[] = {
	{ 1, "type", I("'void", "'char", "'int", "'long long") },
	{ 0, "varset", I("'var","type","$name","'=","$value",";") },
	{ 0, "vardef", I("'var","type","$name",";") },
	{ 0, "vardef2", I("type","$name",";") },
	{ 2, "varlist", I("type","$name", "?,", "...") },
	{ 2, "vardeflist", I("$name", "?,", "...") },
	{ 0, "signature", I("type","$name","(", "vardeflist", ")", ";") },
	{ 0, "funcall", I("$name","(", "varlist", ")", ";") },
	{ 0, "nop", I(";") },
	{ 0, NULL }
};

void reset () {
	int i;
	strack_len = 0;
	strack_name = NULL;
	for (i=0; test[i].name; i++) {
		test[i].idx = 0;
	}
}

typedef struct {
	int depth;
} AltParserState;

void match_fin () {
	int i;
	for (i=0; test[i].name; i++) {
		
	}
}

void callback () {
	int i;
	if (!strack_name) return;
	if (!strack_len) return;
	printf ("CALLBACK (%s) %d\n", strack_name, strack_len);
	for (i=0; i<strack_len; i++) {
		printf ("   - %s\n", strack[i]);
	}
	strack_len = 0;
}

int matches = 0;
#define PUSH(x) if (!matches) strack[strack_len++] = strdup (x)
int ormatch (const char *str, const char *s) {
	int i,j;
	for (i=0; test[i].name; i++) {
		if (!test[i].or) continue;
		if (!s || !strcmp (s, test[i].name)) {
			for (j = 0; test[i].list[j]; j++) {
				const char *p = test[i].list[j];
				switch (*p) {
				case '\'':
					if (!strcmp (p+1, str)) {
						PUSH (p+1);
						return 1;
					}
					break;
				default:
					printf ("wtf\n");
					break;
				}
			}
		}
	}
	return 0;
}

int listmatch (const char *str, char ch) {
	if (ch ==')')
		return 0;
	PUSH (str);
	return 1;
}

void match (const char *str, char ch) {
	int i, j;

	matches = 0;
	//printf ("-- finding match for (%s)\n", str);
	for (i=0; test[i].name; i++) {
		int idx = test[i].idx;
		const char *s;
		switch (test[i].or) {
		case 1: // OR
	//		ormatch (str, ch);
			continue;
		case 2: // LOOP LIST
			matches = listmatch (str, ch);
			continue;
		}
		if (idx==-1) continue;
		s = test[i].list[idx];
		if (s == NULL) {
			printf ("MATCH! (%s)\n", test[i].name);
			continue;
		}
//printf ("(((%s)))\n", str);
//printf ("switch %d (%s)\n", idx, test[i].list[idx]);
		switch (*s) {
		case '?':
			test[i].idx++;
			break;
		case '\'':
//printf ("compare string (%s)\n", s+1);
			if (!strcmp (s+1, str)) {
//				printf ("  MATCH (%s) %d\n",
//					test[i].name, test[i].idx);
				test[i].idx++;
printf ("PUSH MATCH\n");
				PUSH (s+1);
			} else test[i].idx = -1;
			break;
		case '$':
			PUSH (str);
			test[i].idx++;
			break;
		case ';':
			PUSH (str);
			if (!s[1]) {
				if (ch == ';') {
				printf ("##### MATCH END? #####\n");
					callback (test[i].name);
					reset ();
					matches = 1;
					goto beach;
				}
			} else printf ("wtf\n");
			break;
		default:
			if (!strcmp (s, "...")) {
				printf ("TODO: matchloop\n");
			} else {
				if (ormatch (str, s)) 
					test[i].idx++;
				else test[i].idx = -1;
			}
			break;
		}
		if (test[i].idx == -1)
			continue;
		matches++;
		strack_name = test[i].name;
		//printf ("%s\n", test[i].name);
		switch (ch) {
		case ';':
			callback (strack_name);
			break;
		case '(':
			if (test[i].list[idx+1]) {
				strack_name = test[i].name;
				if (*test[i].list[idx+1] =='(') {
					strack_name = test[i].name;
				}
			}
			callback (strack_name);
			break;
		}
		if (test[i].list[idx+1] ) {
			if (*test[i].list[idx+1] == ';') {
				if (ch == ';') {
					printf ("######## TRIGGER #######\n");
					strack_name = test[i].name;
					callback (strack_name);
					reset ();
					goto beach;
				}
#if 0
			case '(':
				printf ("OPEN BRACKET for (%s)\n", strack_name);
				listmatch (strack_name, ch);
				callback (strack);
				break;
#endif
			}
		} else {
			const char * next = test[i].list[idx+1];
			//printf ("NEXT (%s)\n", next);
		}
#if 0
		for (j=0; test[i].list[j]; j++) {
			const char *s = test[i].list[j];
			printf ("    %s\n", test[i].list[j]);
		}
#endif
	}
beach:
	if (!matches) {
		//printf ("** ERRROR **\n");
		//reset ();
	}
}

void list_all() {
	int i, j;
	for (i=0; test[i].name; i++) {
		printf ("%s\n", test[i].name);
		for (j=0; test[i].list[j]; j++) {
			printf ("    %s\n", test[i].list[j]);
		}
	}
}

void cb_level(AltState *st, int delta, char ch) {
	PRINTLEVEL (st->level);
	if (delta<0) {
		callback (strack_name);
reset ();
	}
	printf (delta>0? "%c\n": "%c\n", ch);
}

void cb_word(AltState *st, char ch) {
	char str[1024];
	PRINTLEVEL (st->level);
	printf ("(%s) '%c'\n", st->str, CHF (ch));
	match (st->str, ch);
}

AltState parser_new() {
	AltState st = {0};
	reset ();
	st.cb_word = cb_word;
	st.cb_level = cb_level;
	return st;
}

main() {
	AltState st = parser_new ();
	parse_fd (&st, 0);
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
