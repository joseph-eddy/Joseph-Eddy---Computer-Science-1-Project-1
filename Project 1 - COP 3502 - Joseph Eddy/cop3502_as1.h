typedef struct monster {
	int id;
	char *name;
	char *element;
	int population;
} monster;

typedef struct region {
	char *name;
	int nmonsters;
	int total_population;
	monster **monsters;
} region;

typedef struct itinerary {
	int nregions;
	region **regions;
	int captures;
} itinerary;

typedef struct trainer {
	char *name;
	itinerary *visits;
} trainer;
