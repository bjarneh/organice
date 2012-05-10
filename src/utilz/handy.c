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


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "str/buffer.h"
#include "str/stringutil.h"
#include "str/strvec.h"
#include "lists/vector.h"
#include "utilz/handy.h"

// public functions

int assert_log(int ok, int verbose, const char* info, int * ret){
    if(! ok){
        if(verbose && info != NULL){
            fprintf(stderr,"%s\n", info);
        }
        (*ret) = 0;
        return 0;
    }
    return 1;
};

int is_file(const char * path){
    if(! path){ return 0; }
    struct stat st;
    if(stat(path, &st) != 0){ return 0; }
    return S_ISREG(st.st_mode);
};

int is_dir(const char * path){
    if(! path){ return 0; }
    struct stat st;
    if(stat(path, &st) != 0){ return 0; }
    return S_ISDIR(st.st_mode);
};

void panic(const char * msg, const char * file, int line){
    fprintf(stderr,"[ERROR] in %s:%d\n", file, line);
    if(msg != NULL){ fprintf(stderr,"[ERROR] %s\n", msg); }
    exit(1);
};

time_t modtime(const char * path){
    struct stat st;
    if(stat(path, &st) != 0){
        perror(path);
        panic(NULL, __FILE__, __LINE__);
    }
    return st.st_mtime;
};

char * slurp(const char * fname, long * size){

    size_t ok_read;
    char * buf  = NULL;
    FILE * fd = fopen(fname, "rb");
    
    if(fd){
        if( fseek(fd, 0, SEEK_END) != 0 ){
            perror(fname);
            panic(NULL, __FILE__, __LINE__);
        }
        (*size) = ftell(fd);
        if( fseek(fd, 0, SEEK_SET) != 0){
            perror(fname);
            panic(NULL, __FILE__, __LINE__);
        }

        buf = malloc((*size));

        if(buf != NULL){
            ok_read = fread(buf, 1, (*size), fd);
            if(ok_read != (*size)){
                perror(fname);
                panic(NULL, __FILE__, __LINE__);
            }
        }else{
            perror(fname);
            panic(NULL, __FILE__, __LINE__);
        }
    }else{
        perror(fname);
        panic(NULL, __FILE__, __LINE__);
    }
    
    fclose(fd);
    
    return buf;
};

//TODO fixme hack etc
char * slurp_str(const char * fname){

    long size;
    char * b = slurp(fname, &size);
    char * s = calloc(size + 1, sizeof(char));
    memcpy(s, b, size);
    
    free(b);
    return s;
};

char ** slurp_lines(const char * fname){

    long i;
    long size;
    char * content = slurp(fname, &size);
    struct buffer * b = new_buffer();
    struct vector * v = new_vector();
    
    for(i = 0; i < size; i++){
        if(content[i] == '\n'){
            v->add(v, b->str(b));
            b->clear(b);
        }else{
            b->add_char(b, content[i]);
        }
    }

    if(b->len > 0){
        v->add(v, b->str(b));
    }
    
    char ** lines = calloc(v->len +1, sizeof(char *));
    for(i = 0; i < v->len; i++){
        lines[i] = (char *) v->_[i];
    }

    free(content);
    b->free(b);
    v->free(v);
    
    return lines;
};


void spit_str(const char * fname, const char * content){
    spit_bytes(fname, content, strlen(content));
};


void spit_bytes(const char * fname, const char * content, size_t size){
    
    FILE * fd = fopen(fname, "w");
    
    if(fd){
        
        if( fwrite(content, 1, size, fd) != size ){
            perror(fname);
            panic(NULL, __FILE__, __LINE__);
        }

        fclose(fd);

    }else{
        perror(fname);
        panic(NULL, __FILE__, __LINE__);
    }
};


char ** config2args(const char * fname){

    if(! is_file(fname) ){ return NULL; }
    
    char ** res   = NULL;
    char ** lines = slurp_lines(fname);

    if(lines){

        struct strvec * vec = new_strvec();

        int i, j;

        for(i = 0; lines[i]; i++){

            char * rm_comment  = upto_char(lines[i], '#');
            char ** tokens     = split_white(rm_comment);
            free(rm_comment);
            
            if( tokens ){
                for(j = 0; tokens[j]; j++){
                    vec->add(vec, strdup(tokens[j]));
                }
                free_strings(tokens);
            }
        }
        res = vec->to_array(vec);
        vec->free_all(vec);
    }

    free_strings(lines);
    
    return res;
};

void mkdir_or_die(const char * path){
    if(mkdir(path, 0777) != 0){
        perror(path);
        panic(NULL, __FILE__, __LINE__);
    }
};
