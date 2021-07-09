#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	size_t m_rowsSize;
	size_t* m_rows;
	int*  m_cols_with_values;
	int* m_values;
} compressed_t;

void create_csr(size_t p_rows, size_t p_cols, const int p_matrix[p_rows][p_cols], compressed_t* o_csr);
int* matrix_multipy_vector(const compressed_t* const ptr_csr, const int* const ptr_vector, size_t p_vectorSize);
void destroy_csr(compressed_t* p_csr);

// Create a Compressed Sparse Row from a matrix
void create_csr(size_t p_rows, size_t p_cols, const int p_matrix[p_rows][p_cols], compressed_t* o_csr)
{
	o_csr->m_rowsSize = p_rows + 1;
	o_csr->m_rows = malloc(sizeof(size_t) * o_csr->m_rowsSize);
	// Allocate default buffers, default buffers are large enough for a completely dense matrix
	const size_t l_bufferSize = sizeof(int) * (p_rows * p_cols);
	int* ptr_cols = malloc(l_bufferSize);
	int* ptr_vals = malloc(l_bufferSize);
	
	// Set the beginning of the first row
	o_csr->m_rows[0] = 0;

	size_t l_nnzValues = 0;
	for (size_t row = 0; row < p_rows ; row++)
	{
		for (size_t col = 0; col < p_cols; col++)
		{
			if (p_matrix[row][col] != 0)
			{
				ptr_cols[l_nnzValues] = col;
				ptr_vals[l_nnzValues++] = p_matrix[row][col];
			}
		}
		o_csr->m_rows[row + 1] = l_nnzValues;
	}

	// realloc the arrays to make sure they are the correct size, if the matrix has any zeros the realloc will shrink
	if (l_nnzValues != l_bufferSize)
	{
		const size_t l_newSize = sizeof(size_t) * l_nnzValues;
		ptr_cols = realloc(ptr_cols, l_newSize);
		ptr_vals = realloc(ptr_vals, l_newSize); 
	}

	o_csr->m_cols_with_values = ptr_cols;
	o_csr->m_values = ptr_vals;
}

// Multipy csr with a vector and return a new vector
int* matrix_multipy_vector(const compressed_t* const ptr_csr, const int* const ptr_vector, size_t p_vectorSize)
{
	if (ptr_csr->m_rowsSize - 1 != p_vectorSize)
	{
		printf("Invalid matrix multiply\n");
		return NULL;
	}

	int* o_vector = malloc(sizeof(int) * p_vectorSize);
	for (size_t row = 0; row < ptr_csr->m_rowsSize - 1; row++)
	{
		o_vector[row] = 0;
		for (size_t col = ptr_csr->m_rows[row]; col < ptr_csr->m_rows[row + 1]; col++)
		{
			o_vector[row] += ptr_vector[ptr_csr->m_cols_with_values[col]] * ptr_csr->m_values[col];
		}
	}

	return o_vector;
}

// Clean up compressed_t 
void destroy_csr(compressed_t* p_csr)
{
	free(p_csr->m_rows);
	free(p_csr->m_cols_with_values);
	free(p_csr->m_values);
	p_csr->m_rows = NULL;
	p_csr->m_cols_with_values = NULL;
	p_csr->m_values = NULL;
}
 
int main(int argc, char** args)
{

	const int l_matrix[6][6] = {
		{3, 0, 0, 0, 1, 0},
		{0, 4, 1, 0, 5, 9},
		{0, 0, 0, 2, 0, 6},
		{5, 0, 0, 3, 0, 0},
		{0, 0, 0, 0, 5, 0},
		{0, 0, 0, 8, 9, 7}
	};

	const int l_vector[6] = {1, 4, 2, 8, 5, 7};

	compressed_t l_csr;
	create_csr(6, 6, l_matrix, &l_csr);

	for (size_t i = 0; i < l_csr.m_rowsSize; i++)
	{
		printf("%ld, ", l_csr.m_rows[i]);
	}
	printf("\n");

	for (size_t i = 0; i < l_csr.m_rows[l_csr.m_rowsSize - 1]; i++)
	{
		printf("%d, ", l_csr.m_cols_with_values[i]);
	}
	printf("\n");

	for (size_t i = 0; i < l_csr.m_rows[l_csr.m_rowsSize - 1]; i++)
	{
		printf("%d, ", l_csr.m_values[i]);
	}
	printf("\nAnswer: ");

	int* l_vector2 = matrix_multipy_vector(&l_csr, l_vector, 6);
	for (size_t i = 0; i < 6; i++)
	{
		printf("%d, ", l_vector2[i]);
	}
	free(l_vector2);

	destroy_csr(&l_csr);
	printf("\n");
	return 0;
}

