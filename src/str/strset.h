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


#ifndef str_strset_h
#define str_strset_h

#include "map/hash.h"

struct strset {
	
	int dummy;
	struct hash * map;
	
	void	(*add)(struct strset *, const char *);
	void	(*remove)(struct strset *, const char *);
	int		(*has)(struct strset *, const char *);
	char ** (*to_array)(struct strset *);
	void	(*clear)(struct strset *);
	void	(*free)(struct strset *);
	
};

struct strset * new_strset(void);

#endif
