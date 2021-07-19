/*
 * Ortega Vallejo Raúl Antonio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Maths
#include <math.h>
// Typeof data for the matrix.
#define DATA long double 
#define VECTOR DATA*

typedef struct T_MATRIX{ 
	// Total of matrix...
	struct T_MATRIX *count;
	//  Simulate 2D Matrix
	DATA *store ; // VECTOR
	// Columns, Rows
	int C,R;
}* MATRIX; // Ptr to my matrix

// List of Matrix... START-END
MATRIX history = NULL;
MATRIX history_end = NULL;

// Verify path file
void chekRootFile (FILE *ptr) 
{
	if (!ptr) {
		perror("Error");
		exit(-1);
	}
}
void print_matrix (MATRIX matrix)
{
	if (!matrix) {
		return; }
	int rows,columns;
	for (rows = 0; rows < matrix->R; rows++){
		// Printing to terminal...
		for (columns = 0; columns < matrix->C; columns++){
			printf("%Lf ",matrix->store[rows * matrix->C + columns]);
			}
		printf("\n");
		}
}

DATA media (VECTOR v,int size)
{
	int i;
	DATA m = 0;
	for (i= 0; i<size; i++) {
		m +=v[i];	
		}
	return m/size;
}

void plot_matrix (MATRIX matrix)
{
	// Using gnuplot ... 
	// using persistent for a consitent ploting ?
	FILE *gnuplot = popen("gnuplot -persistent","w");
	chekRootFile (gnuplot);
	int rows, columns;
	// command for ploting
	fprintf(gnuplot, "plot '-' \n");
	for (rows = 0; rows < matrix->R; rows++){
		// Printing to terminal...
		for (columns = 0; columns < matrix->C; columns++){
			fprintf(gnuplot,"%Le ",matrix->store[(rows*matrix->C)+columns]);
			}
		fprintf(gnuplot,"\n");
		}
	// End Gnuplot
	fprintf(gnuplot, "e\n");
	pclose(gnuplot);
	gnuplot = NULL;
}

void delete_all_matrix (void)
{
	MATRIX aux = NULL;
	int i=0;
	while (history) {
		aux  = history->count;
		free(&*history);
		history = aux;
		i++;
		}
	aux = history_end = history = NULL;
	//printf("TOTAL MATRIXS GENERATED %d\n",i);
}

// Verify the memory ---
// Memory for the a VECTOR
void checkMem (VECTOR ptr)
{
	if (!ptr) {
		perror("Memory error");
		delete_all_matrix();
		exit(-1);
	}
}
void checkMemMatrix (MATRIX ptr)
{
	if (!ptr) {
		perror("Memory error");
		delete_all_matrix();
		exit(-1);
	}
}
void checkMemoryString(char *ptr)
{
	if (!ptr) {
		perror("Memory error");
		delete_all_matrix();
		exit(-1);
	}
}

MATRIX createMatrix(int nRows,int nColumns)
{
	MATRIX matrix = malloc(sizeof(MATRIX));
	checkMemMatrix(matrix);
	matrix->R = nRows;
	matrix->C = nColumns;
	matrix->store = malloc ((matrix->R * matrix->C) * sizeof(DATA));
	checkMem(matrix->store);
	matrix->count = NULL;
	if(!history) {
		history_end = history = matrix;
		}
	else {
		history_end->count = matrix;
		history_end = history_end->count;
	}
	
	return matrix;
}

void print_matrix_transpose(MATRIX matrix)
{	
	if (!matrix) {
		return;
	}
	int i,j;
	for (i = 0; i < matrix->R; i++) {
		for (j = 0; j < matrix->C; j++) {
			printf("%Le ",matrix->store[(i * matrix->R)+j]);

		}
		printf("\n");
	}
}
// load matrix (suppose the file is formated!)
MATRIX load(char *f,int nRows,int nColumns)
{
	FILE *file = fopen(f,"r");
	chekRootFile (file);
        fseek(file,0,SEEK_END);
        size_t pos = ftell(file);
        char *c = malloc(sizeof(char*)*pos+1);
	checkMemoryString(c);
        rewind(file);
        fread (c,pos ,1,file);
        fclose(file);
	file = NULL;
        c[pos] = '\0';
	// exprg for avoid Tabulator and Return's
	char expr[10] = "[ ^\t|\n]";
	char *token = strtok (c,expr);
	MATRIX matrix = createMatrix(nRows,nColumns);
	int rows, columns;
	for(rows=0;rows < nRows && token!=NULL; rows++) { 
		for (columns=0; columns < nColumns; columns++) {
			sscanf(token, "%Le",&matrix->store[rows * nColumns +columns]);
			token = strtok (NULL,expr);
		}
	}
	free(c);
	token = c  = NULL;

	return matrix;
}

// Create the transpose of a matrix...
MATRIX matrix_transpose (MATRIX ptr_matrix)
{
	if (!ptr_matrix) return NULL;
        MATRIX matrix = createMatrix(ptr_matrix->C,ptr_matrix->R);
	int rows, columns;
	for (rows = 0; rows < matrix->R; rows++) {
		for (columns = 0; columns < matrix->C; columns++) {
			matrix->store[(columns * matrix->R)+rows] = ptr_matrix->store[(rows * matrix->C) + columns];
		}
	}	
	return matrix;
}


MATRIX cut_matrix (// CUT or COPY matrix with specific dimensions...
		MATRIX matrix, // Matrix
		unsigned rows, // Initial row
		unsigned endR, // End row
		unsigned columns, // Initial column
		unsigned endC // End colums
		)
{
	rows--; columns--; // 
	endR  -= rows;
	endC  -= columns;
	// Verify dimensions
	if (endR > matrix->R || endC > matrix->C){
		printf("Error dimentions\n");
		delete_all_matrix();
		return NULL;
		}
	MATRIX cut_m = createMatrix(endR,endC);
	checkMemMatrix(cut_m);
	int aux = columns;
	int r,c;
	for (r=0; r<cut_m->R && rows<matrix->R; r++,rows++){
		for (c=0, columns=aux; c<cut_m->C && columns<matrix->C; c++,columns++){
			cut_m->store[r * cut_m->C + c] =  matrix->store[rows * matrix->C + columns];
			}
		}
	return cut_m;

}

DATA get_data (MATRIX a,int R, int C) 
{
	int i,j;
	DATA d = 0;
	for (i=0; i<R && i<a->R; i++) {
		for (j=0;  j<C && j<a->C; j++) {
			d = a->store[i * a->C + j];		
			}

	}	
	return d;
}

DATA distance_euclidean (VECTOR a, VECTOR b,int Size)
{
	int i;
	DATA v,distance = 0;
	for (i=0; i<Size; i++) {
		v = a[i] - b[i];
		distance += pow(v,2);
		}
	return sqrt(distance);
}

void print_vector (VECTOR x,int size)
{
	int i;
	for (i=0; i<size; i++)
		printf("%Lf\n",x[i]);
}

int min (VECTOR v,int size)
{
	DATA m,n;
	m = v[0];
	int i,j=0;
	for(i=1; i<size; i++){
		n = v[i];
		if (n < m){
			m = n;
			j=i;
			}
		}
	return j;}

// Returns 0 or 1 if are equasl two vectors (directions)
int equals_vector (VECTOR a, VECTOR b,int size)
{
	int i;
	if (!a || !b) {
		return 0;
		}
	for (i= 0; i<size; i++) {
		if (a[i] != b[i]) {
		       	return 0;
			}
		}	       
	return 1;
}

// Returns 0 or 1 if are equasl two MATRIX
int equals_matrix (MATRIX a, MATRIX b)
{
	int i,j;
	if ((!a && !a->store) || (!b && !b->store) || a->R!=b->R || a->C!=b->C) {
	       	return 0;
		}
	for (i= 0; i<a->R; i++) {
		for (j=0; j<a->C; j++) { 
			if (a->store[i * a->C +j] != b->store[i * b->C +j]) { 
		       		return 0;
				}
			}
		}	

	return 1;
}

MATRIX assing_clouster (MATRIX n) 
{
	MATRIX m = createMatrix(n->R,1);
	VECTOR line = createMatrix(1,n->C)->store;
	int i,j;
	for (i=0; i < n->R; i++) {
		for (j=0; j < n->C; j++) {
			line[j] = n->store[i *n->C + j];
			}
		// ASSIGNMENT proces (minimo values) 
		 m->store[i] = min(line,n->C); // Assing to clouster from line
		 //m->store[i] = line[min(line,n->C)];
		}
	return m;
}

MATRIX get_value_assign (MATRIX n) 
{
	MATRIX m = createMatrix(n->R,1);
	// First line from reading of matrix ... (VECTOR)
	VECTOR line = createMatrix(1,n->C)->store;
	int i,j;
	for (i=0; i < n->R; i++) {
		for (j=0; j < n->C; j++) {
			line[j] = n->store[i *n->C + j];
			}
		// ASSIGNMENT proces (minimo values) 
		 //m->store[i] = min(line,n->C); // Assing to clouster from line
		 m->store[i] = line[min(line,n->C)];
		}
	return m;
}

MATRIX update_clousters (MATRIX c,MATRIX distances) 
{
	int numClousters = c->R;
	
	MATRIX matrix_assignation = assing_clouster(distances);
	MATRIX new_clousters = createMatrix(numClousters,c->C);

	VECTOR assignation = matrix_assignation->store;
	VECTOR values = get_value_assign(distances)->store;

	/************ DEBBUGIN 
	printf("Clousters Total: %d\n",numClousters);
	printf("\n\tDistances\n");
	print_matrix(distances);  
	printf("\n\tMinimo\n");
	print_vector(values,distances->R); 
	printf("\n");
	printf("\n\tINDEX Assignation\n");
	print_vector(assignation,distances->R); 
	printf("\n"); 
 	***************************************/

	int i,j,k,l,
	assign =-1; // Variable for validate the assignation 

	DATA sum, // Sumatoria (DEBUGGIN)
	     r_media; // Result from media values;
	
	VECTOR *test = malloc(sizeof(VECTOR*)*numClousters);
	checkMem((VECTOR)test);

	for (i= k = sum =0;  i < c->R; i++,sum = k = 0) {
		test[i]= malloc(sizeof(VECTOR)*sizeof(DATA)*distances->R);
		checkMem(test[i]);
		for (j=0; k<distances->R; k++) {
				assign = (int)assignation[k];
				if (assign == i) {
					/****
					printf("K = %d, I = %d\n",k,i); 
					printf("Asignando al clouster %d : %Lf\n",i,values[k]); ***/
					test[i][j] = values[k];
					sum +=values[k];
					assign = -1;
					j++;
					}
			}
		sum = sum/(j);
		/*** 
		printf("J value (asinations) = %d\n",j);
		printf("Total del Media = %Lf\n",sum);  ***/
		r_media = media(test[i],j);
	        for (l=0; l < new_clousters->C; l++) {
			new_clousters->store[i *new_clousters->C + l] = r_media;
		 	}
		// Delimiter ... END of vector
		test[i][j] =EOF;

	}

	//printf("\n\tCLOUSTER ASSIGNATED MATRIX\n");
	for (i=0; i<numClousters; i++) {
		for  (j=0; test[i][j]!=EOF; j++) {
			//printf("%Lf ",test[i][j]);
			}
		free(test[i]);
		test[i] = NULL;
		//printf("\n");
		}
	free(test);
	test = NULL;
	//printf("\n");

	return new_clousters;

}

