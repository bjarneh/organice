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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "map/hash.h"
#include "utilz/global.h"
#include "utilz/handy.h"

static struct hash * globals = NULL;


static void null_check(void){
	if(globals == NULL){
		panic("globals not initialized", __FILE__, __LINE__);
	}
};

void global_init(void) {
	// in case it's called multiple times
	if(globals == NULL){
		globals = new_hash();
	}
};

char * global_get_str(const char * id) {
	null_check();
	return (char *) globals->get(globals, id);
};

void global_set_str_blank(const char * id){
	globals->put(globals, id, strdup(""));
};

void global_set_str(const char * id, const char * value){
	null_check();
	if(globals->has(globals, id)){
		char * tmp = globals->get(globals, id);
		free(tmp);
	}
	globals->put(globals, id, strdup(value));
};

void global_set_str_nodup(const char * id, char * value){
	null_check();
	if(globals->has(globals, id)){
		char * tmp = globals->get(globals, id);
		free(tmp);
	}
	globals->put(globals, id, value);
};

int	global_get_bool(const char * id) {
	null_check();
	int * ip = (int *) globals->get(globals, id);
	if(ip != NULL){ return *ip;	}
	return 0;
};

void global_set_bool(const char * id, int value){

	int * v;

	null_check();

	if(globals->has(globals, id)){
		v = globals->get(globals, id);
	}else{
		v = malloc(sizeof(int));
	}

	*v = (value)? 1 : 0;
	globals->put(globals, id, v);
};

void global_free(void) {
	null_check();
	globals->free_all(globals, &free);
};
