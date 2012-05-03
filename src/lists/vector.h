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

#pragma once


struct vector {
	
    int             len;
    int             cap;
    void **         _;
    
    void *          (*at) (struct vector *, int pos);
    int             (*add) (struct vector *, void *);
    void *			(*pop) (struct vector *);
    int             (*add_vector) (struct vector *, struct vector *);
    struct vector * (*clone) (struct vector *);
    void **         (*to_array) (struct vector *);
    void            (*clear) (struct vector *);
    void            (*free) (struct vector *);
    void            (*free_all) (struct vector *, void (*fn)(void *));
    
};

struct vector * new_vector(void);
struct vector * new_vector_size(int size);


/**

Example:

  struct vector * v = new_vector();

  v->add(v, "one");
  v->add(v, "two");
  
  struct vector * copy = v->clone(v);

  int i;
  for(i = 0; i < copy->len; i++){
    printf("%s\n", (char*) copy->_[i]); // same as copy->at(copy, i);
  }

  v->clear(v);
  copy->clear(copy);



Functions:

  new_vector(void)    => returns new vector with size 50
  new_vectorSize(int) => returns new vector of desired size


Struct:

  vector  :  vector, i.e. array that increases its size when needed


Vector Variables:

  len  =>  number of elements in vector
  cap  =>  capacity of vector (it will increase as needed but still)


Vector Methods:
 
  at(vector * v, int pos)              => return (void *) element at position
  add(vector * v, void * e)            => add element to vector
  add_vector(vector * v1, vector * v2) => add elements of v2 to v1
  clone(vector * v)                    => return clone of v
  clear(vector * v)                    => clear v, same as v->len = 0
  free(vector *)                       => free allocated space for vector

*/
