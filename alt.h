#define PRINTLEVEL(x) { int i;for(i=0;i<x;i++) printf("  "); }

#define ALT_VERSION "0.1"

/* tree */
#define TREE_DEPTH 32
#define CHF(x) (x=='\t'?'t':x=='\n'?'n':x=='\r'?'r':(x>'~'||x<' ')?'?':x)

typedef enum {
	KEY_BLOCK, // {
	KEY_LIST,  // (
	KEY_INDEX, // [
} KeyType;

typedef struct AltNode {
	char *str;
	int level;
	int type;
	int endch;
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
	int line;
	int level;
	int levels[ALT_MAX_LEVEL];
	int lastchar;
	char skipuntil;
	char endch;
	char curchar;
	char str[ALT_MAX_LEVEL];
	int stridx;
	void (*cb_comment)(struct AltState *st, char ch);
	void (*cb_word)(struct AltState *st, char ch);
	void (*cb_level)(struct AltState *st, int delta, char ch);
	int (*cb_error)(struct AltState *st, const char *str);
	void *user;
} AltState;

int parse_char(AltState *st, char ch);
int parse_str(AltState *st, char *str);
int parse_fd(AltState *st, int fd);
int parse_file(AltState *st, const char *file);
int parse_is_operator(char ch);

void alt_tree(AltState *st);
void alt_tree_walk(AltState *st);
AltNode* alt_tree_resolve(AltState *st, const char *name);
void alt_tree_walk(AltState *st);
AltNode *alt_tree_child(AltNode *node);
void alt_tree_free(AltState *st);

int alt_script(AltState *st);
