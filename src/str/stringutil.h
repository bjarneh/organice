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

#ifndef str_stringutil_h
#define str_stringutil_h

char ** split_str(const char *, const char *);
char ** split_white(const char *);
char *  add_str(const char *, const char *);
char *  join_str(const char **, const char *);
char ** add_arr(char **, char **);
char ** add_arr_free(char **, char **);
char *  upto_char(const char *, char);
int     starts_with(const char *, const char *);
int     ends_with(const char *, const char *);
int 	is_white_char(char);
int 	is_whitespace(const char *);
void    free_strings(char **);
char ** copy_strings(char **);
char **	append_arr(char **, char **);
int		eq_str(const char *, const char *);
char *	slice_str(int, int, const char *);
char **	shift_strs(char **);
char *	trim_str(const char *);
char *	trim_any_str(const char *, const char *);
char *	reverse_str(const char *);
size_t	rev_strspn(const char *, const char *);

#endif
