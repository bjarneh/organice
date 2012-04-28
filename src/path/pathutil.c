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

#include "str/stringutil.h"
#include "str/buffer.h"
#include "os/specific.h"
#include "path/pathutil.h"
#include "utilz/handy.h"


char * path_join(const char ** dirs){
	return join_str(dirs, dir_sep());
};

char * path_join_len(const char ** dirs, int len){

	if(len <= 1){
		panic("path_join: array's length <= 1",__FILE__,__LINE__);
	}

	int i;
	char * res;
	struct buffer * b = new_buffer();
	for(i = 0; i < (len - 1); i++){
		b->add(b, dirs[i]);
		b->add(b, dir_sep());
	}
	
	b->add(b, dirs[len-1]);
	res = b->str(b);
	b->free(b);

	return res;
};

char ** path_split(const char * dir){
	return split_str(dir, dir_sep());
};

char * path_clean(const char * dir){

	int i;
	char prev  = ' ';
	char sep   = dir_sep_char();
	char * res;
	struct buffer * b = new_buffer();

	for(i = 0; dir[i] != '\0'; i++){
		if( ! (prev == sep && dir[i] == sep) ){
			b->add_char(b, dir[i]); // b·add_char(dir[i])
		}
		prev = dir[i];
	}

	// do not allow os_sep at the end of file-path
	if(b->_[b->len-1] == sep){ b->len -= 1;	}
	res = b->str(b);
	b->free(b);

	return res;
};

char * path_from_slash(char * dir){

	char sep = dir_sep_char();
	
	int i;
	for(i = 0; dir[i]; i++){
		if(dir[i] == '/'){
			dir[i] = sep;
		}
	}
	
	return dir;
};
