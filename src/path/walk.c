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


#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "os/specific.h"
#include "path/walk.h"
#include "path/pathutil.h"
#include "utilz/handy.h"
#include "str/stringutil.h"
#include "lists/vector.h"

// private functions
static int ok_all_files(const char *);
static void walker(struct vector *, const char *, int (*)(const char *));




static int ok_all_files(const char * f){ return 1; };

char ** path_walk(const char * root){
    return path_walk_filter(root, &ok_all_files);
};

char ** path_walk_filter(const char * root, int (*ok)(const char *)){
    
    int i;
    
    char * clean_root = path_clean(root);
    char ** fnames = NULL;
    struct vector * v = new_vector();

    walker(v, clean_root, ok);

    if(v->len > 0){
        fnames = calloc(v->len + 1, sizeof(char *));
        for(i = 0; i < v->len; i++){
            fnames[i] = (char*) v->at(v, i);
        }
        fnames[i] = NULL;
    }
    
    v->free(v);
    free(clean_root);
    
    return fnames;
};

static void 
walker(struct vector * v, const char * root, int (*ok)(const char *)){

    DIR *dp;
    struct dirent *ep;
    char * sub;
    const char ** tmp = calloc(3, sizeof(char *));
    
    if( is_dir(root) && ok(root) ){
        
        dp = opendir(root);

        while( (ep = readdir(dp)) ){
            if(strcmp(ep->d_name, ".") & strcmp(ep->d_name, "..")){
                tmp[0] = root;
                tmp[1] = ep->d_name;
                tmp[2] = NULL;
                sub    = join_str(tmp, dir_sep());
                if( is_file(sub) && ok(sub) ){
                    v->add(v, sub);
                }else{
                    walker(v, sub, ok);
                    free(sub);
                }
            }
        }
        
        closedir(dp);
    }
    
    free(tmp);

};
