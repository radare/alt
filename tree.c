/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alt.h"

static AltNode *alt_node_new() {
	AltNode *node = (AltNode*) malloc (sizeof(AltNode));
	memset (node, 0, sizeof (AltNode));
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
	AltNode *node;

	if (at->laststr && !*st->str && !*at->laststr)
		return;
	node = alt_node_new ();

	node->str = strdup(st->str);
	at->laststr = node->str;
	if (at->root == NULL)  /* define root node */
		at->root = at->depth[0] = at->cur = node;
	if (at->lastlevel < st->level) {
		/* set child */
		at->cur->right = node;
		node->left = at->cur;
		node->level = st->level;
		at->cur = node;
	} else {
		/* add node at same nest level */
		if (node != at->root) // do not infinite loop when only one node
			at->cur->down = node;
		node->up = at->cur;
		node->level = st->level;
		at->cur = node;
	} 
#if DEBUG
	printf ("%d ", st->level);
	PRINTLEVEL (st->level);
	printf ("'%s'\n", st->str);
#endif
	at->lastlevel = st->level;
}

void alt_tree_free(AltState *st) {
	//AltTree *at = (AltTree*) st->user;
	// TODO: recursive free
}

static void _alt_tree_walk(AltNode *node) {
	if (node) {
		PRINTLEVEL (node->level);
		printf(" - %d : (%s)\n", node->level, node->str);
		_alt_tree_walk (node->right);
		_alt_tree_walk (node->down);
	}
}

AltNode *alt_tree_child(AltNode *node) {
	if (node == NULL)
		return NULL;
	if (node->right != NULL)
		return node->right;
	node = node->down;
	if (node && !*node->str)
		return node->right;
	return node;
}

void alt_tree_walk(AltState *st) {
	AltTree *at = (AltTree*) st->user;
	AltNode *node = at->root;
	printf ("WALK NODES:\n");
	while (node) {
		printf (" = %d : (%s)\n", node->level, node->str);
		_alt_tree_walk (node->right);
		node = node->down;
	}
}

AltNode* alt_tree_resolve(AltState *st, const char *name) {
	AltTree *at = (AltTree*) st->user;
	AltNode *node = at->root;
	while (node) {
		if (!strcmp (node->str, name))
			return node;
		node = node->down;
	}
	return NULL;
}

void alt_tree(AltState *st) {
	AltTree *at = (AltTree*) malloc (sizeof(AltTree));
	at->depth[0] = at->cur = at->root = 0;
	st->user = (void *) at;
	st->cb_word = engine_cb_word;
	st->cb_level = engine_cb_level;
}
