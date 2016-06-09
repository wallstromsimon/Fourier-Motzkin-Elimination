#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <alloca.h>


static unsigned long long	fm_count;
static volatile bool		proceed = false;

typedef struct rational_t rational_t;
struct rational_t{
	int n;
	int d;
};

inline rational_t subd(rational_t r1, rational_t r2)
{
	return (rational_t){r1.n*r2.d - r2.n*r1.d, r1.d*r2.d};
}
inline rational_t divd(rational_t r1, rational_t r2)
{
	return (rational_t){r1.n*r2.d, r1.d*r2.n};
}

inline double rtod(rational_t r1)
{
	return (double)r1.n/r1.d;
}

static void done(int unused)
{
	proceed = false;
	unused = unused;
}

rational_t sort_ineq(int rows, int cols, rational_t A[rows][cols], rational_t c[rows])
{
	//count positive and negativ
	int n1 = 0; //#positive
	int n2 = 0; //#positive + #negative
	int i, j;
	for(i = 0; i < rows; i++){
		double eval = rtod(A[i][cols-1]);
		if(eval > 0){
			n1++;
		}else if(eval < 0){
			n2++;
		}
	}
	n2 += n1;

	//sort system according to rightmost coefficient
	rational_t (*As)[cols] = alloca(rows*cols*sizeof(rational_t));
	rational_t *cs = alloca(cols*sizeof(rational_t));
	
	int ppos = 0;
	int npos = n1;
	int zpos = n2;
	for(i = 0; i < rows; i++){
		double eval = rtod(A[i][cols-1]);
		if(eval > 0){
			for (j = 0; j < cols; j++){
				As[ppos][j] = A[i][j];
			}
			cs[ppos] = c[i];
			ppos++;
		}else if(eval < 0){
			for (j = 0; j < cols; j++){
				As[npos][j] = A[i][j];
			}
			cs[npos] = c[i];
			npos++;
		}else{
			for (j = 0; j < cols; j++){
				As[zpos][j] = A[i][j];
			}
			cs[zpos] = c[i];
			zpos++;
		}
	}

	for (int i = 0; i < rows; ++i){
		for (int j = 0; j < cols; ++j){
			A[i][j] = As[i][j];
		}
		c[i] = cs[i];
	}
	return (rational_t){n1,n2};
}

void divide_by_coef(int rows, int cols, rational_t A[rows][cols], rational_t c[rows])
{
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
}

void find_sol(rational_t* q, int n1, int n2, rational_t* br, rational_t* Br)
{
	*br = (rational_t){INT_MIN, 1};
	*Br = (rational_t){INT_MAX, 1};

	if (n2 > n1) {
		double max_value = -INT_MAX;
		for (int j = n1; j < n2; ++j) {
			if (rtod(q[j]) > max_value) {
				max_value = rtod(q[j]);
				*br = q[j];
			}
		}
	}

	if (n1 > 0) {
		double min_value = INT_MAX;
		for (int j = 0; j < n1; ++j) {
			if (rtod(q[j]) < min_value) {
				min_value = rtod(q[j]);
				*Br = q[j];
			}
		}
	}
}

int get_solution(int s, rational_t c[s], int n2, rational_t b, rational_t B) 
{
	if (rtod(b) > rtod(B))
		return false;

	for (int j = n2; j < s; ++j)
		if (rtod(c[j]) < 0)
			return false;
	return true;
}

int fm_elim(int rows, int cols, rational_t a[rows][cols], rational_t c[rows])
{
	int n1;
	int n2;
	int s = rows;
	int r = cols;
	rational_t br;
	rational_t Br;

	rational_t *q = c;
	void *next_matrix_ptr = a;

	while(1){
		rational_t (*T)[r] = next_matrix_ptr;
		rational_t n = sort_ineq(s,r, T, q);
		n1 = n.n;
		n2 = n.d;
		divide_by_coef(n2,r, T, q);

		if(r == 1){
			find_sol(q, n1, n2, &br, &Br);
			return get_solution(s, q, n2, br, Br);
		}

		int s_prime = s - n2 + n1*(n2 - n1);
		if (s_prime == 0)
			return true;

		rational_t (*U)[r-1] = alloca(s_prime * (r-1) * sizeof(rational_t));
		rational_t *q_next = alloca(s_prime * sizeof(rational_t));
		int current_row = 0;
		
		for (int k = 0; k < n1; k++) {
			for (int l = n1; l < n2; l++) {
				for (int i = 0; i < r-1; i++) {
					U[current_row][i] = subd(T[k][i], T[l][i]);
				}
				q_next[current_row++] = subd(q[k], q[l]);
			}
		}

		for (int j = n2; j < s; j++) {
			for (int i = 0; i < r-1; i++) {
				U[current_row][i] = T[j][i];
			}
			q_next[current_row++] = q[j];
		}

		next_matrix_ptr = (void *)U;
		q = q_next;
		r--;
		s = s_prime;
	}
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
	rational_t (*a)[cols] = alloca(rows*cols*sizeof(rational_t));
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
	rational_t *c = alloca(c_col*sizeof(rational_t));
	for(i = 0; i <c_col;++i){
		if(fscanf(cfile,"%d",&(c[i].n)) != 1){
			fprintf(stderr, "could not read from file\n");
		}else{
			c[i].d = 1;
		}
	}

	fclose(afile);
	fclose(cfile);
	if (seconds == 0) {
		/* Just run once for validation. */

		// Uncomment when your function and variables exist...
		return fm_elim(rows, cols, a, c);
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
