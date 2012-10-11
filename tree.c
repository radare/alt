/* alt - abstract language tree // pancake<at>nopcode.org */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alt.h"

static inline AltNode* node_alloc(AltTree *at) {
	if (at->ncount >= ALLOC_POOL_SIZE) {
		if (++at->npool >= ALLOC_POOL_COUNT) {
			fprintf (stderr, "FAIL: Cannot allocate more memory in the pool\n");
			exit (1);
		}
		at->nodes[at->npool] = malloc (sizeof (AltNode)*ALLOC_POOL_SIZE);
		at->ncount = 0;
	}
	return &at->nodes[at->npool][at->ncount++];
}

static AltNode *alt_node_new(AltTree *at) {
	AltNode *node = node_alloc (at);
	memset (node, 0, sizeof (AltNode));
	return node;
}

static void engine_cb_level(AltState *st, int delta, char ch) {
// THIS CODE MAKES TREE CHOP ){ CONSTRUCTIONS
#if 0
	AltTree *at = (AltTree *) st->user;
	if (delta>0) at->depth[st->level] = at->cur;
	else at->cur = at->depth[st->level];
	if (st->debug) {
		printf ("%d ", st->level);
		PRINTLEVEL (st->level);
		if (delta>0) printf ("{\n");
		else printf ("}\n");
	}
#endif
}

static int _word_type(AltState *st) {
	int type = 'W';
	if (st->mode == MODE_STRING) {
		type = 'S';
	} else if (*st->str>='0' && *st->str<='9') {
		if (strchr (st->str, '.'))
			type = 'F';
		else type = 'I';
	} else if (parse_is_operator (*st->str))
		type = 'O';
	return type;
}

static void engine_cb_word(AltState *st, char ch) {
	AltNode *node;
	AltTree *at = (AltTree *) st->user;
	if (at->laststr && !*st->str && !*at->laststr)
		return;
	node = alt_node_new (st->user);
	node->type = _word_type (st);
	node->str = strdup (st->str);
	node->endch = ch;
	at->laststr = node->str;
	if (at->root == NULL)  /* define root node */
		at->root = at->depth[0] = at->cur = node;
	if (at->lastlevel < st->level) {
		/* set child */
		at->cur->right = node;
		node->left = at->cur;
	} else {
		/* add node at same nest level */
		if (node != at->root) // do not infinite loop when only one node
			at->cur->down = node;
		node->up = at->cur;
	} 
	node->level = st->level;
	at->cur = node;
	if (st->debug) {
		printf ("DEBUG %d %c", st->level, CHF (ch));
		printf (" (%s) ", (at->lastlevel < st->level)? "CHILD":"SAME");
		PRINTLEVEL (st->level);
		printf ("'%s'\n", st->str);
	}
	at->lastlevel = st->level;
}

static void _alt_tree_walk(AltNode *node) {
	if (node) {
		PRINTLEVEL (node->level);
		printf (" - %d %c : (%s) '%c'\n",
			node->level, node->type,
			node->str, CHF (node->endch));
		if (node != node->right)
			_alt_tree_walk (node->right);
		if (node != node->down)
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
		printf (" = %d %c : (%s)\n", node->level, node->type, node->str);
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

void alt_tree_free(AltState *st) {
	int i;
	AltTree *at;
	if (st != NULL && st->user != NULL) {
		at = (AltTree*) st->user;
		for (i=0; i<at->npool; i++)
			free (at->nodes[i]);
		free (at);
		st->user = NULL;
	}
}

void alt_tree(AltState *st) {
	AltTree *at = (AltTree*) malloc (sizeof (AltTree));
	at->depth[0] = at->cur = at->root = 0;
	at->npool = -1;
	at->ncount = ALLOC_POOL_SIZE;
	st->user = (void *) at;
	/* set tree callbacks */
	st->cb_word = engine_cb_word;
	st->cb_level = engine_cb_level;
}