// K-mean implementation ... 
MATRIX kmeans (MATRIX x, int numClousters, int MAXITER) 
{
	// Random seed
	srand(time(NULL));
	// Clousters matrix
	MATRIX clousters = createMatrix(numClousters, x->C);
	MATRIX new_clousters = NULL;
	// Distances matrix (for comparative with the clousters)
	MATRIX distances = createMatrix(x->R, numClousters);

	// Vectors: 1 Row
	VECTOR observation  = createMatrix(1,x->C)->store;
	VECTOR centroid = createMatrix(1,x->C)->store;
	VECTOR assignations = NULL;
	VECTOR new_assignations = NULL;
	
	// Index's for my loops 
	int 
		rowRandom,aleatory, //rowRandom => Choose a random observation
		i,j,
		k,l,m,
		steep; // STEEPS FOR THE ALGORITHM;

	// FIRST STEEP
	DATA d= 0; // Take Euclidean distance
	// Chosing clusters...
	for (m=i=0; i<clousters->R; i++) {
		for(aleatory=0; aleatory<1000; aleatory++) {
			rowRandom = rand() %x->R;
			 }
		//printf("Using row %d\n",rowRandom);
		// Selection clouster 
		for (j =0; j < clousters->C; j++) {
				// CENTROID Vector take the first line (row)
				centroid[j] =  // Matrix of Clousters
						clousters->store[i * clousters->C +j] = 
						// Take value from observations
						x->store[rowRandom * x->C + j];
			}
		for (k= 0; k < x->R; k++, m++)  {
			for (l=0; l <x->C; l++) {
				observation[l]  = x->store[k * x->C + l];
				// Creating DISTANCES MATRIX...
				// Put into cluster....only if l+1== x->C
				}
				//if (l+1 == x->C) {		
					d = distance_euclidean(// Take Euclidean distance:
								observation,// Vector of observation
								centroid, // Vector of centroid selected
								clousters->C // SIZE
								);

					//printf("Distance %Le\n",d);
					distances->store[m] = d;
				//	}
				//}			
			}		 
		}
		
		// New assignations = New clousters
		assignations = assing_clouster(distances)->store;
		clousters = update_clousters (clousters,distances);
	
		//printf("\n\tClousters MATRIX:\n");
		//print_matrix(clousters);
		//print_matrix(clousters);

	// STEEPS ....

	for (steep = 0; steep<MAXITER; steep++) {
		// STEEP 2
		d= 0; // Take Euclidean distance
		// Chosing clusters...
		for (m=i=0; i<clousters->R; i++) {
			//printf("Using row %d\n",rowRandom);
			// Selection clouster 
			for (j =0; j < clousters->C; j++) {
					// CENTROID Vector take the first line (row)
					centroid[j] = clousters->store[i * clousters->C +j];
				}
		
			for (k= 0; k < x->R; k++, m++)  {
				for (l=0; l <x->C; l++) {
					observation[l]  = x->store[k * x->C + l];
					}

					d = distance_euclidean(// Take Euclidean distance:
								observation,// Vector of observation
								centroid, // Vector of centroid selected
								clousters->C // SIZE
								);
					distances->store[m] = d;
				}		 
			}
		
		//printf("\n\tClousters MATRIX:\n");
		new_clousters = update_clousters (clousters,distances);
		new_assignations = assing_clouster(distances)->store;
		//print_matrix(clousters);
		//print_matrix(clousters); printf("\n\n");
		//print_matrix(new_clousters); printf("\n\n");
		
		// If there is not more assginations....
		if (equals_vector(new_assignations,assignations,distances->R)) {
				   // &&  equals_matrix(clousters,new_clousters)) {
			  /****
			  printf("\n\tKMEAN!!!!\n");
			  printf("\n\tKMEAN!!!!\n");  ***/
			  //printf("STEEPS: %d\n",steep);
			  //plot_matrix(distances);
			  //printf("\n\tAssignations:\n");
			  //print_vector(new_assignations,distances->R);
			  /******** FINISH *****************************/
			  return cut_matrix(clousters,1,clousters->R,1,1);
			}

		assignations = new_assignations;
		clousters = new_clousters;

		}
	
	printf("Error with the clousters\n");
	delete_all_matrix();
	exit(-1);
	return NULL;
}


// load file from argument 
int main(int argc,char *argv[])
{
	int n,h;
	n = 6;	// N. Neurons 
	h = (2*n)+1; // Kolmogorov Theorema 

	MATRIX training = load(argv[1],500,9); // File, Rows, Colums
	if (argv[2]) { // TESTING SET...
		MATRIX testing = load(argv[2],100,6);
		print_matrix(testing);
		}
	

	MATRIX Y = cut_matrix(training,1,500,1,3);
	//print_matrix(Y);
	//plot_matrix(Y);
	
	MATRIX C = kmeans (// Get clousters from K-MEANS ALGORITHM
			Y, // MATRIX 
			h, // No. CLOUSTERS
			500 // MAXITER (LOOP)
			);

	printf("\n\tClousters from KMEANS\n");
	print_matrix(C);
       
        /*
       
	MATRIX b = createMatrix(1,5);

	b->store[0] = 6;
	b->store[1] = 7;
	b->store[2] = 7;
	b->store[3] = 7;
	b->store[4] = 8;

	DATA d = distance_euclidean(a->store,b->store,a->C);
	printf("Distance %Lf\n",d);
		*/
	
	delete_all_matrix();
	return 0;
}

