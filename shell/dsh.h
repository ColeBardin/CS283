#ifndef _CSH_H_
#define _CSH_H_

typedef struct Command Command;

struct Command {
	char *name;
	int (*f)(char *argv[]);
};

#endif
