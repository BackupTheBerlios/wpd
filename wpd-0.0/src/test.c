/*
 * The only purpose of this test.c is to measure the speed of some portion on the wpd code
 * This code is GPL
 */


#include <stdio.h>
#include <sys/time.h>

#include "wpd.h"
#include "wpd_commands.h"

#define N_ITER 50

FILE* output_file;

int main()
{

	
/*
 * struct timeval pre;
	struct timeval post;
	int a,b;
	unsigned long int diff_usec[N_ITER];
	int n = 0;
	char tmp[200] = "";
	
	output_file = stdout;

	for ( n = 0; n < N_ITER; n++ )
	{
		gettimeofday( &pre, 0 ); 
		collect_battery_data( &a, &b );
		//collect_cpu_data( &b );
		//collect_governor_data( tmp );
		gettimeofday( &post, 0 ); 
		diff_usec[n] = post.tv_usec - pre.tv_usec;
		printf("(%d %s) For iter %3d difference is: %lu (%lu)\n", b, tmp, n, post.tv_usec - pre.tv_usec,
				                                     post.tv_sec - pre.tv_sec);
	}
	for ( n = 1; n < N_ITER; n++ )
		diff_usec[0] += diff_usec[n];

	diff_usec[0] /= N_ITER;
	
	printf("Time difference is: %lu \n", diff_usec[0] );
	*/

}

