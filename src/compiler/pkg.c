//--------------------------------------------------------------------------
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
//--------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <semaphore.h>

#include "os/specific.h"
#include "compiler/pkg.h"
#include "str/strset.h"
#include "str/strvec.h"
#include "str/stringutil.h"
#include "utilz/handy.h"
#include "utilz/global.h"
#include "lists/vector.h"
#include "parse/source/code.h"

// semaphore used when -jobs option is given
sem_t job_semaphore;

// pkg mehtods
static void     pkg_add_file(struct pkg *, const char *);
static void     pkg_add_child(struct pkg *, struct pkg *);
static int      pkg_up2date(struct pkg *);
static void     pkg_make_args(struct pkg *, char **);
static void     pkg_print(struct pkg *);
static int      pkg_compile(struct pkg *);
static int      pkg_is_test(struct pkg *);
static int      pkg_is_main(struct pkg *);
static char **  pkg_get_tests(struct pkg *);
static time_t   pkg_modtime(struct pkg *);

// utility functions
static char *   test_name_from_signature(const char *);


struct pkg * new_pkg(char * n){

    struct pkg * p = malloc(sizeof(struct pkg));
    
    p->name      = n;
    p->indegree  = 0;
    p->h_file    = NULL;
    p->c_file    = NULL;
    p->o_file    = NULL;
    p->args      = NULL;
    p->children  = new_vector();
    p->deps      = new_strset();
    p->add_file  = &pkg_add_file;
    p->add_child = &pkg_add_child;
    p->make_args = &pkg_make_args;
    p->up2date   = &pkg_up2date;
    p->modtime   = &pkg_modtime;
    p->print     = &pkg_print;
    p->compile   = &pkg_compile;
    p->is_test   = &pkg_is_test;
    p->is_main   = &pkg_is_main;
    p->get_tests = &pkg_get_tests;
    
    return p;
};


static void pkg_add_file(struct pkg * slf, const char * fname){

    if(ends_with(fname, ".h")){
        slf->h_file = strdup(fname);
    }else if(ends_with(fname, ".c")){
        slf->c_file = strdup(fname);
    }else{
        panic("pkg.add_file only allows .h or .c files", __FILE__,__LINE__);
    }
    
    char ** imports = get_deps_from_fname(fname);

    if(imports){
        int i;
        for(i = 0; imports[i]; i++){
            if(! eq_str(imports[i], slf->name) ){
                slf->deps->add(slf->deps, imports[i]);
            }
        }
        free_strings(imports);
    }
};

static void pkg_add_child(struct pkg * slf, struct pkg * other){
    slf->children->add(slf->children, other);
};

static void pkg_make_args(struct pkg * slf, char ** xtra){

    if(slf->c_file){
        // replace .h with .o to create output
        char * tmp  = strdup(slf->c_file);
        size_t len  = strlen(tmp);
        tmp[len-1]  = 'o';
        slf->o_file = tmp;

        struct strvec * v = new_strvec();

        v->add(v, global_get_str("-backend"));
        v->add(v, "-I");
        v->add(v, global_get_str("-src"));
        v->add(v, "-c");
        v->add(v, slf->c_file);
        //TODO extra arguments go here...
        if(xtra){
            int i;
            for(i = 0; xtra[i]; i++){
                v->add(v, "-I");
                v->add(v, xtra[i]);
            }
        }
        v->add(v, "-o");
        v->add(v, slf->o_file);

        slf->args = v->join(v, " ");

        v->free(v);
    }
};

static int pkg_up2date(struct pkg * slf){

    time_t mod_c;
    time_t mod_h;
    time_t mod_o;
    
    if(! slf->c_file){ return 1; }
    if(! is_file(slf->o_file) ){ return 0; }
    
    //mod_h = modtime(slf->h_file);
    mod_c = modtime(slf->c_file);
    mod_o = modtime(slf->o_file);
    
    if( mod_c <= mod_o ){
        if( slf->h_file ){
            mod_h = modtime(slf->h_file);
            return ( mod_h <= mod_o );
        }
        return 1;
    }
    
    return 0;
};

