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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str/buffer.h"
#include "str/strvec.h"
#include "lists/vector.h"
#include "str/stringutil.h"



// returns NULL terminated array allocated with 'calloc'
char ** split_str(const char * s, const char * delim){

    int i;
    char * word;
    char ** res = NULL;
    char * copy = strdup(s);
    struct vector * vec = new_vector();

    word = strtok(copy, delim);

    while(word != NULL){
        vec->add(vec, strdup(word));
        word = strtok(NULL, delim);
    }

    free(copy);

    if(vec->len > 0){

        res = calloc(vec->len + 1, sizeof(char *));

        for(i = 0; i < vec->len; i++){
            res[i] = (char *) vec->_[i];
        }
        res[i] = NULL;

    }

    vec->free(vec);

    return res;
};

char * add_str(const char * a, const char * b){

    int i, len_a, len_b, j = 0;

    len_a = strlen(a);
    len_b = strlen(b);

    char * ab = calloc(len_a + len_b + 1, sizeof(char));

    for(i = 0; i < len_a; i++){
        ab[j++] = a[i];
    }

    for(i = 0; i < len_b; i++){
        ab[j++] = b[i];
    }

    ab[j] = '\0';

    return ab;
};

// arr must be NULL terminated
char * join_str(const char ** arr, const char * sep){
	
	int len, i;
	char * res;
	struct buffer * b;

	// not the most fancy error handling
	if( arr == NULL || sep == NULL ){ return NULL; }
	
	len = 0;
	for(i = 0; arr[i] != NULL; i++){ len++; }
	
	if(len == 0){ return NULL; } // not possible perhaps :-)
	if(len == 1){ return strdup(arr[0]); }
	
	b = new_buffer();
	
	for(i = 0; i < len-1; i++){
		b->add(b, arr[i]);
		b->add(b, sep);
	}
	
	b->add(b, arr[len-1]);
	
	res = b->str(b);
	
	b->free(b);
	
	return res;
};

// arr1 arr2 must be NULL terminated
char ** join_arr_str(char ** arr1, char ** arr2){
	
	int i;
	char ** res = NULL;
	struct strvec * v = new_strvec();
	
	// add each element in both arrays to strvec
	if(arr1){ for(i = 0; arr1[i]; i++){ v->add(v, arr1[i]); }}
	if(arr2){ for(i = 0; arr2[i]; i++){ v->add(v, arr2[i]); }}
	
	if(v->len(v) > 0){
		res = v->to_array(v);
		v->free(v);
	}
	
	return res;
};

int starts_with(const char * haystack, const char * needle){

    int len_needle, len_haystack, i;

    len_needle   = strlen(needle);
    len_haystack = strlen(haystack);

    if(len_needle > len_haystack){
        return 0;
    }

    for(i = 0; i < len_needle; i++){
        if(haystack[i] != needle[i]){
            return 0;
        }
    }

    return 1;
};

int ends_with(const char * haystack, const char * needle){

    int len_needle, len_haystack, i, y;
    
    len_needle   = strlen(needle);
    len_haystack = strlen(haystack);

    if( len_needle > len_haystack ){
        return 0;
    }

    i = len_needle - 1;
    y = len_haystack - 1;

    while(i >= 0){
        if(haystack[y--] != needle[i--]){
            return 0;
        }
    }

    return 1;
};

char * upto_char(const char * s, char c){
	
	if(! s){ return NULL; }
	
	int i;
	char * res;
	struct buffer * b = new_buffer();

	for(i = 0; s[i]; i++){
		if( s[i] == c ){
			break;
		}else{
			b->add_char(b, s[i]);
		}
	}
	
	res = b->str(b);
	b->free(b);
	
	return res;
};

int is_white_char(char c){

	int white = 0;
	
	switch( c ){
		case ' ' :
		case '\t':
		case '\n':
		case '\r': white = 1;
	}
	
	return white;
};

int is_whitespace(const char * s){
	int i;
	for(i = 0; s[i]; i++){
		if( ! is_white_char(s[i] ) ){
			return 0;
		}
	}
	return 1;
};

