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
 

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#include "map/hash.h"
#include "compiler/pkg.h"
#include "compiler/dag.h"
#include "os/specific.h"
#include "os/spawn/jobs.h"
#include "utilz/handy.h"
#include "utilz/global.h"
#include "path/walk.h"
#include "path/pathutil.h"
#include "str/buffer.h"
#include "str/strvec.h"
#include "str/stringutil.h"
#include "parse/source/code.h"



// dag method signatures
static void	  dag_free(struct dag *);
static void	  dag_print(struct dag *);
static void	  dag_fancy(struct dag *);
static void	  dag_parse(struct dag *);
static void	  dag_build(struct dag *, char **);
static void	  dag_sort(struct dag *);
static void	  dag_link(struct dag *, char *, char *, char **, char **);
static void	  dag_topsort(struct dag *);
static void	  dag_compile(struct dag *);
static void	  dag_compile_par(struct dag *);
static void	  dag_clean(struct dag *);
static char * dag_add_test(struct dag *, char **);
static void   dag_unlink_test(struct dag *, char *);
static void	  dag_dryrun(struct dag *);
// private method signature
static int           num_main_pkgs(struct dag *);
static struct pkg ** filter_relevant_main(struct dag *, char *);
static struct pkg *  get_pkg_from_name(struct dag *, const char *);
// private functions
static int 		all_c_code(const char *);
static int 		c_code_no_tests(const char *);
static int 		dag_binary_up2date(struct pkg **, const char *);


struct dag * new_dag(void){

	struct dag * d = malloc(sizeof(struct dag));
	
    d->pkgs        = new_hash();
    d->sorted      = NULL;
    d->parse       = &dag_parse;
    d->build       = &dag_build;
    d->topsort     = &dag_topsort;
    d->compile     = &dag_compile;
    d->pcompile    = &dag_compile_par;
    d->clean       = &dag_clean;
    d->dryrun      = &dag_dryrun;
    d->sort        = &dag_sort;
    d->link        = &dag_link;
    d->print       = &dag_print;
    d->fancy       = &dag_fancy;
    d->add_test    = &dag_add_test;
    d->unlink_test = &dag_unlink_test;
	d->free		   = &dag_free;

	return d;
};

static void dag_parse(struct dag * slf){

	int i;
	struct pkg * pk; 
	char * pkg_name;
	struct hash * hs = slf->pkgs;
	const char * root = global_get_str("-src");
	int (*filter)(const char *);

    int include_tests = global_get_bool("-test") || global_get_bool("-clean");

	filter = (include_tests)? &all_c_code : &c_code_no_tests;

	char ** files = path_walk_filter(root, filter);
	
	if( files != NULL ){
		for(i = 0; files[i] != NULL; i++){
			pkg_name = pkg_name_from_fname(root, files[i]);
			if(hs->has(hs, pkg_name)){
				pk = hs->get(hs, pkg_name);
				pk->add_file(pk, files[i]);
				free(pkg_name);
			}else{
				pk = new_pkg(pkg_name);
				pk->add_file(pk, files[i]);
				hs->put(hs, pkg_name, pk);
			}
		}
	}

	free_strings(files);

};

static void dag_build(struct dag * slf, char ** xtra){
	
	int i, j;
	struct pkg * pk;
	struct pkg * parent;
	char ** pkg_deps;
	char ** pkg_names = slf->pkgs->keys(slf->pkgs);

	if(pkg_names){
		
		// create connections + compiler arguments
		for(i = 0; pkg_names[i]; i++){
			
			pk = get_pkg_from_name(slf, pkg_names[i]);
			//printf("\npkg: %s\n", pk->name);
			pkg_deps = pk->deps->to_array(pk->deps);

			if(pkg_deps){
				for(j = 0; pkg_deps[j] ; j++){
					if(slf->pkgs->has(slf->pkgs, pkg_deps[j])){
						parent = get_pkg_from_name(slf, pkg_deps[j]);
						parent->add_child(parent, pk);
						pk->indegree += 1;
					}else{
						struct buffer * b = new_buffer();
						b->add(b, "cannot find pkg: ");
						b->add(b, pkg_deps[j]);
						b->add(b, " (imported from: ");
						b->add(b, pkg_names[i]);
						b->add(b, ")");
						panic(b->str(b), __FILE__, __LINE__ );
					}
				}
				free(pkg_deps);
			}
			
			// create arguments needed to compile pk
			pk->make_args(pk, xtra);
		}
		
		free(pkg_names);
	}
};

