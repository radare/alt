#define PRINTLEVEL(x) { int i;for(i=0;i<x;i++) printf("  "); }

#define DEBUG 0

/* tree */
#define TREE_DEPTH 32

typedef enum {
	TYPE_WORD,     // foo
	TYPE_STRING,   // "foo"
	TYPE_INTEGER,  // 123
	TYPE_FLOAT,    // 493.22
	TYPE_OPERATOR, // operator
} WordType;

typedef enum {
	KEY_BLOCK, // {
	KEY_LIST,  // (
	KEY_INDEX, // [
} KeyType;

typedef struct AltNode {
	char *str;
	int level;
	int type;
	struct AltNode *up;
	struct AltNode *down;
	struct AltNode *right;
	struct AltNode *left;
} AltNode;

#define ALLOC_POOL_SIZE 1024
#define ALLOC_POOL_COUNT 128

typedef struct AltTree {
	AltNode *root;
	AltNode *cur;
	AltNode *depth[TREE_DEPTH];
	int lastlevel;
	char *laststr;
	AltNode *nodes[ALLOC_POOL_COUNT];
	int ncount;
	int npool;
} AltTree;

/* parser */
#define ALT_MAX_LEVEL 256

typedef enum {
	MODE_PARSE,
	MODE_OPERATOR,
	MODE_COMMENT,
	MODE_STRING,
} AltMode;

typedef struct AltState {
	AltMode mode;
	int debug;
	//int word;
	int line;
	int level;
	int levels[ALT_MAX_LEVEL];
	int lastchar;
	char skipuntil;
	char endch;
	char curchar;
	char str[ALT_MAX_LEVEL];
	int stridx;
	void (*cb_word)(struct AltState *st);
	void (*cb_level)(struct AltState *st, int delta, char ch);
	int (*cb_error)(struct AltState *st, const char *fmt, ...);
	void *user;
} AltState;

int parse_char(AltState *st, char ch);
int parse_str(AltState *st, char *str);
int parse_fd(AltState *st, int fd);
int parse_file(AltState *st, const char *file);
int parse_is_operator(char ch);

void alt_tree(AltState *st, int debug);
void alt_tree_walk(AltState *st);
AltNode* alt_tree_resolve(AltState *st, const char *name);
void alt_tree_walk(AltState *st);
AltNode *alt_tree_child(AltNode *node);
void alt_tree_free(AltState *st);

int alt_script(AltState *st);
