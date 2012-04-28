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


#include <string.h>
#include "utilz/handy.h"
#include "parse/options/copt.h"
#include "parse/options/copt_test.h"

int test_copt(int verbose){

    int i;
    int ok = 1;
    int argc = 7;
    char *argv[] = {"a","-I/lib","-I","/usr/lib","b","-output=out","-vh"};
    char ** args;
    char * o_val;

    struct copt * c = new_copt();

    c->str_option(c, "-I");
    c->str_option_fancy(c, "-o --output");
    c->bool_option(c, "-h -help --help");
    c->bool_option(c, "-v -version --version");

    args = c->parse(c, argc, argv);

    assert_log( c->is_set(c,"-I"), verbose, "-I not set", &ok);
    assert_log( c->is_set(c,"-output"), verbose, "-o not set", &ok);
    assert_log( c->is_set(c,"-v"), verbose, "-v not set", &ok);
    assert_log( c->is_set(c,"-help"), verbose, "-h not set", &ok);

    o_val = c->get(c, "-output");
    assert_log( !strcmp(o_val, "out"), verbose, "-o != out", &ok);

    return ok;
};
