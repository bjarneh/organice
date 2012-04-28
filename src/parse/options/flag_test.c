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


#include <stdio.h>
#include <string.h>
#include "utilz/handy.h"
#include "parse/options/flag.h"
#include "parse/options/flag_test.h"

int test_flag(int verbose){
    
    int i, conter;
    int ok = 1;
    char ** opts;

    struct flag * f = new_str_flag("-f --file");
    assert_log( ! f->is_bool, verbose, "flag->is_bool", &ok);
    assert_log( ! f->is_set(f), verbose, "flag->is_set", &ok);

    f->add(f, "abc");
    assert_log( f->is_set(f), verbose, "! flag->is_set", &ok);
    assert_log( f->get(f) != NULL, verbose, "flag->get == NULL", &ok);
    assert_log( !strcmp(f->get(f),"abc"), verbose, "flag->get != abc", &ok);

    conter = 0;
    for(i = 0; f->flags[i] != NULL; i++){ conter++; }
    assert_log( conter == 2, verbose, "conter != 2", &ok);

    f->make_fancy(f);

    conter = 0;
    for(i = 0; f->flags[i] != NULL; i++){ conter++; }
    assert_log( conter == 6, verbose, "conter != 6", &ok);

    f->add(f, "def");

    opts = f->get_all(f);
    assert_log( !strcmp(opts[0],"abc"), verbose, "opts[0] != abc", &ok);
    assert_log( !strcmp(opts[1],"def"), verbose, "opts[1] != def", &ok);
    assert_log( opts[2] == NULL, verbose, "opts[2] != NULL", &ok);

    f->free(f);

    return ok;
};
