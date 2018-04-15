/*
 This file is part of EASAL. 

 EASAL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 EASAL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef THREAD_H_
#define THREAD_H_


#ifdef WIN32
#include <windows.h>
    #include <pthread/pthread.h>
#else
    #include <pthread.h>
#endif

#include <iostream>


/*
 * This class is used as a basis for the threads.
 * Allowing a simplified approach to starting and waiting on threads.
 */
class Thread{
    pthread_t thread;
    static void *thread_func(void *d)
    {
        ((Thread *)d)->run();
        return NULL;
    }
public:
	//Default Constructor
    Thread(){}
    //Destructor
    virtual ~Thread(){}

	//The run function to start the thread
    virtual void run(){}
    //The start function
    int start(){
        pthread_attr_t attr;
        size_t stacksize;
        pthread_attr_init(&attr);
        pthread_attr_getstacksize(&attr, &stacksize);
        std::cout << "Thread (base): Stacksize = " << stacksize << std::endl;

        size_t tmp_size=50*stacksize;
        pthread_attr_setstacksize(&attr, tmp_size);
        pthread_attr_getstacksize(&attr, &stacksize);
        std::cout << "Thread (base): Stacksize = " << stacksize << std::endl;
        return pthread_create(&thread, &attr, Thread::thread_func, (void*)this);

    }

	//The wait function
    int wait(){
      return pthread_join(thread, NULL);
    }
};

#endif
