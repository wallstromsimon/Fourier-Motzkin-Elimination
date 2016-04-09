#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>

static unsigned long long	fm_count;
static volatile bool		proceed = false;

ypedef struct rational_t rational_t;
struct rational_t{
	int p;
	int q;
};

void print_rational(rational_t r){
	printf("%d/%d\n",r.p ,r.q);
}

rational_t reduce(rational_t r){
	int a=r.p, b=r.q, c;
 	while (a != 0) {
  		c = a;
		a = b%a;
		b = c;
 	}
  	r.p = r.p/b;
	r.q = r.q/b;
	return r;
}

rational_t addq(rational_t r1, rational_t r2){
	rational_t r;
  	r.p = r1.p*r2.q + r2.p*r1.q;
	r.q = r1.q*r2.q;
	return reduce(r);
}
rational_t subq(rational_t r1, rational_t r2){
	rational_t r;
  	r.p = r1.p*r2.q - r2.p*r1.q;
	r.q = r1.q*r2.q;
	return reduce(r);
}
rational_t mulq(rational_t r1, rational_t r2){
	rational_t r;
  	r.p = r1.p*r2.p;
	r.q = r1.q*r2.q;
	return reduce(r);
}
rational_t divq(rational_t r1, rational_t r2){
	rational_t r;
  	r.p = r1.p*r2.q;
	r.q = r1.q*r2.p;
	return reduce(r);
}

static void done(int unused)
{
	proceed = false;
	unused = unused;
}
int fm_elim(int rows, int cols, int a[rows][cols], int c[rows])
{
	int b[rows][cols];
	int B[rows][cols];

	int n1;
	int n2;

	while(1){
		sort_ineq(a, c, &n1, &n2);

		for (int i = 0; i < r; ++i){
			for (int j = 0; i < n2; ++j){
				T[i][j] = T[i][j] / T[i][m-1];
			}
		}

		for (int j = 0; i < n2; ++j){
			q[j] = q[j]/T[i][m-1];
		}

		if (n2 > n1){
			//max
		}else{
			//-inf
		}
		if(n1 > 0){
			//min
		}else{
			//inf
		}

		if(cols == 1){
			//return make solution
		}


	}


	return 1;
}
	
unsigned long long swada_fm(char* aname, char* cname, int seconds)
{
	FILE*		afile = fopen(aname, "r");
	FILE*		cfile = fopen(cname, "r");

	fm_count = 0;

	if (afile == NULL) {
		fprintf(stderr, "could not open file A\n");
		exit(1);
	}

	if (cfile == NULL) {
		fprintf(stderr, "could not open file c\n");
		exit(1);
	}
	int rows,cols,i,j,c_col;
	fscanf(afile,"%d",&rows);
	fscanf(afile,"%d",&cols);
	int a[rows][cols];
	for(i = 0; i<rows; ++i){
		for(j = 0; j<cols; ++j){
			fscanf(afile,"%d",&a[i][j]);
		}
	}
	fscanf(cfile,"%d",&c_col);
	int c[c_col];
	for(i = 0; i <c_col;++i)
			fscanf(cfile,"%d",&c[i]);
	/* read A and c files. */

	fclose(afile);
	fclose(cfile);

	if (seconds == 0) {
		/* Just run once for validation. */
			
		// Uncomment when your function and variables exist...
		return fm_elim(rows, cols, a, c);
		// return 1; // return one, i.e. has a solution for now...
	}

	/* Tell operating system to call function DONE when an ALARM comes. */
	signal(SIGALRM, done);
	alarm(seconds);

	/* Now loop until the alarm comes... */
	proceed = true;
	while (proceed) {
		// Uncomment when your function and variables exist...
		fm_elim(rows, cols, a, c);

		fm_count++;
	}

	return fm_count;
}