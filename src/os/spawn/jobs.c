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


// perhaps this should be just pthread.h ? depends on SFU or win32-pthreads

#if ( defined _WIN32 ||  defined WIN32 )
	#include <Pthread.h>
#else
	#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "utilz/handy.h"
#include "os/spawn/jobs.h"



struct job * new_job(void){
	return calloc(1, sizeof(struct job));
};

int spawn_jobs(int len, struct job ** jobs){
	
	pthread_t * threads = calloc(len, sizeof(pthread_t));
	
	int i, fail, has_failed;
	void * retval;

	for(i = 0; i < len; i++){
		
		fail = pthread_create(&threads[i],
						      NULL,
						      jobs[i]->fn,
						      jobs[i]->arg);
		
		if( fail ){
			panic("failed to fork", __FILE__, __LINE__);
		}

	}

	has_failed = 0;
	
	for(i = 0; i < len; i++){
		fail = pthread_join(threads[i], &retval);
		if( fail ){
			panic("failed to join thread", __FILE__, __LINE__);
		}
		int * iret = (int *) retval;
		if(*iret){
			printf("job[%d] failed to execute\n", i);
			has_failed = 1;
		}
		free(iret);
	}
	
	free(threads);

	return has_failed;
};
