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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "map/hash.h"
#include "lists/vector.h"
#include "str/buffer.h"

// methods accessible through 'hash'
static void          hash_print(struct hash *, char * (* val2str)(void *));
static char *        hash_str(struct hash *, char * (* val2str)(void *));
static int           hash_put(struct hash *, const char *, void *);
static void *        hash_get(struct hash *, const char *);
static int           hash_has(struct hash *, const char *);
static char **       hash_keys(struct hash *);
static void          hash_free(struct hash *);
static void          hash_free_all(struct hash *, void (*fn)(void *));
static unsigned long hash_hash_code(const char *);


struct hash * new_hash(void) {
    return new_hash_size(50);
};

struct hash * new_hash_size(int size) {
    int i;
    struct hash * h = (struct hash *) calloc(1, sizeof(struct hash));
    h->size       = size;
    h->factor     = 0.0;
    h->hash_code  = &hash_hash_code;
    h->put        = &hash_put;
    h->get        = &hash_get;
    h->has        = &hash_has;
    h->keys       = &hash_keys;
    h->free       = &hash_free;
    h->free_all   = &hash_free_all;
    h->print      = &hash_print;
    h->str        = &hash_str;
    h->content    = calloc(size, sizeof(struct vector *));
    for(i = 0; i < size; i++){ h->content[i] = new_vector(); }
    return h;
};


static int hash_put(struct hash * slf, const char * key, void * value){

    long hash_value  = slf->hash_code(key);
    unsigned int pos = (unsigned int) hash_value % slf->size;
    int  i;

    struct key_value * kv;
    struct vector * vec;

    vec = slf->content[pos];

    for(i = 0; i < vec->len; i++){
        kv = (struct key_value *) vec->_[i];
        if(! strcmp(kv->key, key) ){
            kv->value = value;
            return 1;
        }
    };

    kv = (struct key_value *) calloc(1, sizeof(struct key_value));
    kv->key   = strdup(key);
    kv->value = value;

    vec->add(vec, kv);

    return 0;
};

static void * hash_get(struct hash * slf, const char * key){

    long hash_value  = slf->hash_code(key);
    unsigned int pos = (unsigned int) hash_value % slf->size;
    int  i;

    struct key_value * kv;
    struct vector * vec;

    vec = slf->content[pos];

    for(i = 0; i < vec->len; i++){
        kv = (struct key_value *) vec->_[i];
        if(! strcmp(kv->key, key) ){
            return kv->value;
        }
    };

    return NULL;
};

static int hash_has(struct hash * slf, const char * key){

    long hash_value  = slf->hash_code(key);
    unsigned int pos = (unsigned int) hash_value % slf->size;
    int  i;
    struct vector * vec;
    struct key_value * kv;

    vec = slf->content[pos];

    for(i = 0; i < vec->len; i++){
        kv = (struct key_value *) vec->_[i];
        if(! strcmp(kv->key, key) ){
            return 1;
        }
    };

    return 0;
};

static char ** hash_keys(struct hash * slf){

    int i,j;
    char ** keys;
    struct vector * tmp;
    struct key_value * kv;
    struct vector * k = new_vector();

    for(i = 0; i < slf->size; i++){
        tmp = slf->content[i];
        for(j = 0; j < tmp->len; j++){
            kv = (struct key_value *) tmp->_[j];
            k->add(k, kv->key);
        }
    }

    keys = calloc(k->len + 1, sizeof(char *));
    for(i = 0; i < k->len; i++){
        keys[i] = (char *) k->_[i];
    }
    keys[i] = NULL;

    k->free(k);

    return keys;
};

// java.lang.String.hashCode
static unsigned long hash_hash_code(const char *key){

    int i, len;
    unsigned long hash_value = 1;

    if( key == NULL ){
        fprintf(stderr,"map/hash keys cannot be NULL\n");
        exit(1);
    } // bad exception handling here :-)

    len = strlen(key);

    for(i = 0; i < len; i++){
        hash_value = hash_value * 37 * (unsigned int) key[i];
    }

    return hash_value;
};

static void hash_free(struct hash * slf){

    int i, j;
    struct key_value * kv;
    struct vector * vec;

    for(i = 0; i < slf->size; i++){
        vec = (struct vector *) slf->content[i];
        for(j = 0; j < vec->len; j++){
            kv = (struct key_value *) vec->at(vec, j);
            free(kv->key);
            free(kv);
        }
        vec->free(vec);
    };

    free(slf->content);
    free(slf);
};

static void hash_free_all(struct hash * slf, void (*fn)(void *)){

    int i, j;
    struct key_value * kv;
    struct vector * vec;

    for(i = 0; i < slf->size; i++){
        vec = (struct vector *) slf->content[i];
        for(j = 0; j < vec->len; j++){
            kv = (struct key_value *) vec->at(vec, j);
            free(kv->key);
            fn(kv->value);
            free(kv);
        }
        vec->free(vec);
    };

    free(slf->content);
    free(slf);

};

static void hash_print(struct hash * slf, char * (* val2str)(void *)){
    
    char * strval = slf->str(slf, val2str);
    
    printf("%s\n", strval);
    free(strval);
    
};

static char * hash_str(struct hash * slf, char * (* val2str)(void *)){

    int i             = 0;
    char ** k         = slf->keys(slf);
    char * retval;
    struct buffer * b = new_buffer();

    b->add(b, "hash{\n");
    for(i = 0; k[i] != NULL; i++){
        b->add(b,"  ");
        b->add(b, k[i]);
        b->add(b, " => ");
        b->add(b, val2str(slf->get(slf, k[i])));
        b->add(b, "\n");
    };
    b->add(b, "}\n");
    
    retval = b->str(b);
    b->free(b);
    
    return retval;
    
};

