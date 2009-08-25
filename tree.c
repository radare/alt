/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alt.h"

#define TREE_DEPTH 32

typedef struct AltNode {
	char *str;
	int level;
	struct AltNode *up;
	struct AltNode *down;
	struct AltNode *right;
	struct AltNode *left;
} AltNode;

typedef struct AltTree {
	AltNode *root;
	AltNode *cur;
	AltNode *depth[TREE_DEPTH];
	int lastlevel;
} AltTree;

static AltNode *alt_node_new() {
	AltNode *node = (AltNode*) malloc (sizeof(AltNode));
	memset (node, 0, sizeof(AltNode));
	return node;
}

static void engine_cb_level(AltState *st, int delta, char ch) {
	AltTree *at = (AltTree *) st->user;
	if (delta>0) at->depth[st->level] = at->cur;
	else at->cur = at->depth[st->level];
	//printf ("%d ", st->level); PRINTLEVEL(st->level); if (delta>0) printf ("{\n"); else printf("}\n");
}

static void engine_cb_word(AltState *st) {
	AltTree *at = (AltTree *) st->user;
	AltNode *node = alt_node_new ();

	node->str = strdup(st->str);
	if (at->root != NULL) { /* define root node */
		if (at->lastlevel < st->level) {
			/* set child */
			at->cur->right = node;
			node->left = at->cur;
			at->cur = node;
			node->level = st->level;
		} else {
			/* add node at same nest level */
			at->cur->down = node;
			node->up = at->cur;
			at->cur = node;
			node->level = st->level;
		}
	} else at->root = at->depth[0] = at->cur = node;

	printf ("%d ", st->level);
	PRINTLEVEL(st->level);
	printf ("'%s'\n", st->str);
	at->lastlevel = st->level;
}

void alt_tree_free(AltState *st) {
	AltTree *at = (AltTree*) st->user;
	// TODO: recursive free
}

static void _alt_tree_walk(AltNode *node) {
	if (node) {
		PRINTLEVEL(node->level);
		printf(" - %d : (%s)\n", node->level, node->str);
		_alt_tree_walk (node->right);
		_alt_tree_walk (node->down);
	}
}

void alt_tree_walk(AltState *st) {
	AltTree *at = (AltTree*) st->user;
	AltNode *node = at->root;
	printf ("WALK NODES:\n");
	while (node) {
		printf (" = %d : %s\n", node->level, node->str);
		_alt_tree_walk (node->right);
		node = node->down;
	}
}

AltNode* alt_tree_resolve(AltState *st, const char *name) {
	AltTree *at = (AltTree*) st->user;
	AltNode *node = at->root;
	while (node) {
		if (!strcmp(node->str, name))
			return node;
		node = node->down;
	}
	return NULL;
}

void alt_tree(AltState *st) {
	AltTree *at = (AltTree*) malloc(sizeof(AltTree));
	at->depth[0] = at->cur = at->root = 0;
	st->user = (void *) at;
	st->cb_word = engine_cb_word;
	st->cb_level = engine_cb_level;
}
