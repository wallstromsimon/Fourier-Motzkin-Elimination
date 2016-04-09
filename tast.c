#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ROWS (4)
#define COLS (2)

//n rows, m cols

void print_ineq(int n, int m, int matrix[n][m], int v[n])
{
	int i, j;
	for(i = 0; i < n; i++){
		for(j=0; j < m; j++){
			printf("%d    ", matrix[i][j]);
		}
		printf("<=    %d\n", v[i]);
	}
	//printf("\n");
}

int main(int argc, char** argv)
{
	int A[ROWS][COLS] = {{2,-11}, {-3,2}, {1,3}, {-2,0}};
	int c[ROWS] = {3, -5, 4, -3};

	//Start by copying system in real solution?

	printf("Original\n");
	print_ineq(ROWS, COLS, A, c);

	//count positive and negativ
	int n1 = 0; //#positive
	int n2 = 0; //#positive + #negative
	int i;
	for(i = 0; i < ROWS; i++){
		if(A[i][COLS-1] > 0){
			n1++;
		}else if(A[i][COLS-1] < 0){
			n2++;
		}
	}
	n2 += n1;
	printf("\nn1: %d, n2: %d \n", n1, n2);

	//sort system according to rightmost coefficient
	int As[ROWS][COLS];
	int cs[COLS];
	int smallest_row;
	int smallest_value;
	int j;
	//FIXME should have COLS-iter instead of -1, or change cols each iter
	for(i = 0; i < ROWS; i++){
		smallest_row = INT_MAX;
		smallest_value = INT_MAX;
		//Might be possible to sort one value of each category per iteration
		for (j = 0; j < ROWS; j++){
			if(A[j][COLS-1] < smallest_value && A[j][COLS-1] > 0 && i < n1){
				smallest_value = A[j][COLS-1];
				smallest_row = j;
			}else if(A[j][COLS-1] < smallest_value && A[j][COLS-1] < 0 && i >= n1){
				smallest_value = A[j][COLS-1];
				smallest_row = j;
			} else if(A[j][COLS-1] == 0 && i >= n2){
				smallest_value = A[j][COLS-1];
				smallest_row = j;
			}
		}
		for (j = 0; j < COLS; j++){
			As[i][j] = A[smallest_row][j];
			A[smallest_row][j] = INT_MAX;
			cs[i] = c[smallest_row];
			//c[smallest_row] = INT_MAX;
		}
	}
	printf("\nSorted\n");
	print_ineq(ROWS, COLS, As, cs);

	//Divide by coefficient, int div...
	// < should be > when dividing with negative
	int coef;
	for (i = 0; i < ROWS; i++){
		coef = As[i][COLS-1];
		if(coef != 0){
			for(j = 0; j < COLS; j++){
				As[i][j] = As[i][j]/coef;
			}
			cs[i] = cs[i]/coef;
		}

	}
	printf("\nDivided\n"); 
	print_ineq(ROWS, COLS, As, cs);

	//Isolate x2(COLS-1(itr))

}