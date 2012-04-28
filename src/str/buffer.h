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


#ifndef str_buffer_h
#define str_buffer_h

struct buffer {
	
    int     cap;
    int     len;
    char *  _;
    
    void    (*add)(struct buffer *, const char *);
    void    (*add_len)(struct buffer *, int, const char *);
    void	(*add_char)(struct buffer *, char);
    void    (*clear)(struct buffer *); 
    char *  (*str)(struct buffer *);
    void    (*free)(struct buffer *);
    
};

struct buffer * new_buffer(void);
struct buffer * new_buffer_size(int);

#endif
