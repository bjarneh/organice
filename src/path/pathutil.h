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


#ifndef path_pathutil_h
#define path_pathutil_h

char ** path_split(const char *);
char *  path_join(const char **);
char *  path_join_len(const char **, int);
char *  path_clean(const char *);
char *	path_from_slash(char *);

#endif
