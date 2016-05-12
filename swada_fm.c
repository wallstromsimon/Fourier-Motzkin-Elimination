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

rational_t muld(rational_t,rational_t);

rational_t reduce(rational_t r){
	int a=r.n, b=r.d, c;
	while (a != 0) {
		c = a;
		a = b%a;
		b = c;
	}
	b = b < 0 ? b * (-1) : b;
	r.n = r.n/b;
	r.d = r.d/b;
	if(r.d < 0){
		r = muld(r, (rational_t){-1,-1});
	}
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

double rtod(rational_t r1)
{
	return (double)r1.n/r1.d;
}

int lessd(rational_t r1, rational_t r2){

	return rtod(r1) < rtod(r2);
}
int greatd(rational_t r1, rational_t r2){
	return rtod(r1) > rtod(r2);
}
int equald(rational_t r1, rational_t r2){
	return rtod(r1) == rtod(r2);
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
		printf("<=    ");
		print_rational(v[i]);
		printf("\n");
	}
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
	rational_t zero = {.n=0, .d=1};
	rational_t smallest_value;
	for(i = 0; i < rows; i++){
		smallest_row = INT_MAX;
		smallest_value.n = INT_MAX;
		smallest_value.d = 1;

		//Might be possible to sort one value of each category per iteration
		for (j = 0; j < rows; j++){
			rational_t eval = A[j][cols-1];
			if(lessd(eval,smallest_value) && greatd(eval,zero) && i < n1){
				smallest_value = eval;
				smallest_row = j;
			}else if(lessd(eval,smallest_value) && lessd(eval,zero) && i >= n1){
				smallest_value = eval;
				smallest_row = j;
			} else if(equald(eval,zero) && i >= n2){
				smallest_value = eval;
				smallest_row = j;
			}
		}
		for (j = 0; j < cols; j++){
			As[i][j] = A[smallest_row][j];
			A[smallest_row][j].n = INT_MAX;
			A[smallest_row][j].d = 1;			
			cs[i] = c[smallest_row];
			//c[smallest_row] = INT_MAX;
		}
	}
	printf("\nSorted\n");
	for (int i = 0; i < rows; ++i){
		for (int j = 0; j < cols; ++j){
			A[i][j] = As[i][j];
		}
		c[i] = cs[i];
	}
	print_ineq(rows, cols, A, c);
}

//Contineu with rationals here!!!!!!!!!!!!!!!!!!!!!!!!!
void divide_by_coef(int rows, int cols, rational_t A[rows][cols], rational_t c[rows] )
{
	//Divide by coefficient, int div...
	// < should be > when dividing with negative
	rational_t coef;
	int i,j;
	for (i = 0; i < rows; i++){
		coef = A[i][cols-1];
		if(coef.n != 0){
			for(j = 0; j < cols; j++){
				A[i][j] = divd(A[i][j], coef);
			}
			c[i] = divd(c[i], coef);
		}
	}
	printf("\nDivided\n");
	print_ineq(rows, cols, A, c);
}


int fm_elim(int rows, int cols, rational_t a[rows][cols], rational_t c[rows])
{
	sort_ineq(rows,cols, a, c);
	divide_by_coef(rows,cols, a, c);
	/*
	int b[rows][cols];
	int B[rows][cols];

	int n1;
	int n2;

	while(1){
		sort_ineq(rows,cols, a, c);

		divide_by_coef(rows,cols, a, c);


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
	rational_t a[rows][cols];
	for(i = 0; i<rows; ++i){
		for(j = 0; j<cols; ++j){
			if(fscanf(afile,"%d",&(a[i][j].n)) != 1){
				fprintf(stderr, "could not read from file\n");
			}else{
				a[i][j].d = 1;
			}
		}
	}
	if(fscanf(cfile,"%d",&c_col) != 1){
		fprintf(stderr, "could not read from file\n");
	}
	rational_t c[c_col];
	for(i = 0; i <c_col;++i){
		if(fscanf(cfile,"%d",&(c[i].n)) != 1){
			fprintf(stderr, "could not read from file\n");
		}else{
			c[i].d = 1;
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
