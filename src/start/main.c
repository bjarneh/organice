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


#include <stdio.h>
#include <stdlib.h>

#include "utilz/handy.h"
#include "compiler/pkg.h"
#include "compiler/dag.h"
#include "os/specific.h"
#include "path/pathutil.h"
#include "str/stringutil.h"
#include "utilz/global.h"
#include "utilz/timer.h"
#include "str/strvec.h"
#include "parse/options/copt.h"



// bool options
char * bools[] = {
	"-help",
	"-list",
	"-test",
	"-quiet",
	"-print",
	"-clean",
	"-fancy",
	"-sort",
	"-dryrun",
	"-single",
	"-static",
	"-version",
	"-verbose",
	NULL,
};

// string options
char * strs[] = {
	"-src",
	"-match",
	"-output",
	"-testbin",
	"-backend",
	NULL,
};

// option parser (getopt like thing)
struct copt * c  = NULL;
// dag
struct dag * dg  = NULL;
// regex that will match test package
char * testmatch = NULL;
// special takes multiple arguments
// link with libraries
char ** lnk   	 = NULL;
// search for libraries in
char ** look	 = NULL;
// unparsed arguments
char ** args  	 = NULL;
// simple timer thingy
struct timer * t = NULL;


char * helpmsg[] = {
	"",
	"oc - compiler front-end for C\n",
 	"usage: oc [OPTIONS]\n",
 	"options:\n",
    "-h --help      :  print this message and exit",
    "-v --version   :  print version and exit",
    "-d --dryrun    :  print what oc would do",
    "-p --print     :  print package info found",
    "-1 --single    :  do not compile in parallel",
    "-q --quiet     :  silent unless error occurs",
    "-V --verbose   :  verbose passed to unit-tests",
    "-c --clean     :  delete all generated objects",
    "-S --static    :  produce a statically linked binary",
    "-z --sort      :  print packages ordered (topsort)",
    "-i --list      :  print options and values",
    "-m --match     :  regexp to match main package",
    "-o --output    :  link main package -> output",
    "-t --test      :  run all unit-tests",
    "-T --testbin   :  name of test-binary (default:octest)",
    "-s --src       :  src dir (default: src)",
    "-b --backend   :  back-end [gcc,clang] (default:gcc)\n",
	"NOTE: legal gcc/clang options can be given\n",
	NULL,
};


void init(void){
	
	// initialize global map
	global_init();

	// initialize timer, and start 'everything'
	t = new_timer();
	t->start(t, "all");
	
	// create option parser
    c = new_copt();
    
    c->bool_option(c, "-h -help --help help");
    c->bool_option(c, "-v -version --version");
    c->bool_option(c, "-V -verbose --verbose");
    c->bool_option(c, "-d -dryrun --dryrun");
    c->bool_option(c, "-p -print --print print");
    c->bool_option(c, "-1 -single --single");
    c->bool_option(c, "-c -clean --clean clean");
    c->bool_option(c, "-t -test --test test");
    c->bool_option(c, "-S -static --static");
    c->bool_option(c, "-z -sort --sort sort");
    c->bool_option(c, "-q -quiet --quiet");
    c->bool_option(c, "-i -list --list");
    c->bool_option(c, "-f -fancy --fancy");
    c->str_option(c, "-L -L=");
    c->str_option(c, "-l -l=");
    c->str_option_fancy(c, "-o --output");
    c->str_option_fancy(c, "-m --match");
    c->str_option_fancy(c, "-T --testbin");
    c->str_option_fancy(c, "-s --src");
    c->str_option_fancy(c, "-b --backend");

	// hopefully the source forms a DAG
	dg = new_dag();
	
	// default bool and str flags
	int i;
	for(i = 0; bools[i] != NULL; i++){
		global_set_bool(bools[i], 0);
	};


	for(i = 0; strs[i] != NULL; i++){
		global_set_str_blank(strs[i]);
	};
	
	// -src does not default to '' but to 'src'
	global_set_str("-src", "src");	
	// -backend defaults to 'gcc'
	global_set_str("-backend", "gcc");	
	// -testbin defaults to octest[.exe]
	global_set_str("-testbin", is_windows()? "octest.exe" : "octest");

};

void refill_globals(void){
	
	int i;
	char * tmp;
	
	for(i = 0; bools[i] != NULL; i++){
		if(c->is_set(c, bools[i])){
			global_set_bool(bools[i], 1);
		}
	};

	for(i = 0; strs[i] != NULL; i++){
		if(c->is_set(c, strs[i])){
			tmp = c->get(c, strs[i]);
			global_set_str(strs[i], tmp);
			free(tmp);
		}
	};

	// special make sure '-src' is clean
	if(c->is_set(c, "-src")){
		tmp = path_clean(global_get_str("-src"));
		global_set_str("-src", tmp);
		free(tmp);
	}
	
	if(c->is_set(c, "-l")){

		if( lnk ){
			lnk = add_arr_free(lnk, c->get_all(c, "-l"));
		}else{
			lnk = c->get_all(c, "-l");
		}

	}
	
	if(c->is_set(c, "-L")){
		if( look ){
			look = add_arr_free(look, c->get_all(c, "-L"));
		}else{
			look = c->get_all(c, "-L");
		}
	}

	c->reset(c);	
};

// opposite of init()
void endit(void){

    // report time and free timer
    if(! global_get_bool("-quiet") && ! global_get_bool("-dryrun") ){
        t->stop(t, "all");
        double secs = t->seconds(t, "all");
		printf("time used: %.3fs\n", secs);
    }
    t->free(t);

    // free option parser
    c->free(c);
    // free dag
    dg->free(dg);
    // free global map
    global_free();
	// free unparsed arguments
    free_strings(args);
    // free link dirs
    free_strings(lnk);
    // free paths to library dirs
    free_strings(look);
    // free possible test-package regex
    free(testmatch);
};