static void dag_sort(struct dag * slf){
	int i;
	if(slf->sorted){
		for(i = 0; slf->sorted[i]; i++){
			printf("%s\n", slf->sorted[i]->name);
		}
	}
};

static void dag_dryrun(struct dag * slf){
	int i;
	if(slf->sorted){
		for(i = 0; slf->sorted[i]; i++){
			printf("%s\n", slf->sorted[i]->args);
		}
	}
};

static void dag_print(struct dag * slf){

	int i;
	char ** keys;
	struct pkg * pk;
	
	puts("--------------------------------------");
	puts("Packages and Dependencies");
	puts("p = package, f = file, d = dependency ");
	puts("--------------------------------------");
	
	keys = slf->pkgs->keys(slf->pkgs);

	for(i = 0; keys[i] != NULL; i++){
		pk = get_pkg_from_name(slf, keys[i]);
		pk->print(pk);
	}
	
	puts("");
	
	free(keys);
};

static void	dag_fancy(struct dag * slf){
	
	int i, j;
	char ** keys;
	struct pkg * pk;
	struct pkg * child;
	
	puts("--------------------------------------");
	puts("            FANCY PRINT               ");
	puts("--------------------------------------");

	keys = slf->pkgs->keys(slf->pkgs);

	for(i = 0; keys[i] != NULL; i++){
		pk = get_pkg_from_name(slf, keys[i]);
		printf("\nname     :  %s\n", pk->name);
		printf("indegree :  %d\n", pk->indegree);
		for(j = 0; j < pk->children->len; j++){
			child = (struct pkg *) pk->children->_[j];
			printf("after me :  %s\n", child->name);
		}
	}
	
	puts("");
	
	free(keys);
};

static void dag_topsort(struct dag * slf){
	
	int count = 0, i = 0, j = 0, total = 0;

	struct pkg * pk;
	struct pkg * child;

	struct vector * zero = new_vector();
	struct vector * done = new_vector();

	struct pkg ** sorted;
	
	char ** packages = slf->pkgs->keys(slf->pkgs);

	for(i = 0; packages[i]; i++){ total++; };
	
	for(i = 0; packages[i]; i++){
		pk = get_pkg_from_name(slf, packages[i]);
		if(pk->indegree == 0){
			zero->add(zero, pk);
		}
	}

	while( zero->len > 0 ){
		pk = (struct pkg *) zero->pop(zero);
		for(j = 0; j < pk->children->len; j++){
			child = (struct pkg *) pk->children->_[j];
			child->indegree -= 1;
			if(child->indegree == 0){
				zero->add(zero, child);
			}
		}
		count += 1;
		done->add(done, pk);
	};
	
	if(count < total){
		panic("loop in dependency graph", __FILE__, __LINE__);
	}

    // +2 due to possible test-package that might be added
	sorted = calloc(done->len + 2, sizeof(struct pkg *));
	
	for(i = 0; i < done->len; i++){
		sorted[i] = (struct pkg *) done->_[i];
	}
	sorted[i] = NULL;

	free(packages);
	zero->free(zero);
	done->free(done);
	
	slf->sorted = sorted;
};

static void dag_compile(struct dag * slf){
	
	if(global_get_bool("-dryrun")){
		slf->dryrun(slf);
		return;
	}
	
	int i, rv;
	struct pkg * pk;
	
	if(slf->sorted){

		for(i = 0; slf->sorted[i]; i++){

			pk = slf->sorted[i];
			rv = pk->compile(pk);
			
			if( rv != 0 ){
				exit(EXIT_FAILURE);
			}
		}		
	}
};

