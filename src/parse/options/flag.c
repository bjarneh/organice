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
#include "parse/options/flag.h"
#include "str/stringutil.h"

// flag methods
static void    flag_make_fancy(struct flag *);
static int     flag_is_set(struct flag *);
static char *  flag_get(struct flag *);
static char ** flag_get_all(struct flag *);
static void    flag_add(struct flag *, const char *);
static void    flag_free(struct flag *);
static void    flag_reset(struct flag *);

// initialize flag of both types with this function
static struct flag * new_flag(const char *);



struct flag * new_str_flag(const char * optstr){
    struct flag * f = new_flag(optstr);
    f->is_bool = 0;
    return f;
};

struct flag * new_bool_flag(const char * optstr){
    struct flag * f = new_flag(optstr);
    f->is_bool = 1;
    return f;
};


static struct flag * new_flag(const char * optstr){

    struct flag * f = calloc(1, sizeof(struct flag));

    f->flags      = split_str(optstr, " ");
    f->set        = 0;
    f->options    = new_vector_size(10);
    f->is_set     = &flag_is_set;
    f->make_fancy = &flag_make_fancy;
    f->add        = &flag_add;
    f->get        = &flag_get;
    f->get_all    = &flag_get_all;
    f->reset	  = &flag_reset;
    f->free       = &flag_free;

    return f;
};

static void flag_add(struct flag * slf, const char * arg){
    slf->options->add(slf->options, strdup(arg));
};

static int flag_is_set(struct flag * slf){

    if( slf->is_bool ){
        return slf->set;
    }

    return slf->options->len > 0;
};

static void flag_reset(struct flag * slf){
	
	if( slf->is_bool){
		slf->set = 0;
	}else{
		int i;
		for(i = 0; i < slf->options->len; i++){
			free( slf->options->_[i] );
		};
		slf->options->clear(slf->options);
	}
};

// return allocated copy to be able to free copt/flags
static char * flag_get(struct flag * slf){
    char * opt = (char *) slf->options->_[0];
    return strdup(opt);
};

// return allocated copy to be able to free copt/flags
static char ** flag_get_all(struct flag * slf){

    int i;
    char ** opts = (char **) calloc(slf->options->len + 1, sizeof(char *));

    for(i = 0; i < slf->options->len; i++){
        opts[i] = strdup(slf->options->_[i]);
    }
    opts[i] = NULL;

    return opts;
};

static void flag_free(struct flag * slf){
    int i;
    for(i = 0; slf->flags[i]; i++){
        free(slf->flags[i]);
    }
    free(slf->flags);
    slf->options->free_all(slf->options, &free);
    free(slf);
};

static void flag_make_fancy(struct flag * slf){
    
    int i, len_flag;
    struct vector * vec = new_vector();

    for(i = 0; slf->flags[i] != NULL; i++){

        len_flag = strlen(slf->flags[i]);

        if(len_flag == 2 && slf->flags[i][0] == '-'){
            vec->add(vec, add_str(slf->flags[i], "="));
        }else if(len_flag > 3 && starts_with(slf->flags[i], "--")){
            vec->add(vec, strdup(&(slf->flags[i][1])));
            vec->add(vec, add_str(slf->flags[i], "="));
            vec->add(vec, add_str(&(slf->flags[i][1]), "="));
        }
    }

    if(vec->len > 0){

        for(i = 0; slf->flags[i] != NULL; i++){
            vec->add(vec, strdup(slf->flags[i]));
        }

        char ** all = calloc(vec->len + 1, sizeof(char *));

        for(i = 0; i < vec->len; i++){
            all[i] = vec->_[i];
        }
        all[i] = NULL;

        for(i = 0; slf->flags[i] != NULL; i++){
            free(slf->flags[i]);
        }
        free(slf->flags);

        slf->flags = all;

    }
    
    vec->free(vec);
};

