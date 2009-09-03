/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alt.h"

int alt_word_is_num(const char *str) {
	return (*str>='0' && *str<='9');
}

int alt_word_is_assign(const char *str) {
	return (!strcmp(str, "=") || !strcmp(str, "+=") || !strcmp(str, "-="));
}

int alt_word_is_op(const char *str) {
	if (str[1]=='\0')
	switch(str[0]) {
	case '+':
	case '-':
	case '*':
	case '/':
	case '^':
	case '|':
	case '&':
	case '%':
		return 1;
	}
	return 0;
}

//-----------------//

int alt_script_run(AltState *st, AltNode *node) {
	AltNode *onode = node;
	if (node == NULL)
		return 1;
	if (!*node->str) {
		// ignore
	} else
	if (*node->str=='$') {
		// variable
		if (node->down && node->down->down && alt_word_is_assign(node->down->str)) {
			//printf("ASSIGN '%s' = '%s'\n", node->str, node->down->down->str);
			setenv(node->str, node->down->down->str, 1);
			onode = node->down->down;
		}
	} else {
		if (!strcmp(node->str, "printf")) {
			node = alt_tree_child (node);
			while (node) {
				printf (node->str);
				node = node->down;
			}
		} else
		if (!strcmp(node->str, "system")) {
			if (!node->right) {
				onode = node->down;
				node = node->down;
				node = alt_tree_resolve (st, node->str);
				if (node)
					node = alt_tree_child (node);
			} else node = alt_tree_child (node);
			while (node) {
				system (node->str);
				node = node->down;
			}
		} else
		if (!strcmp(node->str, "exit")) {
			node = alt_tree_child (node);
			if (node) exit (atoi(node->str));
		} else printf ("UNKNOWN (%s)\n", node->str);
	}
	return alt_script_run (st, onode->down);
}

int alt_script(AltState *st) {
	AltTree *at = st->user;	
	AltNode *node;
	if (at == NULL)
		return st->cb_error (st, "No tree found.");

	node = alt_tree_resolve (st, "main");
	if (node == NULL)
		return st->cb_error (st, "Cannot find 'main'.");

	//alt_tree_walk(st);
	node = alt_tree_child (node);
	return alt_script_run (st, node);
}
