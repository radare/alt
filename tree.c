/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <stdlib.h>
#include "alt.h"

#define TREE_DEPTH 32

typedef struct AltNode {
	char *str;
} AltNode;

typedef struct AltTree {
	int count[TREE_DEPTH];
	AltNode tree[TREE_DEPTH][TREE_DEPTH];
} AltTree;

static int engine_cb_word(AltState *st) {
	AltTree *at = (AltTree *) st->user;
	st->str[st->stridx] = 0;
	printf("%d (%s)\n", st->level, st->str);
	at->count[st->level]++;
}

void alt_tree(AltState *st) {
	int i, j;
	AltTree *at = (AltTree*) malloc(sizeof(AltTree));

	for(i=0;i<TREE_DEPTH;i++)
		at->count[i] = 0;
	for(i=0;i<TREE_DEPTH;i++)
		for(j=0;j<TREE_DEPTH;j++)
			at->tree[i][j].str = NULL;
	st->user = (void *) at;
	st->cb_word = engine_cb_word;
}