void listing(void){

	int i;	
	char * tmp;
	
	for(i = 0; bools[i] != NULL; i++){
		tmp = (global_get_bool(bools[i]))? "true" : "false";
		printf("%10s  =>  %s\n", bools[i], tmp);
	};
	for(i = 0; strs[i] != NULL; i++){
		printf("%10s  =>  '%s'\n", strs[i], global_get_str(strs[i]));
	};

	if(lnk){
		printf("%10s  => ", "-l");
		for(i = 0; lnk[i]; i++){ printf(" '%s'", lnk[i]); }
		puts("");
	}

	if(look){
		printf("%10s  => ", "-L");
		for(i = 0; look[i]; i++){ printf(" '%s'", look[i]); }
		puts("");
	}
	
	exit(0);

};


void usage(int code){

	int i;
	FILE * fd = stdout;

	if(code){ fd = stderr; }
	
	for(i = 0; helpmsg[i] != NULL; i++){
		fprintf(fd, "  %s\n", helpmsg[i]);
	}

	exit(code);

};

void parse_this_config(const char * fname){

	if(is_file(fname)){
		char ** argv = config2args(fname);
		if(argv){
			char ** xargs = c->parse_arr(c, argv);
			refill_globals();
			free_strings(xargs);
			free_strings(argv);
		}
	}
};

void parse_config(){

	char * home = getenv(get_home_env());
	char * pwd  = getenv("PWD");
	char * xdg  = getenv("XDG_CONFIG_HOME");
	char * tmp;
	
	// ${XDG_CONFIG_HOME}/oc/ocrc
	if(is_windows() && xdg){
		tmp = add_str(xdg, "/oc/ocrc");
		parse_this_config(tmp);
		free(tmp);
	}

	if(home){
		// ${HOME}/.config/oc/ocrc
		tmp = path_from_slash(add_str(home, "/.config/oc/ocrc"));
		parse_this_config(tmp);
		free(tmp);

		// ${HOME}/.ocrc
		tmp = path_from_slash(add_str(home, "/.ocrc"));
		parse_this_config(tmp);
		free(tmp);
	};
	
	if(pwd){
		// ${PWD}/.ocrc
		tmp = path_from_slash(add_str(pwd, "/.ocrc"));
		parse_this_config(tmp);
		free(tmp);
		
		// ${PWD}/ocrc
		tmp = path_from_slash(add_str(pwd, "/ocrc"));
		parse_this_config(tmp);
		free(tmp);
	};

};


int main(int argc, char ** argv){

	// allocate resources
	init();
	
	// parse configuration files
	parse_config();

	// parse command line arguments
    args = c->parse_argv(c, argc, argv);
    refill_globals();
    
    if(args == NULL){
        puts("args == NULL");
    }

	// simple commands (i.e. print stuff)
	if(global_get_bool("-list")){ listing(); }
	if(global_get_bool("-help")){ usage(EXIT_SUCCESS); }
	if(global_get_bool("-version")){
		puts("oc 1.0");
		exit(EXIT_SUCCESS); 
	}
	
	// allow first non-option to be src-dir
	if(args && is_dir(args[0]) ){
		global_set_str_nodup("-src", path_clean(args[0]));
		args = shift_strs(args);
	}
	
	// exit with full help message
	if( ! is_dir(global_get_str("-src")) ){
		fprintf(stderr, "'%s' : not a directory\n", global_get_str("-src"));
		usage(EXIT_FAILURE);
	};

	// pathwalk (-src) + parse imports
	dg->parse(dg);

	// info about dependencies and so on for each package	
	if(global_get_bool("-print")){	dg->print(dg); exit(EXIT_SUCCESS); }

	// build dependency graph
	dg->build(dg, args);

	// for debugging
	if(global_get_bool("-fancy")){	dg->fancy(dg); exit(EXIT_SUCCESS);  }
	// remove objects
	if(global_get_bool("-clean")){	dg->clean(dg); exit(EXIT_SUCCESS);  }


	// get legal compile order (loop check)
	dg->topsort(dg);

	// print packages in legal compile order
	if(global_get_bool("-sort")){	dg->sort(dg); exit(EXIT_SUCCESS); }
	
	if(global_get_bool("-test")){
		testmatch = dg->add_test(dg, args);
	}
	
	// the default is to run each command in parallel
	if(! global_get_bool("-dryrun") && !global_get_bool("-single") ){
		dg->pcompile(dg); // parallel compile
	}else{
		dg->compile(dg);
	}

    // link main package of generated test code
    if(global_get_bool("-test")){

		dg->link(dg, global_get_str("-testbin"), testmatch, look, lnk);

        if(! global_get_bool("-quiet") ){
            int verb = global_get_bool("-verbose");
            char * out = (verb)? "testing  :\n" : "testing  : ";
            printf("%s",out);
        }

        // common point of reference
        setenv("SRCROOT", global_get_str("-src"), 1);

        const char * dirs[2];
        dirs[0] = getenv("PWD");
        dirs[1] = global_get_str("-testbin");

        char * path_to_testbin = path_join_len(dirs, 2);

        int fail = run(path_to_testbin);

        if(!fail && !global_get_bool("-quiet")){
            puts("PASS");
        }

        dg->unlink_test(dg, testmatch);

        free(path_to_testbin);
    }

	// if output is given try to link main package
	if(! eq_str("", global_get_str("-output"))){
	    dg->link(dg, global_get_str("-output"), global_get_str("-match"), look, lnk);
	}
	
	// free resources
	endit();
	
    return 0;
};


