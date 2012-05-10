// -----------------------------------------------------------------------
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
// -----------------------------------------------------------------------

#pragma once

#include "map/hash.h"
#include "lists/vector.h"

struct copt {
    
    struct vector * options;
    struct hash * cache;
        
    int     (*is_set)(struct copt *, const char *);
    void    (*bool_option)(struct copt *, const char *);
    void    (*str_option)(struct copt *, const char *);
    void    (*str_option_fancy)(struct copt *, const char *);
//  void    (*out)(struct copt *, FILE *);
    char *  (*get)(struct copt *, const char *);
    char ** (*get_all)(struct copt *, const char *);
    char ** (*parse_arr)(struct copt *, char **);
    char ** (*parse)(struct copt *, int, char **);
    char ** (*parse_argv)(struct copt *, int, char **);
    void    (*reset)(struct copt *);
    void    (*free)(struct copt *);
    
};

struct copt * new_copt(void);
