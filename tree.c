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
	int count[TREE_DEPTH];
	AltNode *root;
	AltNode *cur;
	AltNode *depth[TREE_DEPTH];
	int lastlevel;
} AltTree;

static AltNode *alt_node_new() {
	AltNode *node = (AltNode*) malloc(sizeof(AltNode));
	memset (node, 0, sizeof(AltNode));
	return node;
}

static void engine_cb_level(AltState *st, int delta) {
	printf ("%d ", st->level);
	PRINTLEVEL
	if (delta>0) printf("{\n");
	else printf("}\n");
}

static void engine_cb_word(AltState *st) {
	AltTree *at = (AltTree *) st->user;
//if (at->lastlevel > st->level) { printf("left\n"); } else
if (at->lastlevel < st->level) { printf("->\n");
} else printf("-v\n");
	printf ("%d ", st->level);
	PRINTLEVEL;
	printf ("'%s'\n", st->str);
	at->lastlevel = st->level;
#if old
	AltNode *node;
	st->str[st->stridx] = 0;
	printf ("%d (%s)\n", st->level, st->str);
	node = &at->tree[at->count[st->level]][st->level];
	node->str = strdup (st->str);
	at->count[st->level]++;
#endif
}

void alt_tree(AltState *st) {
	AltTree *at = (AltTree*) malloc(sizeof(AltTree));
	at->depth[0] = at->cur = at->root = alt_node_new ();
	st->user = (void *) at;
	st->cb_word = engine_cb_word;
	st->cb_level = engine_cb_level;
}
