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


#ifndef parse_options_flag_h
#define parse_options_flag_h

#include "lists/vector.h"

struct flag {
	
    int             set;
    int             is_bool;
    char **         flags;
    struct vector * options;
    
    int             (*is_set)(struct flag *);
    void            (*make_fancy)(struct flag *);
    void            (*add)(struct flag *, const char *);
    char *          (*get)(struct flag *);
    char **         (*get_all)(struct flag *);
    void			(*reset)(struct flag *);
    void            (*free)(struct flag *);
    
};

struct flag * new_str_flag(const char *);
struct flag * new_bool_flag(const char *);

#endif
