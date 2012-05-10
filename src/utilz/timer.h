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

#include "map/hash.h"

struct timer {
    
    struct hash * h;
    
    void    (*start)(struct timer *, const char *);
    void    (*stop)(struct timer *, const char *);
    char *  (*str)(struct timer *, const char *);
    long    (*millis)(struct timer *, const char *);
    double  (*seconds)(struct timer *, const char *);
    void    (*free)(struct timer*);

};

struct timer * new_timer(void);