static time_t pkg_modtime(struct pkg * slf){
    if(slf->o_file){
        return modtime(slf->o_file);
    }
    return 0;
};

static void pkg_print(struct pkg * slf){

    printf("\np  %s\n", slf->name);

    if(slf->h_file != NULL){ printf("f  %s\n", slf->h_file); }
    if(slf->c_file != NULL){ printf("f  %s\n", slf->c_file); }

    int i;
    char ** imports = slf->deps->to_array(slf->deps);
    
    if(imports){

        for(i = 0; imports[i]; i++){
            printf("d  %s\n", imports[i]);
        }

        free(imports);
    }
};

static int pkg_is_test(struct pkg * slf){

    if( slf->h_file && slf->c_file ){
        return ends_with(slf->h_file, "_test.h") && 
               ends_with(slf->c_file, "_test.c");
    }
    
    return 0;
};

// TODO fix this stuff
static int pkg_is_main(struct pkg * slf){

    if( ! slf->h_file ){
        return ends_with(slf->c_file, "main.c");
    }
    
    return 0;
};

static int pkg_compile(struct pkg * slf){

    int rv;
    int silent = global_get_bool("-quiet");

    if(slf->up2date(slf)) {
        if(! silent){ printf("up 2 date: %s\n", slf->name); }
    }else{
        rv = run(slf->args);
        if(! silent){ printf("compiling: %s\n", slf->name); }
        if( rv == -1 ){ panic("pkg_compile: cant fork()", __FILE__,__LINE__);}
        return rv;
    }
    return 0; // OK (nonzero indicates error)
};


void * pkg_compile_par(void * p){

    int * fail = malloc(sizeof(int));

    struct pkg * pk = (struct pkg *) p;
    *fail = pk->compile(pk);
    
    return (void *) fail;
    
};

void pkg_sem_init(int size){ 

    int fail;

    fail = sem_init(&job_semaphore, 0, size);

    if(fail){
        perror("fail to initialize semaphore");
        panic(NULL, __FILE__, __LINE__);
    }
};

void pkg_sem_destroy(void){
    sem_destroy(&job_semaphore);
};

void * pkg_compile_par_sem(void * p){

    void * rv;

    sem_wait(&job_semaphore);
    
    rv = pkg_compile_par(p);

    sem_post(&job_semaphore);

    return rv;

};

static char ** pkg_get_tests(struct pkg * slf){
    
    int fail;
    int i, rv;
    regex_t re;
    char * test_name;
    char ** res = NULL;
    
    char * regx = "(int|bool)\\s+test[^\\(]+\\((int|bool)\\)";
    fail = regcomp(&re, regx, REG_EXTENDED);
    if(fail){
        panic("regexp - didn't compile?", __FILE__, __LINE__);
    }
    
    // store names of test functions here
    struct strvec * sv = new_strvec();
    // test functions should be writte
    char ** content = slurp_lines(slf->h_file);
    
    for( i = 0; content[i]; i++){
        
        rv = regexec(&re, content[i], 0, NULL, 0);
        
        if( ! rv ){
            test_name = test_name_from_signature(content[i]);
            sv->add(sv, test_name);
        }else if( rv != REG_NOMATCH ){// stackoverflow.com :-)
            char msgbuf[200];
            regerror(rv, &re, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
            panic(NULL, __FILE__, __LINE__);
        }
    }
    
    // return value != NULL if header file contains tests
    if(sv->len(sv) > 0){
        res = sv->to_array(sv);
    }
    
    free_strings(content);
    regfree(&re);
    sv->free_all(sv);
    
    return res;
};

void free_pkg(void * p){
    
    struct pkg * slf;

    slf = (struct pkg *) p;
    
    slf->children->free(slf->children);
    slf->deps->free(slf->deps);

    free(slf->name);
    free(slf->h_file);
    free(slf->c_file);
    free(slf->o_file);
    free(slf->args);
    free(slf);

};

//__________________________________________util

static char * test_name_from_signature(const char * s){
    char * slice = slice_str(4, strlen(s), s);
    char * fn    = upto_char(slice, '(');
    char * fn_tr = trim_str(fn);
    free(slice);
    free(fn);
    return fn_tr;
};

