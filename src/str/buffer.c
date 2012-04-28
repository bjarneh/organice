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


#include <stdlib.h>
#include <string.h>
#include "str/buffer.h"


// buffer methods
static void     buffer_clear(struct buffer *);
static void     buffer_add(struct buffer *, const char *);
static void     buffer_add_len(struct buffer *, int, const char *);
static void     buffer_add_char(struct buffer *, char);
static void     buffer_twice(struct buffer *);
static char *   buffer_str(struct buffer *);
static void     buffer_free(struct buffer *);


struct buffer * new_buffer(void){
    return new_buffer_size(100);
};

struct buffer * new_buffer_size(int size){
    struct buffer * b = (struct buffer *) calloc(1, sizeof(struct buffer));
    b->len      = 0;
    b->cap      = size;
    b->_        = (char *) calloc(size, sizeof(char));
    b->add      = &buffer_add;
    b->add_len  = &buffer_add_len;
    b->add_char = &buffer_add_char;
    b->clear    = &buffer_clear;
    b->str      = &buffer_str;
    b->free     = &buffer_free;
    return b;
};

static void buffer_clear(struct buffer * slf){
    slf->len = 0;
};

static void buffer_add(struct buffer * slf, const char * s){

    int i;
    int slen = strlen(s);

    if( (slen + slf->len) >= slf->cap ){
        buffer_twice(slf);
    }

    for(i = 0; i < slen; i++){
        slf->_[i + slf->len] = s[i];
    }

    slf->len += i;
};

static void buffer_add_char(struct buffer * slf, char c){
	
    if( 1 + slf->len >= slf->cap ){
        buffer_twice(slf);
    }
    
    slf->_[slf->len] = c;
    slf->len += 1;

};

static void buffer_add_len(struct buffer * slf, int n, const char * s){

    int i;

    if( n + slf->len >= slf->cap ){
        buffer_twice(slf);
    }

    for(i = 0; i < n; i++){
        slf->_[i + slf->len] = s[i];
    }

    slf->len += n;
};

static void buffer_twice(struct buffer * slf){

    int i;

    char * cp = (char *) calloc(slf->cap * 2, sizeof(char));

    for(i = 0; i < slf->cap; i++){
        cp[i] = slf->_[i];
    }
    
    slf->cap = slf->cap * 2;

    free(slf->_);

    slf->_ = cp;
};

static char * buffer_str(struct buffer * slf){

    int i;

    char * ret = (char *) calloc(slf->len + 1, sizeof(char));

    for(i = 0; i < slf->len; i++){
        ret[i] = slf->_[i];
    }

    ret[slf->len] = '\0';

    return ret;
};


static void buffer_free(struct buffer * slf){
    free(slf->_);
    free(slf);
};
