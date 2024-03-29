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

#pragma once


char *  dir_sep(void);
char    dir_sep_char(void);
char *  path_sep(void);
char    path_sep_char(void);
char *  get_home_env(void);
char *  os_name(void);
int     is_windows(void);
int     run(const char *);
int     os_unlink(const char *);
int     os_rmdir(const char *);

