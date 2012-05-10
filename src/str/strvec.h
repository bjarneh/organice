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

#pragma once

#include "lists/vector.h"


struct strvec {
    
    struct vector * v;
    
    int     (*len)(struct strvec *);
    void    (*add)(struct strvec *, char *);
    void    (*add_array)(struct strvec *, char **);
    char *  (*join)(struct strvec *, const char *);
    char *  (*str)(struct strvec *);
    char *  (*at)(struct strvec *, int);
    char ** (*to_array)(struct strvec *);
    void    (*free)(struct strvec *);
    void    (*free_all)(struct strvec *);
    
};

struct strvec * new_strvec(void);