static void dag_compile_par(struct dag * slf){

    int use_semaphore  = 0;
    int size_semaphore = 0;

    size_semaphore = atoi(global_get_str("-jobs"));

    if( size_semaphore > 0 ){
        use_semaphore  = 1;
        pkg_sem_init(size_semaphore);
    }

	if(slf->sorted){

		int i, max = 0;
		for(i = 0; slf->sorted[i]; i++){ max++; }
		
		struct job ** jobs = calloc(max+1, sizeof(struct job *));
		
		for(i = 0; i < max; i++){
			jobs[i] = new_job();
            if( use_semaphore ){
			    jobs[i]->fn = &pkg_compile_par_sem;
            }else{
			    jobs[i]->fn = &pkg_compile_par;
            }
			jobs[i]->arg = (void *) slf->sorted[i];
		}
		jobs[max] = NULL;
		
		int fail = spawn_jobs(i, jobs);
		if(fail){
			panic("parallel compile failed", __FILE__, __LINE__);
		}
		
		for(i = 0; jobs[i]; i++){
			free(jobs[i]);
		}
		
		free(jobs);
	}

    if(use_semaphore){
        pkg_sem_destroy();
    }
};

static int dag_binary_up2date(struct pkg ** pks, const char * out){

	if(! is_file(out)){
		return 0;
	}
	
	int i;
	time_t tmp;
	time_t latest = 0;
	time_t out_time = modtime(out);
	
	for(i = 0; pks[i]; i++){
		tmp = pks[i]->modtime(pks[i]);
		if( tmp && tmp > latest ){
			latest = tmp;
		}
	}
	
	return latest <= out_time;
};

static void 
dag_link(struct dag * slf,char *output, char *match, char ** look, char ** lnk)
{
	
	int i, rv, main_count;
	char * args;
	struct pkg ** pks;
	
	if(slf->sorted){
		
		main_count = num_main_pkgs(slf);

		if( main_count > 1 ){
            pks = filter_relevant_main(slf, match);
		}else{
            pks = slf->sorted;
		}
		
		if( ! global_get_bool("-dryrun") ){
            if( dag_binary_up2date( pks , output ) ){
                if(! global_get_bool("-quiet")){
                    printf("up 2 date: %s\n", output );
                }
                return;
            }
		}

		struct strvec * v = new_strvec();
		
		v->add(v, global_get_str("-backend"));
		v->add(v, "-o");
		v->add(v, output );

		for(i = 0; pks[i]; i++){
			if(pks[i]->o_file){
				v->add(v, pks[i]->o_file);
			}
		}
		
		if(v->len(v) > 3){
			
			if(!global_get_bool("-quiet") && !global_get_bool("-dryrun") ) {
				printf("linking  : %s\n", output );
			}
			
			if( global_get_bool("-static") ){
				v->add(v, "-static");
			}
			
			if(look){
				for(i = 0; look[i]; i++){
					v->add(v, "-L");
					v->add(v, look[i]);
				}
			}

			if(lnk){
				for(i = 0; lnk[i]; i++){
					v->add(v, "-l");
					v->add(v, lnk[i]);
				}
			}

			args = v->join(v, " ");

			if(global_get_bool("-dryrun")) {
				printf("%s\n", args);
			}else{
				rv = run(args);
				if(rv == -1){ panic("failed to fork()",__FILE__,__LINE__); }
				if(rv){ exit(EXIT_FAILURE); }
			}
			
			free(args);
		}
		v->free(v);

        // we have allocated a new array if we filtered
        if( main_count > 1 ){
            free(pks);
        }
	}
};


static void dag_clean(struct dag * slf){

	int i, silent, rv;
	struct pkg * pk;
	char ** packages = slf->pkgs->keys(slf->pkgs);
	
	if(packages){

		silent = global_get_bool("-quiet");
		
		for(i = 0; packages[i]; i++){
			pk = get_pkg_from_name(slf, packages[i]);
			if(pk->o_file && is_file(pk->o_file)){
				if(! silent){
					printf("rm: %s\n", pk->o_file);
				}
				rv = unlink(pk->o_file);
				if(rv){
					perror(pk->o_file);
					panic("failed to delete file", __FILE__, __LINE__);
				}
			}
		}		
	}
};


