#define PRINTLEVEL { int i;for(i=0;i<st->level;i++) printf("  "); }

typedef enum {
	MODE_PARSE,
	MODE_COMMENT,
	MODE_STRING,
} AltMode;

typedef struct AltState {
	AltMode mode;
	int word;
	int line;
	int level;
	int lastchar;
	int skipuntil;
	char str[128];
	int stridx;
	int (*cb_word)(struct AltState *st);
	int (*cb_error)(struct AltState *st, const char *fmt, ...);
	void *user;
} AltState;


int parse_char(AltState *st, char ch);
int parse_str(AltState *st, char *str);
int parse_fd(AltState *st, int fd);
int parse_file(AltState *st, const char *file);

void engine_tree(AltState *st);
