#include <stdio.h>
#ifdef _MSC_VER
#define strcasecmp    _stricmp
#define strncasecmp   _strnicmp
#else
#include <strings.h>
#endif
#include <string.h>
#include <stdlib.h>

#include "flp.h"
#include "shape.h"
#include "util.h"

int desc_get_blk_index(flp_desc_t *flp_desc, char *name);

int desc_count_units(FILE *fp);

flp_desc_t *desc_alloc_init_mem(int count, flp_config_t *config);

void desc_populate_blks(flp_desc_t *flp_desc, FILE *fp);

void desc_populate_connects(flp_desc_t *flp_desc, FILE *fp);

flp_desc_t *read_flp_desc(char *file, flp_config_t *config);

void free_flp_desc(flp_desc_t *flp_desc);

void print_unplaced(unplaced_t *unit);

void desc_print_wire_density(flp_desc_t *flp_desc);

void print_flp_desc(flp_desc_t *flp_desc);