char ** split_white(const char * s){

	if( !s ){ return NULL; }
	
	char ** res = NULL;
	struct strvec * v = new_strvec();
	
	int i = 0;
	while(s[i]){
		
		// skip all white
		while( s[i] && is_white_char(s[i]) ){ i++; }

		// gather all non-white
		struct buffer * b = new_buffer();
		
		while( s[i] && ! is_white_char(s[i]) ){
			b->add_char(b, s[i]); i++;
		}
		if(b->len > 0){	v->add(v, b->str(b)); }
		b->free(b);
	}
	
	if(v->len(v) > 0){ res = v->to_array(v); }
	
	v->free_all(v);
	
	return res;
};

void free_strings(char ** s){

	if( !s ){ return; }
	
    int i;
    for(i = 0; s[i]; i++){
        free(s[i]);
    }

    free(s);
};

char ** copy_strings(char ** s){

	if( !s ){ return NULL; }
	
	int i;
	for(i = 0; s[i]; i++);
	
	char ** cp = calloc(i+1, sizeof(char *));
	
	for(i = 0; s[i]; i++){
		cp[i] = strdup(s[i]);
	}
	
	return cp;
};

char ** add_arr_free(char ** a, char ** b){
	
	char ** res = add_arr(a, b);

	free_strings(a);
	free_strings(b);
	
	return res;
};

char ** add_arr(char ** a, char ** b){
	
	int i;
	char ** res = NULL;
	struct strvec * v = new_strvec();
	
	if(a){
		for(i = 0; a[i]; i++){
			v->add(v, a[i]);
		}
	}
	
	if(b){
		for(i = 0; b[i]; i++){
			v->add(v, b[i]);
		}
	}
	
	if(v->len(v) > 0){
		res = v->to_array(v);
	}
	
	v->free(v);
	
	return res;
};

int eq_str(const char * a, const char * b){
	return strcmp(a, b) == 0;
};

char * slice_str(int from, int to, const char * s){
	int i, j;
	char * tmp = malloc(sizeof(char) * ((to - from) + 1));
	j = 0;
	for(i = from; i < to; i++){
		tmp[j++] = s[i];
	};
	tmp[j] = '\0';
	return tmp;
};

// NOTE: s must be NULL terminated
// NOTE: s is freed it must be memory allocated
char ** shift_strs(char ** s){
	
	int i, len;
	char ** new_strs;
	
	len = 0;
	for(i = 0; s[i]; i++){ len++; }

	new_strs = calloc(len, sizeof(char *));

	for(i = 1; s[i]; i++){
		new_strs[i-1] = strdup(s[i]);
	};
	new_strs[i-1] = NULL;
	
	free_strings(s);

	return new_strs;
};

char * reverse_str(const char * s){
	
	int i;
	size_t s_len = strlen(s);
	char * rev = calloc(s_len + 1, sizeof(char));

	s_len -= 1;
	
	for(i = 0; s[i]; i++){
		rev[s_len - i] = s[i];
	}
	rev[s_len+1] = '\0';

	return rev;
};

size_t rev_strspn(const char * s, const char * accept){

	int i, j;
	size_t count = 0;
	size_t s_len = strlen(s);
	size_t r_len = strlen(accept);
	
	for(i = s_len-1; i >= 0; i--){
		for(j = 0; j < r_len; j++){
			if(s[i] == accept[j]){
				break;
			}
		}
		if(j == r_len){
			break;
		}else{
			count++;
		}
	}
	return count;
};


char * trim_str(const char * s){
	
	return trim_any_str(s, " \t\r\n");
	
};

char * trim_any_str(const char * s, const char * reject){
	
	size_t s_len		= strlen(s);
	size_t reject_start	= strspn(s, reject);
	size_t reject_stop	= rev_strspn(s, reject);
	
	if( (reject_start+reject_stop) > s_len ){// it's all rejected
		return (char *) calloc(1, sizeof(char));
	}
	
	return slice_str(reject_start, (s_len-reject_stop), s);
};

