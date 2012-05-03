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
 
#pragma once

#include <time.h>

#include "str/strset.h"
#include "lists/vector.h"

struct pkg {
	
	int		indegree;
	char * 	name;
	char * 	h_file; // header file
	char * 	c_file; // C code
	char * 	o_file; // object code
	char * 	args;   // howto compile pkg

	struct vector * children;
	struct strset * deps;
	
	void		(*add_file)(struct pkg *, const char *);
	void		(*add_child)(struct pkg *, struct pkg *);
	void		(*make_args)(struct pkg *, char **);
	void		(*print)(struct pkg *);
	int			(*up2date)(struct pkg *);
	int			(*compile)(struct pkg *);
	int			(*is_test)(struct pkg *);
	int			(*is_main)(struct pkg *);
	char **		(*get_tests)(struct pkg *);
	time_t		(*modtime)(struct pkg *);
};

struct pkg * new_pkg(char *);
void		 free_pkg(void *);
void *       pkg_compile_par(void *);