static char * dag_add_test(struct dag * slf, char ** xtra){

	int i;
	struct pkg * pk;
	char ** tests;
    char tmpdir[11];
    char * test_regex = NULL;
	struct strvec * hv = new_strvec();
	struct strvec * tv = new_strvec();

	if(slf->sorted){
		
		for(i = 0; slf->sorted[i]; i++){

			pk = slf->sorted[i];

			if(pk->is_test(pk)){
				tests = pk->get_tests(pk);
				if(tests){
					tv->add_array(tv, tests);
					hv->add(hv, add_str(pk->name, ".h"));
					free(tests);
				}
			};
		}		
		
	}else{
		panic("dag_add_test: pkg's not found!", __FILE__, __LINE__);
	}
	
	if(tv->len(tv) > 0){
		
		struct buffer * b  = new_buffer();

		b->add(b, "\n// Auto generated code..\n");
		b->add(b, "// http://oc.googlecode.com for documentation\n");
		b->add(b, "\n// imports\n#include <stdio.h>\n\n");
	
		for(i = 0; i < hv->len(hv); i++){
			b->add(b, "#include \"");
			b->add(b, hv->at(hv, i));
			b->add(b, "\"\n");
		}
		
		b->add(b, "\n\n#define TOTAL_NUM_TESTS ");
		char cbuf[20]; // should be enough space 
		sprintf(cbuf, "%d\n", tv->len(tv));
		b->add(b, cbuf);
		
		b->add(b, "\n\nstruct unit_test{\n");
		b->add(b, "  char * name;\n");
		b->add(b, "  int  (*fn)(int);\n");
		b->add(b, "} tests[TOTAL_NUM_TESTS] = {\n");

		tests = tv->to_array(tv);

		for(i = 0; tests[i]; i++){
			b->add(b, "  {\"");
			b->add(b, tests[i]);
			b->add(b, "\", (int (*)(int))");
			b->add(b, tests[i]);
			b->add(b, "},\n");
		};
		    
		free_strings(tests); // tv->to_array creates copies
		
		b->add(b, "};\n\n");

		b->add(b, "int main(int argc, char ** argv){\n\n");
		b->add(b, "  int i, ok, failed = 0;\n");
		b->add(b, "  int verbose = ");
		b->add(b, global_get_bool("-verbose")? "1;" : "0;");
		b->add(b, "\n\n");
		b->add(b, "  for( i = 0; i < TOTAL_NUM_TESTS; i++ ){\n\n");
		b->add(b, "    ok = tests[i].fn(verbose);\n\n");
		b->add(b, "    if(! ok){\n");
		b->add(b, "      fprintf(stderr,\"NO       : %s\\n\", tests[i].name);\n");
		b->add(b, "      failed = 1;\n");
		b->add(b, "    }else{\n");
		b->add(b, "      if(verbose){\n");
		b->add(b, "        printf(\"OK       : %s\\n\", tests[i].name);\n");
		b->add(b, "      }\n");
		b->add(b, "    }\n");
		b->add(b, "  }\n");
		b->add(b, "  return failed;\n");
		b->add(b, "}\n\n");
		
        // create content of package in filetree
        const char * test_path[3];
        sprintf(tmpdir, "%ld", time(NULL));
        test_path[0] = global_get_str("-src");
        test_path[1] = tmpdir;
        test_path[2] = "octestmain";

        // we can use timestamp as regex to match test pkg
        test_regex = strdup(tmpdir);

        char * test_dir = path_join_len(test_path, 2);

        mkdir_or_die(test_dir);

        char * pkg_name         = path_join_len(&test_path[1], 2);
        test_path[2] = "octestmain.c";
        char * test_path_joined = path_join_len(test_path, 3);
        
        // spit content to file
		char * test_content = b->str(b);
        spit_str(test_path_joined, test_content);

        // add newly created package to hash
        pk = new_pkg(pkg_name);
        pk->add_file(pk, test_path_joined);
        pk->make_args(pk, xtra);
        slf->pkgs->put( slf->pkgs, pkg_name, pk);
        // add newly create package to sorted (it's got room for it)
        for( i = 0; slf->sorted[i]; i++){;} // figure out stop
        slf->sorted[i] = pk;
        
        free(test_dir);
        free(test_path_joined);
		free(test_content);
		b->free(b);
	}
	
	tv->free_all(tv);
	hv->free_all(hv);

    return test_regex;
};

