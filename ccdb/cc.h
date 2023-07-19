typedef struct CComp CComp;

enum {
	Nname = 32,
	Nmaker = 16,
	Ncpu = 8,
	Ndesc = 192,
};

struct CComp {
	char name[Nname];
	char maker[Nmaker];
	char cpu[Ncpu];
	int year;
	int id;
	char desc[Ndesc];
};
