#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>

static unsigned long long	fm_count;
static volatile bool		proceed = false;

typedef struct rational_t rational_t;
	struct rational_t{
        int n;
        int d;
};

void print_rational(rational_t r){
        printf("%d/%d",r.n ,r.d);
}

rational_t reduce(rational_t r){
        int a=r.n, b=r.d, c;
        while (a != 0) {
                c = a;
                a = b%a;
                b = c;
        }
        r.n = r.n/b;
        r.d = r.d/b;
        return r;
}

rational_t addd(rational_t r1, rational_t r2){
        rational_t r;
        r.n = r1.n*r2.d + r2.n*r1.d;
        r.d = r1.d*r2.d;
        return reduce(r);
}
rational_t subd(rational_t r1, rational_t r2){
        rational_t r;
        r.n = r1.n*r2.d - r2.n*r1.d;
        r.d = r1.d*r2.d;
        return reduce(r);
}
rational_t muld(rational_t r1, rational_t r2){
        rational_t r;
        r.n = r1.n*r2.n;
        r.d = r1.d*r2.d;
        return reduce(r);
}
rational_t divd(rational_t r1, rational_t r2){
        rational_t r;
        r.n = r1.n*r2.d;
        r.d = r1.d*r2.n;
        return reduce(r);
}

static void done(int unused)
{
	proceed = false;
	unused = unused;
}

void print_ineq(int n, int m, rational_t matrix[n][m], rational_t v[n])
{
        int i, j;
        for(i = 0; i < n; i++){
                for(j=0; j < m; j++){
			print_rational(matrix[i][j]);
                        printf("    ");
                }
                printf("<=    \n");
		print_rational(v[i]);
        }
        //printf("\n");
}

void sort_ineq(int rows, int cols, rational_t A[rows][cols], rational_t c[rows] )
{
	//count positive and negativ
	int n1 = 0; //#positive
	int n2 = 0; //#positive + #negative
	int i, j;
	for(i = 0; i < rows; i++){
		double eval = (double)A[i][cols-1].n / A[i][cols-1].d;
		if(eval > 0){
			n1++;
		}else if(eval < 0){
			n2++;
		}
	}
	n2 += n1;
	printf("\nn1: %d, n2: %d \n", n1, n2);

	//sort system according to rightmost coefficient
	rational_t As[rows][cols];
	rational_t cs[cols];
	int smallest_row;
	rational_t smallest_value;
	//FIXME should have cols-iter instead of -1, or change cols each iter
	for(i = 0; i < rows; i++){
		smallest_row = INT_MAX;
		smallest_value = INT_MAX;
		//Might be possible to sort one value of each category per iteration
		for (j = 0; j < rows; j++){
			double eval = (double)A[i][cols-1].n / A[i][cols-1].d;
			if(eval < smallest_value && eval > 0 && i < n1){
				smallest_value = eval;
				smallest_row = j;
			}else if(eval < smallest_value && eval < 0 && i >= n1){
				smallest_value = eval;
				smallest_row = j;
			} else if(eval == 0 && i >= n2){
				smallest_value = eval;
				smallest_row = j;
			}
		}
		for (j = 0; j < cols; j++){
			As[i][j] = A[smallest_row][j];
			A[smallest_row][j] = INT_MAX;
			cs[i] = c[smallest_row];
			//c[smallest_row] = INT_MAX;
		}
	}
	printf("\nSorted\n");
	A = As;
	c = cs;
	print_ineq(rows, cols, A, c);
}

//Contineu with rationals here!!!!!!!!!!!!!!!!!!!!!!!!!
void divide_by_coef(int rows, int cols, rational_t A[rows][cols], rational_t c[rows] )
{
	//Divide by coefficient, int div...
	// < should be > when dividing with negative
	int coef;
	int i,j;
	for (i = 0; i < rows; i++){
		coef = A[i][cols-1];
		if(coef != 0){
			for(j = 0; j < cols; j++){
				A[i][j] = A[i][j]/coef;
			}
			c[i] = c[i]/coef;
		}
	}
	printf("\nDivided\n");
	print_ineq(rows, cols, A, c);
	//Isolate x2(cols-1(itr))
}


int fm_elim(int rows, int cols, int a[rows][cols], int c[rows])
{
	sort_ineq(rows,cols, a, c);
	divide_by_coef(rows,cols, a, c);
	/*int b[rows][cols];
	int B[rows][cols];

	int n1;
	int n2;

	while(1){
		sort_ineq(rows,cols, a, c);

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


	}*/


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
	if(fscanf(afile,"%d",&rows) != 1 || fscanf(afile,"%d",&cols) != 1){
		fprintf(stderr, "could not read from file\n");
	}
	int a[rows][cols];
	for(i = 0; i<rows; ++i){
		for(j = 0; j<cols; ++j){
			if(fscanf(afile,"%d",&a[i][j]) != 1){
				fprintf(stderr, "could not read from file\n");
			}
		}
	}
	if(fscanf(cfile,"%d",&c_col) != 1){
		fprintf(stderr, "could not read from file\n");
	}
	int c[c_col];
	for(i = 0; i <c_col;++i){
		if(fscanf(cfile,"%d",&c[i]) != 1){
			fprintf(stderr, "could not read from file\n");
		}
	}

	fclose(afile);
	fclose(cfile);

	printf("\n\n\n\n------------------\n\n\n\nStarting new test of size %d x %d\n", rows,cols);
	print_ineq(rows,cols,a,c);

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