static void dag_unlink_test(struct dag * slf, char * tstamp){
    
    // test package should be last package of slf->sorted
    int i, fail;
    for(i = 0; slf->sorted[i]; i++);

    struct pkg * pk;
    pk = slf->sorted[i-1];

    // we can assume all is fine if this is ok
    if( starts_with( pk->name, tstamp) ){

        fail = unlink(pk->c_file);
        if(fail){
            perror(pk->c_file);
            panic(NULL, __FILE__, __LINE__);
        }

        fail = unlink(pk->o_file);
        if(fail){
            perror(pk->o_file);
            panic(NULL, __FILE__, __LINE__);
        }
    }

    const char * testdir[2];
    testdir[0] = global_get_str("-src");
    testdir[1] = tstamp;

    char * testpath = path_join_len(testdir, 2);
    fail = rmdir(testpath);
    if(fail){
        perror(testpath);
        panic(NULL,__FILE__,__LINE__);
    }

    free(testpath);

    if(is_file(global_get_str("-testbin"))){
        fail = unlink(global_get_str("-testbin"));
        if(fail){
            perror(global_get_str("-testbin"));
            panic(NULL,__FILE__,__LINE__);
        }
    }
};

static void dag_free(struct dag * slf){
	slf->pkgs->free_all(slf->pkgs, &free_pkg);
	free(slf->sorted);
	free(slf);
};

//____________________________________________private

static struct pkg * get_pkg_from_name(struct dag * d, const char * n){
	return (struct pkg *) d->pkgs->get(d->pkgs, n);
};

static int num_main_pkgs(struct dag * slf){
	
	int i;
	int cnt = 0;
	struct pkg * pk;
	
	for( i = 0; slf->sorted[i]; i++ ){
		pk = slf->sorted[i]; // just for readability (next line is readable)
		if( pk->is_main(pk) ){ cnt++; }
	};

	return cnt;
};


static struct pkg ** filter_relevant_main(struct dag * slf, char * filter){
    
    //TODO ask user to select main pkg instead
    if(strlen(filter) == 0 || is_whitespace(filter) ){
        fprintf(stderr,"[ERROR] multiple main packages found in directory\n");
        fprintf(stderr,"[ERROR] no -m/--match option given to select one\n");
        panic("dag: filter_relevant", __FILE__, __LINE__);
    }

    int i, fail;
    struct pkg * pk;
    struct vector * v = new_vector();

    regex_t re;
    fail = regcomp(&re, filter, REG_EXTENDED);
    if(fail){
        fprintf(stderr,"regex: %s - didn't compile\n", filter);
        panic("dag: filter_relevant_main", __FILE__, __LINE__);
    }

    for(i = 0; slf->sorted[i]; i++){
        pk = slf->sorted[i];
        if( pk->is_main(pk) ){
            fail = regexec(&re, pk->name, 0, NULL, 0);
            if(! fail){
                v->add(v, pk);
            }
        }
    };

    if(v->len == 0){
        fprintf(stderr,"[ERROR] no main package matches '%s'\n", filter);
        panic("dag: filter_relevant", __FILE__, __LINE__);
    }

    if(v->len > 1){
        fprintf(stderr,"[ERROR] multiple main packages match '%s'\n", filter);
        for(i = 0; i < v->len; i++){
            pk = (struct pkg *) v->_[i]; 
            fprintf(stderr,"[ERROR] match: '%s'\n", pk->name );
        }
        panic("dag: filter_relevant", __FILE__, __LINE__);
    }

    // v->len == 1
    for( i = 0; slf->sorted[i]; i++ ){
        pk = slf->sorted[i];
        if( ! pk->is_main(pk) ){
            v->add(v, pk);
        }
    }

    struct pkg ** relevant = calloc(v->len + 1, sizeof(struct pkg *));

    for( i = 0; i < v->len; i++){
        relevant[i] = (struct pkg *) v->_[i];
    }

    v->free(v);
    regfree(&re);

    return relevant;
};

//____________________________________________utility


static int all_c_code(const char * f){

	if(is_dir(f) && !starts_with(f,".")){ return 1; }
	
	if(starts_with(f, "_")){ return 0; } //TODO fixme

	if(ends_with(f, ".h") || ends_with(f, ".c")){
		return 1;
	}

	return 0;
};

static int c_code_no_tests(const char *f){

	if(all_c_code(f)){
		if(ends_with(f, "_test.c") || ends_with(f, "_test.h")){
			return 0;
		}
		return 1;
	}
	return 0;
};


