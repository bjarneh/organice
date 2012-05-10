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

#include "map/hash.h"
#include "compiler/pkg.h"

struct dag {

    struct hash * pkgs;
    struct pkg ** sorted;
    
    void   (*parse)(struct dag *);
    void   (*build)(struct dag *, char **);
    void   (*topsort)(struct dag *);
    void   (*compile)(struct dag *);
    void   (*pcompile)(struct dag *);
    void   (*link)(struct dag *, char *, char *, char **, char **);
    void   (*clean)(struct dag *);
    void   (*dryrun)(struct dag *);
    void   (*sort)(struct dag *);
    void   (*fancy)(struct dag *);
    void   (*print)(struct dag *);
    char * (*add_test)(struct dag *, char **);
    void   (*unlink_test)(struct dag *, char *);
    void   (*free)(struct dag *);
    
};

struct dag * new_dag();

