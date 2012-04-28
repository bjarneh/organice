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
#include <stdio.h>
#include "parse/options/flag.h"
#include "parse/options/copt.h"
#include "str/stringutil.h"
#include "lists/vector.h"

// copt methods
static int     copt_is_set(struct copt *, const char *);
static void    copt_bool_option(struct copt *, const char *);
static void    copt_str_option(struct copt *, const char *);
static void    copt_str_option_fancy(struct copt *, const char *);
static char *  copt_get(struct copt *, const char *);
static char ** copt_get_all(struct copt *, const char *);
static char ** copt_parse(struct copt *, int, char **);
static char ** copt_parse_arr(struct copt *, char **);
static char ** copt_parse_argv(struct copt *, int, char **);
static void    copt_reset(struct copt *);
static void    copt_free(struct copt *);
// private functions
static void    copt_register_option(struct copt *, struct flag *);
static int     juxta_bool(struct copt *, char *);
static char *  juxta_str(struct copt *, char *);


struct copt * new_copt(void){
    struct copt * c = calloc(1, sizeof(struct copt));
    c->options          = new_vector();
    c->cache            = new_hash();
    c->is_set           = &copt_is_set;
    c->bool_option      = &copt_bool_option;
    c->str_option       = &copt_str_option;
    c->str_option_fancy = &copt_str_option_fancy;
    c->get              = &copt_get;
    c->get_all          = &copt_get_all;
    c->parse            = &copt_parse;
    c->parse_arr		= &copt_parse_arr;
    c->parse_argv       = &copt_parse_argv;
    c->reset			= &copt_reset;
    c->free             = &copt_free;
    return c;
};

static int copt_is_set(struct copt * slf, const char * opt){

    struct flag * f;

    if(slf->cache->has(slf->cache, opt)){
        f = slf->cache->get(slf->cache, opt);
        return f->is_set(f);
    }

    return 0;
};


static char * copt_get(struct copt * slf, const char * opt){

    struct flag * f;

    if(slf->cache->has(slf->cache, opt)){
        f = slf->cache->get(slf->cache, opt);
        return f->get(f);
    }

    return NULL;
};

static char ** copt_get_all(struct copt * slf, const char * opt){

    struct flag * f;

    if(slf->cache->has(slf->cache, opt)){
        f = slf->cache->get(slf->cache, opt);
        return f->get_all(f);
    }

    return NULL;
};

static void copt_bool_option(struct copt * slf, const char * optstr){

    struct flag * f = new_bool_flag(optstr);
    copt_register_option(slf, f);

};

static void copt_str_option(struct copt * slf, const char * optstr){

    struct flag * f = new_str_flag(optstr);
    copt_register_option(slf, f);

};

static void copt_str_option_fancy(struct copt * slf, const char * optstr){

    struct flag * f = new_str_flag(optstr);
    f->make_fancy(f);
    copt_register_option(slf, f);

};

static void copt_register_option(struct copt * slf, struct flag * f){

    int i;

    for(i = 0; f->flags[i] != NULL; i++){
        slf->cache->put(slf->cache, f->flags[i], f);
    }

    slf->options->add(slf->options, f);

};

static char ** copt_parse_arr(struct copt * slf, char ** argv){
	int i;
	for(i = 0; argv[i]; i++);
	return slf->parse(slf, i, argv);
};

static char ** copt_parse_argv(struct copt * slf, int n, char ** argv){
    return copt_parse(slf, --n, ++(argv) );
};

static char ** copt_parse(struct copt * slf, int n, char ** argv){

    int i;
    int bool_juxta;
    char * str_juxta;
    struct flag * f;
    struct hash * look_up;
    struct vector * vec = new_vector();

    look_up = slf->cache;

    for(i = 0; i < n; i++){
        if(look_up->has(look_up, argv[i])){
            f = (struct flag *) look_up->get(look_up, argv[i]);
            if(f->is_bool){
                f->set = 1;
            }else{
                if(i + 1 >= n){
                    printf("[ERROR] missing argument for: %s\n", argv[i]);
                    exit(1);
                }else{
                    f->add(f, argv[++i]);
                }
            }
        }else{

            bool_juxta = 0;
            str_juxta  = NULL;

            bool_juxta = juxta_bool(slf, argv[i]);

            if( ! bool_juxta ){

                str_juxta = juxta_str(slf, argv[i]);

                if(str_juxta){
                    f = (struct flag *) slf->cache->get(slf->cache, str_juxta);
                    f->add(f, (argv[i]) += strlen(str_juxta));
                }
            }

            if( (! bool_juxta) && (str_juxta == NULL) ){
                vec->add(vec, strdup( argv[i] ));
            }
        }
    }

    char ** rest = calloc(vec->len + 1, sizeof(char *));
    for(i = 0; i < vec->len; i++){
        rest[i] = (char *) vec->_[i];
    }
    rest[i] = NULL;

    vec->free(vec);

    return rest;
};

// -abc == -a -b -c  iff -a -b -c are options
static int juxta_bool(struct copt * slf, char * opt){

    int i, opt_len;
    struct flag * f;
    char possible[3];
    possible[0] = '-';
    possible[2] = '\0';

    if( ! starts_with(opt, "-")){
        return 0;
    }

    opt_len = strlen(opt);
    for(i = 1; i < opt_len; i++){
        possible[1] = opt[i];
        if( ! slf->cache->has(slf->cache, possible) ){
            return 0;
        }else{
            f = (struct flag *) slf->cache->get(slf->cache, possible);
            if( ! f->is_bool ){
                return 0;
            }
        }
    }

    // options were bool so we set them all
    for(i = 1; i < opt_len; i++){
        possible[1] = opt[i];
        f = (struct flag *) slf->cache->get(slf->cache, possible);
        f->set = 1;
    }

    return 1;
};

static char * juxta_str(struct copt * slf, char * opt){

    int i;
    int key_len;
    int max_len = 0;
    char * max_flag = NULL;
    struct flag * f;
    char ** keyz = slf->cache->keys(slf->cache);

    for(i = 0; keyz[i]; i++){
        if(starts_with(opt, keyz[i])) {
            f = (struct flag *) slf->cache->get(slf->cache, keyz[i]);
            if( ! f->is_bool ){
                key_len = strlen(keyz[i]);
                if(key_len > max_len){
                    max_len  = key_len;
                    max_flag = keyz[i];
                }
            }
        }
    }

    free(keyz);

    return max_flag;
};


static void copt_reset(struct copt * slf){
	
    int i;
    struct flag * f;
    char ** keyz = slf->cache->keys(slf->cache);
    
	for(i = 0; keyz[i]; i++){
		f = (struct flag *) slf->cache->get(slf->cache, keyz[i]);
		f->reset(f);
	}
	
	free(keyz);
};

static void copt_free(struct copt * slf){
    int i;
    struct flag * f;
    for(i = 0; i < slf->options->len; i++){
        f = (struct flag *) slf->options->_[i];
        f->free(f);
    }
    slf->options->free(slf->options);
    slf->cache->free(slf->cache);
    free(slf);
};
