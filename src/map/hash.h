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


#ifndef map_hash_h
#define map_hash_h

#include "lists/vector.h"

struct hash {
	
    int              size;
    double           factor;
    struct vector ** content;
    
    unsigned long    (*hash_code) (const char*);
    int              (*put) (struct hash *, const char *, void *);
    void *           (*get) (struct hash *, const char *);
    int              (*has) (struct hash *, const char *);
    char **          (*keys) (struct hash *);
    void             (*print) (struct hash *, char * (*val2str)(void *));
    char *           (*str) (struct hash *, char * (*val2str)(void *));
    void             (*free) (struct hash *);
    void             (*free_all) (struct hash *, void (*fn)(void *));

};

struct key_value {
    char * key;
    void * value;
};

struct hash * new_hash(void);
struct hash * new_hash_size(int size);

/*
*
* Example:
    
    struct hash h = new_hash();

    h->put(h, "hello", "world");
    h->put(h, "foo", "bar");

    if(h->has(h, "hello")) {
        printf("hello => %s\n", (char *) h->get(h, "hello"));
    }else{
        printf("this should not happen\n");
    }

*
*
*/

#endif

