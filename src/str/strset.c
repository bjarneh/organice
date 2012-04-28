// ------------------------------------------------------------------------
//  Copyright © 2012 bjarneh
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ------------------------------------------------------------------------


#include <stdlib.h>
#include "str/strset.h"
#include "map/hash.h"


// strset methods
static void 	strset_add(struct strset *, const char *);
static int		strset_has(struct strset *, const char *);
static void		strset_remove(struct strset *, const char *);
static char **	strset_to_array(struct strset *);
static void 	strset_clear(struct strset *);
static void 	strset_free(struct strset *);

// constructor
struct strset * new_strset(void){
	struct strset * s = malloc(sizeof(struct strset));
	s->map		= new_hash();
	s->add		= &strset_add;
	s->has		= &strset_has;
	s->remove	= &strset_remove;
	s->to_array	= &strset_to_array;
	s->clear	= &strset_clear;
	s->free		= &strset_free;
	return s;
};


static void strset_add(struct strset * slf, const char * e){
	slf->map->put(slf->map, e, &(slf->dummy));
};

static int strset_has(struct strset * slf, const char * e){
	return slf->map->has(slf->map, e);
};

static void strset_remove(struct strset * slf, const char * e){
	slf->map->put(slf->map, e, NULL);
};

static char ** strset_to_array(struct strset * slf){
	return slf->map->keys(slf->map);
};

static void strset_clear(struct strset * slf){
	slf->map->free(slf->map);
	slf->map = new_hash();
};

static void strset_free(struct strset * slf){
	slf->map->free(slf->map);
	free(slf);
};


