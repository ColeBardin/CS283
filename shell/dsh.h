#ifndef _DSH_H_
#define _DSH_H_

typedef struct Command Command;
struct Command {
	char *name;
	int (*f)(int argc, char *argv[]);
};

#endif
