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
#include <string.h>

#include "str/strvec.h"
#include "str/buffer.h"
#include "lists/vector.h"


// strvec methods
static void     strvec_add(struct strvec *, char *);
static void     strvec_add_array(struct strvec *, char **);
static char *   strvec_join(struct strvec *, const char *);
static char *   strvec_at(struct strvec *, int);
static char *   strvec_str(struct strvec *);
static char **  strvec_to_array(struct strvec *);
static int      strvec_len(struct strvec *);
static void     strvec_free(struct strvec *);
static void     strvec_free_all(struct strvec *);


struct strvec * new_strvec(void){

    struct strvec * sv = malloc(sizeof(struct strvec));

    sv->v          = new_vector();
    sv->add        = &strvec_add;
    sv->add_array  = &strvec_add_array;
    sv->join       = &strvec_join;
    sv->str        = &strvec_str;
    sv->at         = &strvec_at;
    sv->to_array   = &strvec_to_array;
    sv->len        = &strvec_len;
    sv->free       = &strvec_free;
    sv->free_all   = &strvec_free_all;
    
    return sv;
};


static void strvec_add(struct strvec * slf, char * s){
    slf->v->add(slf->v, s);
};

static void strvec_add_array(struct strvec * slf, char ** a){
    int i;
    for(i = 0; a[i]; i++){
        slf->add(slf, a[i]);
    }
};

static int strvec_len(struct strvec * slf){
    return slf->v->len;
};

static char * strvec_at(struct strvec * slf, int index){
    if(index >= slf->v->len){
        return NULL;
    }
    return (char *) slf->v->_[index];
};

static char * strvec_join(struct strvec * slf, const char * d){
    
    if(slf->v->len == 0){
        return NULL;
    }

    int i, max;
    struct buffer * b = new_buffer();
    char * result;
    
    max = slf->v->len - 1;
    
    for(i = 0; i < max; i++){
        b->add(b, (char *) slf->v->_[i]);
        b->add(b, d);
    }

    b->add(b, slf->v->_[i]);
    
    result = b->str(b);
    
    b->free(b);
    
    return result;
    
};


static char * strvec_str(struct strvec * slf){
    
    if(slf->v->len == 0){
        return NULL;
    }

    int i;
    struct buffer * b = new_buffer();
    char * result;
    
    for(i = 0; i < slf->v->len; i++){
        b->add(b, slf->v->_[i]);
    }
    
    result = b->str(b);
    
    b->free(b);
    
    return result;
};

static char ** strvec_to_array(struct strvec * slf){

    int i;
    
    char ** res = calloc(slf->v->len + 1, sizeof(char *));
    
    for( i = 0; i < slf->v->len; i++ ){
        res[i] = strdup( (char *) slf->v->_[i] );
    };
    res[slf->v->len] = NULL;
    
    return res;
};

static void strvec_free(struct strvec * slf){
    slf->v->free(slf->v);
    free(slf);
};

static void strvec_free_all(struct strvec * slf){
    slf->v->free_all(slf->v, &free);
    free(slf);
};
