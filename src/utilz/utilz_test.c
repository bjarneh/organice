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


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utilz/handy.h"
#include "utilz/timer.h"
#include "utilz/utilz_test.h"


//TODO use SRCROOT variable here
int test_is_dir(int verbose){
    int ok = 1;
    assert_log(is_dir("src/parse"), verbose, "src/parse ! dir", &ok);
    assert_log(!is_dir("src/lskdjf"), verbose, "src/lskdjf dir?", &ok);
    return ok;
};

//TODO use SRCROOT variable here
int test_is_file(int verbose){
    int ok = 1;
    assert_log(is_file("src/start/main.c"), verbose, "main.c not file?", &ok);
    assert_log(!is_file("sldkfj"), verbose, "sldkjf is file?", &ok);
    return ok;
};

int test_timer(int verbose){

    int ok = 1;
    
    struct timespec * ts = calloc(1, sizeof(struct timespec));

    ts->tv_sec  = 0;
    ts->tv_nsec = 5000000;
    
    struct timer * t = new_timer();

    t->start(t, "whatever");
    
    // only if nano sleep works can we work out something here
    if(nanosleep(ts, NULL) == 0){
        t->stop(t, "whatever");
        long millis = t->millis(t, "whatever");
        assert_log(millis > 0 && millis < 10,verbose,"timer ! [0,10]", &ok);
    }
    
    return ok;
    
};

