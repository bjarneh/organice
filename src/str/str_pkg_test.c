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


/// #include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "utilz/handy.h"
#include "str/buffer.h"
#include "str/strvec.h"
#include "str/strset.h"
#include "str/stringutil.h"
#include "str/str_pkg_test.h"



int test_buffer(int verbose){
    
    int i;
    int ok = 1;

    struct buffer * b = new_buffer_size(3);

    b->add(b, "one");
    assert_log(! strcmp(b->str(b), "one"),verbose,"buf != 'one'",&ok);

    b->add(b, "two");
    assert_log(! strcmp(b->str(b), "onetwo"),verbose,"buf != 'onetwo'",&ok);

    b->add_char(b, '1');
    assert_log(! strcmp(b->str(b), "onetwo1"),verbose,"buf != 'onetwo1'",&ok);

    return ok;
};


int test_strset(int verbose){

	int ok = 1;

	struct strset * s = new_strset();

	s->add(s, "one");
	s->add(s, "two");
	s->add(s, "three");

	assert_log(s->has(s, "one"), verbose, "! strset has 'one'", &ok);
	assert_log(!s->has(s, "ab"), verbose, "strset has 'ab'", &ok);
	
	return ok;
};

int test_split_str(int verbose){

    int ok = 1;

    char ** sp = split_str("one two  !", " ");

    assert_log(!strcmp(sp[0], "one"), verbose, "chunk should be: one", &ok);
    assert_log(!strcmp(sp[1], "two"), verbose, "chunk should be: two", &ok);
    assert_log(!strcmp(sp[2], "!"), verbose, "chunk should be: !", &ok);
    assert_log( sp[3] == NULL , verbose, "chunk should be: NULL", &ok);

    return ok;
};

int test_starts_with(int verbose){

    int ok = 1;

    assert_log(starts_with("whatever", "what"), verbose, "starts_with!", &ok);
    assert_log(!starts_with("whatever", "whzt"), verbose, "starts_with", &ok);

    return ok;
};

int test_ends_with(int verbose){

    int ok = 1;

    assert_log(ends_with("whatever", "ever"), verbose, "ends_with!", &ok);
    assert_log(!ends_with("some","!me"), verbose, "ends_with", &ok);
	assert_log(ends_with("some/file.o", ".o"), verbose, "ends_with .o", &ok);
	
    return ok;
};

int test_add_str(int verbose){

    int ok = 1;

    char * j = add_str("one", "two");

    assert_log(!strcmp(j, "onetwo"), verbose, "add_str != onetwo", &ok);

    return ok;
};

int test_join_str(int verbose){
	int ok = 1;
	const char * tmp[4] = {"en", "to", "tre", NULL};
	char * j    = join_str(tmp, "/");
	assert_log(!strcmp("en/to/tre", j), verbose, "join_str != en/to/tre",&ok);
	return ok;
};

int test_slice_str(int verbose){
	int ok = 1;
	char * tmp = "0123456789";
	assert_log(!strcmp("01",slice_str(0,2,tmp)),verbose,"slice ! '01'",&ok);
	assert_log(!strcmp("23",slice_str(2,4,tmp)),verbose,"slice ! '34'",&ok);
	return ok;
};

int test_reverse_str(int verbose){
	int ok = 1;
	char * tmp = "0123";
	char * exp = "3210";
	char * rev = reverse_str(tmp);
	assert_log(!strcmp(exp, rev), verbose, "reverse not working", &ok);
	return ok;
};

int test_rev_strspn(int verbose){

	int ok = 1;

	char * tmp = "efgh ijk  \t \n";
	size_t exp = 5;
	size_t res = rev_strspn(tmp, " \t\n");
	assert_log(res == exp, verbose, "rev_strspn not working", &ok);
	
	return ok;
};

int test_trim_str(int verbose){

	int ok = 1;
	
	char * tmp = " a \t";
	char * res = trim_str(tmp);

	assert_log(!strcmp("a", res), verbose, "trim 1 !ok", &ok);

	tmp = "ab \t\n";
	res = trim_str(tmp);
	assert_log(!strcmp("ab", res), verbose, "trim 2 !ok", &ok);
	
	tmp = "\t \r   \t \t\n";
	res = trim_str(tmp);
	assert_log(!strcmp("", res), verbose, "trim 3 !ok", &ok);
	
	return ok;
	
};


int test_trim_any_str(int verbose){
	
	int ok = 1;
	
	char * tmp = " a \t";
	char * res = trim_any_str(tmp, " ");

	assert_log(!strcmp("a \t", res), verbose, "trim_any 1 !ok", &ok);

	tmp = "ab 45";
	res = trim_any_str(tmp, "a45");
	assert_log(!strcmp("b ", res), verbose, "trim_any 2 !ok", &ok);
	
	return ok;
	
};

int test_strvec(int verbose){
	
	int ok = 1;
	
	struct strvec * sv = new_strvec();
	
	sv->add(sv, "a");
	sv->add(sv, "b");
	sv->add(sv, "c");
	
	char * joined = sv->join(sv, "-");
	assert_log(!strcmp("a-b-c", joined), verbose, "!strvec->join", &ok);
	joined = sv->str(sv);
	assert_log(!strcmp("abc", joined), verbose, "!strvec->str", &ok);
	
	return ok;
};

int test_upto_char(int verbose){

	int ok = 1;
	
	char * line = "this is a line # this is a comment";
	char * rm   = upto_char(line, '#');
	
	assert_log(!strcmp(rm, "this is a line "), verbose, "! upto_char", &ok);
	
	line = "this is a line # this is a comment";
	rm   = upto_char(line, '-');

	assert_log(!strcmp(rm, line), verbose, "! upto_char 2", &ok);
	
	return ok;
};

int test_split_white(int verbose){

	int ok = 1;
	
	char * line = "this is  \n a \t line";
	char ** sp  = split_white(line);
	
	assert_log(!strcmp(sp[0], "this"), verbose, "sp[0] != 'this'", &ok);
	assert_log(!strcmp(sp[1], "is"), verbose, "sp[1] != 'is'", &ok);
	assert_log(!strcmp(sp[2], "a"), verbose, "sp[2] != 'a'", &ok);
	assert_log(!strcmp(sp[3], "line"), verbose, "sp[3] != 'line'", &ok);

	line = " \n \t ";
	sp   = split_white(line);
	
	assert_log( sp == NULL, verbose, "sp != NULL", &ok);
	
	return ok;
};

int test_add_arr(int verbose){

	int ok = 1;
	
	char * a[] = {"1", "2", NULL};
	char * b[] = {"3", "4", NULL};
	char ** c;
	
	c = add_arr(a, b);
	
	assert_log(!strcmp(c[0], "1"), verbose, "c[0] != '1'", &ok);
	assert_log(!strcmp(c[1], "2"), verbose, "c[1] != '2'", &ok);
	assert_log(!strcmp(c[2], "3"), verbose, "c[2] != '3'", &ok);
	assert_log(!strcmp(c[3], "4"), verbose, "c[3] != '4'", &ok);
	
	return ok;
};

