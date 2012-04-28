// -------------------------------------------------------------------------
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
// -------------------------------------------------------------------------
 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utilz/handy.h"
#include "lists/vector.h"
#include "os/specific.h"
#include "str/stringutil.h"
#include "parse/source/code.h"

// private functions
static char * import_from_line(char *);

/*
 if ( root == "src" || root == "src/" ) and
 	( name == "src/path/to/file.c" || name == "src/path/to/file.h")
 then
 		"path/to/file"
*/

char * pkg_name_from_fname(const char * root, const char * name){

	int len_root, start;

	if(root != NULL){
		len_root = strlen(root);
		start = ( ends_with(root, dir_sep()) )? len_root : len_root + 1;
	}else{
		start = 0;
	}

	return slice_str(start, (strlen(name) - 2), name);
};

char ** get_deps_from_fname(const char * fname){
	
	char * dep;
	char ** dependencies = NULL;
	struct vector * v = new_vector();
	char ** lines = slurp_lines(fname);

	int i;
	for(i = 0; lines[i]; i++){
		dep = import_from_line(lines[i]);
		if(dep){
			v->add(v, dep);
		}
	}
	
	if(v->len > 0){
		dependencies = calloc(v->len + 1, sizeof(char *));
		for(i = 0; i < v->len; i++){
			dependencies[i] = (char *) v->_[i];
		}
		dependencies[i] = NULL;
	}
	
	free_strings(lines);
	v->free(v);

	return dependencies;
};

//_____________________________________________util

static char * import_from_line(char * line){

	char * trimmed;
	char * file_name;
	char * pkg_name;
	
	if(starts_with(line, "#include ")){
		trimmed = trim_str(&line[8]);
		if(starts_with(trimmed, "\"") && ends_with(trimmed, "\"")){
			file_name = trim_any_str(trimmed, "\"");
			pkg_name  = pkg_name_from_fname(NULL, file_name);
			free(file_name);
			free(trimmed);
			return pkg_name;
		}
		free(trimmed);
	}
	return NULL;
};
