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


#ifndef utilz_global_h
#define utilz_global_h

void	global_init(void);
char *	global_get_str(const char *);
void	global_set_str(const char *, const char *);
void	global_set_str_nodup(const char *, char *);
void	global_set_str_blank(const char *);
int		global_get_bool(const char *);
void	global_set_bool(const char *, int);
void	global_free(void);

#endif
