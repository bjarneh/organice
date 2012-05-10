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


#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "map/hash.h"
#include "utilz/timer.h"
#include "utilz/handy.h"
#include "str/stringutil.h"

// timer methods
static char * timer_str(struct timer *, const char *);
static void   timer_start(struct timer *, const char *);
static void   timer_stop(struct timer *, const char *);
static long   timer_millis(struct timer *, const char *);
static double timer_seconds(struct timer *, const char *);
static void   timer_free(struct timer *);

// private functions
static long   to_millis(struct timeval tv);


struct timer * new_timer(void){

    struct timer * t = malloc(sizeof(struct timer));
    
    t->h        = new_hash();
    t->str      = &timer_str;
    t->start    = &timer_start;
    t->stop     = &timer_stop;
    t->millis   = &timer_millis;
    t->seconds  = &timer_seconds;
    t->free     = &timer_free;
    
    return t;
};

static long timer_millis(struct timer * slf, const char * id){
    
    long * interval;
    interval = (long *) slf->h->get( slf->h, id );

    if(! interval){
        char * msg = add_str("utilz/timer: unknown job:", id);
        panic(msg, __FILE__, __LINE__);
    }
    
    return (*interval);
};

static double timer_seconds(struct timer * slf, const char * id){
    long milli_secs = slf->millis(slf, id);
    return (double)milli_secs / 1000;
};

static char * timer_str(struct timer * slf, const char * id){
    // TODO FIXME
    return NULL;
};

static void timer_start(struct timer * slf, const char * id){
    struct timeval * tv = calloc(1, sizeof(struct timeval));
    if( gettimeofday(tv, NULL) != 0 ){
        panic("failed to gettimeofday", __FILE__, __LINE__);
    }
    slf->h->put( slf->h, id, (void *) tv );
};

static void timer_stop(struct timer * slf, const char * id){

    struct timeval * tv;
    tv = (struct timeval *) slf->h->get( slf->h, id );

    if(! tv){
        char * msg = add_str("stopping job not started:", id);
        panic(msg, __FILE__, __LINE__);
    }

    struct timeval tv2;
    if( gettimeofday(&tv2, NULL) != 0 ){
        panic("failed to gettimeofday", __FILE__, __LINE__);
    }
    
    long millis_start = to_millis( *tv );
    long millis_stop  = to_millis( tv2 );
    
    long * interval   = malloc(sizeof(long));
    (*interval)       = millis_stop - millis_start;
    
    slf->h->put( slf->h, id, (void *) interval);
    
    free(tv);
};

static void timer_free(struct timer * slf){
    slf->h->free_all( slf->h, &free );
    free(slf);
};

static long to_millis(struct timeval tv){
    long ret = 0;
    ret     += tv.tv_sec * 1000;
    ret     += tv.tv_usec / 1000;
    return ret;
};
