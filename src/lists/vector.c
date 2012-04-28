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


#include <stdlib.h>
#include "lists/vector.h"

// vector methods
static void            vector_twice(struct vector *);
static int             vector_add(struct vector * slf, void * e);
static void *          vector_pop(struct vector * slf);
static int             vector_add_vec(struct vector * slf, struct vector * other);
static void            vector_del(struct vector *);
static void **         vector_to_array(struct vector *);
static void            vector_free_all(struct vector *, void (*fn)(void *));
static void            vector_clear(struct vector *);
static void *          vector_at(struct vector *, int pos);
static struct vector * vector_clone(struct vector * slf);


struct vector * new_vector(void){
    return new_vector_size(50);
};

struct vector * new_vector_size(int size){
    struct vector * v = (struct vector *) calloc(1, sizeof(struct vector));
    v->cap        = size;
    v->len        = 0;
    v->_          = calloc(size, sizeof(void *));
    v->at         = &vector_at;
    v->add        = &vector_add;
    v->pop		  = &vector_pop;
    v->add_vector = &vector_add_vec;
    v->to_array   = &vector_to_array;
    v->free       = &vector_del;
    v->free_all   = &vector_free_all;
    v->clone      = &vector_clone;
    v->clear      = &vector_clear;
    return v;
};

static int vector_add(struct vector * slf, void * e) {

    int increased = 0;

    if( slf->len + 1 >= slf->cap ){
        vector_twice(slf);
        increased = 1;
    }

    slf->_[(slf->len)++] = e;

    return increased;
};

static void * vector_pop(struct vector * slf){
	slf->len -= 1;
	return slf->_[slf->len];
};

static int vector_add_vec(struct vector * slf, struct vector * other){
    int i;
    int increased = 0;
    for(i = 0; i < other->len; i++){
        if(slf->add(slf, other->_[i])){
            increased = 1;
        }
    }
    return increased;
}

static void vector_clear(struct vector * slf){
    slf->len = 0;
};

static struct vector * vector_clone(struct vector * slf){

    int i;
    struct vector * v = new_vector_size(slf->len + 1);

    for(i = 0; i < slf->len; i++){
        v->add(v, slf->_[i]);
    }

    return v;
};

static void * vector_at(struct vector * slf, int pos){
    return slf->_[pos];
};

static void vector_twice(struct vector * slf){

    int i;
    slf->cap = slf->cap * 2;

    void ** v2 = calloc(slf->cap, sizeof(void *));

    for(i = 0; i < slf->len; i++){
        v2[i] = slf->_[i];
    };

    free(slf->_);

    slf->_ = v2;
};

static void ** vector_to_array(struct vector * slf){
    int i;
    void ** elmts = calloc(slf->len + 1, sizeof(void *));
    for(i = 0; i < slf->len; i++){
        elmts[i] = slf->_[i];
    }
    elmts[i] = NULL;
    return elmts;
};

// fn - should free up memory allocated by elements in vector
static void vector_free_all(struct vector * slf, void (*fn)(void *)){
    int i;
    for(i = 0; i < slf->len; i++){
        fn(slf->_[i]);
    }
    slf->free(slf);
};

static void vector_del(struct vector * slf){
    free(slf->_);
    free(slf);
}

