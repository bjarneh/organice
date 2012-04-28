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


#include <string.h>
#include <stdio.h>
#include "map/hash.h"
#include "map/hash_test.h"
#include "utilz/handy.h"

char * helper(void * v){
	return (char *) v;
};

int test_hash(int verbose){

    int i;
    int ok = 1;

    char ** k;
    char * hash_str;
    struct hash * h = new_hash();

    h->put(h, "key1", "value1");
    h->put(h, "key2", "value2");
    h->put(h, "key3", "value3");
    h->put(h, "key4", "value4");
    
    assert_log(h->has(h,"key1"), verbose, "no key1?", &ok);
    assert_log(h->has(h,"key2"), verbose, "no key2?", &ok);
    assert_log(h->has(h,"key3"), verbose, "no key3?", &ok);
    assert_log(h->has(h,"key4"), verbose, "no key4?", &ok);

    assert_log(!strcmp(h->get(h,"key2"), "value2"),verbose, "no value2?", &ok);

    k = h->keys(h);
    for(i = 0; k[i] != NULL; i++){
        assert_log(h->has(h, k[i]),verbose, "no has?", &ok);
    }

    assert_log(i == 4,verbose, "i != 4?", &ok);

    return ok;
};
