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

#if ( defined _WIN32 ||  defined WIN32 )
    #include <windows.h>
#else
    #include <stdlib.h>
#endif


#include "os/specific.h"

//     Windows
#if ( defined _WIN32 ||  defined WIN32 )
char * my_dir_sep           = "\\";
char   my_dir_sep_char      = '\\';
char * my_path_sep          = ";";
char   my_path_sep_char     = ';';
char * my_os_name           = "windows";
int    my_is_windows        = 1;
char * my_home_env          = "USERPROFILE";
#else  // Linux/Unix
char * my_dir_sep           = "/";
char   my_dir_sep_char      = '/';
char * my_path_sep          = ":";
char   my_path_sep_char     = ':';
char * my_os_name           = "*nix";
int    my_is_windows        = 0;
char * my_home_env          = "HOME";
#endif

char * dir_sep(void){ return my_dir_sep; }
char   dir_sep_char(void){ return my_dir_sep_char; }
char * path_sep(void){ return my_path_sep; }
char   path_sep_char(void){ return my_path_sep_char; }
char * os_name(void);
char * get_home_env(void){ return my_home_env; }

int is_windows(void){
    return my_is_windows;
};

int run(const char * cmd){
    return system(cmd);
};
