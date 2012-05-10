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

#pragma once

#include <time.h>

int     is_dir(const char *);
int     is_file(const char *);
int     assert_log(int, int, const char *, int *);
void    panic(const char *, const char *, int);
time_t  modtime(const char *);
char *  slurp(const char *, long *);
char *  slurp_str(const char *);
char ** slurp_lines(const char *);
void    spit_str(const char *, const char *);
void    spit_bytes(const char *, const char *, size_t);
char ** config2args(const char *);
void    mkdir_or_die(const char *);

