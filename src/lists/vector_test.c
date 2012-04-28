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


#include <stdio.h>
#include <string.h>
#include "utilz/handy.h" /* assert_log */
#include "lists/vector.h"
#include "lists/vector_test.h"

int test_vector_basics(int verbose){

    int i;
    int ok = 1;

    char *words[] = {"one", "two", "three", "four", NULL};

    struct vector * v = new_vector_size(4);

    v->add(v, words[0]);
    v->add(v, words[1]);
    v->add(v, words[2]);

    assert_log(v->cap == 4, verbose, "vec->cap != 4", &ok);
    assert_log(v->len == 3, verbose, "vec->len != 3", &ok);

    v->add(v, words[3]);

    for(i = 0; words[i] != NULL; i++){
        if( strcmp((char*) v->_[i], words[i]) ){
            if(verbose){
                fprintf(stderr,"vec->[%d] != words[%d]\n", i , i);
                fprintf(stderr,"%s != %s\n", (char*) v->_[i] , words[i]);
            }
            ok = 0;
        }
    };


    assert_log(v->cap == 8, verbose, "vec->cap != 8", &ok);

    v->free(v);

    return ok;
};


int test_add_vec(int verbose){

    int i;
    int ok = 1;

    char *words[] = {"one", "two", "three", "four", NULL};

    struct vector * v  = new_vector();
    struct vector * v2 = new_vector_size(2);

    for(i = 0; words[i] != NULL; i++){
        v->add(v, words[i]);
        v2->add(v2, words[i]);
    }
    
    v->add_vector(v, v2);

    assert_log(v->len == 8, verbose, "vec->len != 8", &ok);
    assert_log(v->cap == 50, verbose, "vec->cap != 50", &ok);

    assert_log( ! strcmp(v->_[0], v->_[4]), verbose,"v->_[0] != v->_[4]", &ok);

    return ok;
};

int test_clone(int verbose){

    int i;
    int ok = 1;
    
    char *words[] = {"one", "two", "three", "four", NULL};

    struct vector * v  = new_vector();

    for(i = 0; words[i] != NULL; i++){
        v->add(v, words[i]);
    }

    struct vector * copy = v->clone(v);

    for(i = 0; i < v->len; i++){
        assert_log(copy->_[i] == v->_[i], verbose, "copy != original", &ok);
    }

    return ok;
};

int test_to_array(int verbose){

    int ok = 1;
    struct vector * v  = new_vector();

    v->add(v, "one");
    v->add(v, "two");
    v->add(v, "3");

    void ** arr = v->to_array(v);

    assert_log( !strcmp("one", (char *) arr[0]), verbose, "one != one", &ok);
    assert_log( !strcmp("two", (char *) arr[1]), verbose, "two != two", &ok);
    assert_log( !strcmp("3", (char *) arr[2]), verbose, "3 != 3", &ok);
    assert_log( arr[3] == NULL , verbose, "arr[3] != NULL", &ok);

    return ok;
};
