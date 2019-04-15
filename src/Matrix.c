/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Matrix.c) - Source Finding Application             ///
/// Copyright (C) 2019 Tobias Westmeier                                  ///
/// ____________________________________________________________________ ///
///                                                                      ///
/// Address:  Tobias Westmeier                                           ///
///           ICRAR M468                                                 ///
///           The University of Western Australia                        ///
///           35 Stirling Highway                                        ///
///           Crawley WA 6009                                            ///
///           Australia                                                  ///
///                                                                      ///
/// E-mail:   tobias.westmeier [at] uwa.edu.au                           ///
/// ____________________________________________________________________ ///
///                                                                      ///
/// This program is free software: you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation, either version 3 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                      ///
/// This program is distributed in the hope that it will be useful,      ///
/// but WITHOUT ANY WARRANTY; without even the implied warranty of       ///
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         ///
/// GNU General Public License for more details.                         ///
///                                                                      ///
/// You should have received a copy of the GNU General Public License    ///
/// along with this program. If not, see http://www.gnu.org/licenses/.   ///
/// ____________________________________________________________________ ///
///                                                                      ///

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Matrix.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class Matrix     //
// ----------------------------------------------------------------- //

class Matrix
{
	size_t  rows;
	size_t  cols;
	double *values;
};

private inline size_t Matrix_get_index (const Matrix *this, const size_t row, const size_t col);
private void          Matrix_swap_rows (Matrix *this, const size_t row1, const size_t row2);
private void          Matrix_add_row   (Matrix *this, const size_t row1, const size_t row2, const double factor);
private void          Matrix_mul_row   (Matrix *this, const size_t row, const double factor);



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) rows     - Number of rows.                                  //
//   (2) cols     - Number of columns.                               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Matrix object.                         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new, empty Matrix object    //
//   and return a pointer to the newly created object. The number of //
//   rows and columns of the matrix needs to be specified. The ma-   //
//   trix will be initialised with 0. Note that the destructor will  //
//   need to be called explicitly once the object is no longer re-   //
//   quired to release any memory allocated during the lifetime of   //
//   the object.                                                     //
// ----------------------------------------------------------------- //

public Matrix *Matrix_new(const size_t rows, const size_t cols)
{
	// Sanity checks
	ensure(rows && cols, "Number of matrix rows and cols must be > 0.");
	
	Matrix *this = (Matrix *)malloc(sizeof(Matrix));
	ensure(this != NULL, "Failed to allocate memory for new Matrix object.");
	
	this->rows = rows;
	this->cols = cols;
	
	this->values = (double *)calloc(rows * cols, sizeof(double));
	ensure(this->values != NULL, "Memory allocation error while creating matrix.");
	
	return this;
}



// ----------------------------------------------------------------- //
// Copy constructor                                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) source   - Matrix to be copied.                             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created copy of matrix.                        //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Copy constructor. Will create a new matrix with the same dimen- //
//   sions as the specified source matrix and copy all entries from  //
//   source. A pointer to the newly created copy will be returned.   //
//   Note that the destructor will need to be called on the copy if  //
//   it is no longer required to release its memory.                 //
// ----------------------------------------------------------------- //

public Matrix *Matrix_copy(const Matrix *source)
{
	// Sanity checks
	check_null(source);
	
	// Call standard constructor
	Matrix *this = Matrix_new(source->rows, source->cols);
	
	// Copy values
	memcpy(this->values, source->values, source->rows * source->cols * sizeof(double));
	
	return this;
}



// ----------------------------------------------------------------- //
// Constructor for square identity matrix                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) size     - Size of the new matrix.                          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created identity matrix.                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Variant of the standard constructor that will create a square   //
//   identity matrix where all diagonal elements are set to 1. A     //
//   pointer to the newly created matrix will be returned. Note that //
//   the destructor will need to be called on the matrix if it is no //
//   longer required to release its memory.                          //
// ----------------------------------------------------------------- //

public Matrix *Matrix_identity(const size_t size)
{
	// Sanity checks
	ensure(size, "Matrix size must be > 0.");
	
	// Call standard constructor
	Matrix *this = Matrix_new(size, size);
	
	// Set diagonal elements to 1
	for(size_t i = 0; i < size; ++i) this->values[Matrix_get_index(this, i, i)] = 1.0;
	
	return this;
}



// ----------------------------------------------------------------- //
// Destructor                                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Destructor. Note that the destructor must be called explicitly  //
//   if the object is no longer required. This will release the me-  //
//   mory occupied by the object.                                    //
// ----------------------------------------------------------------- //

public void Matrix_delete(Matrix *this)
{
	if(this != NULL) free(this->values);
	free(this);
	
	return;
}



// ----------------------------------------------------------------- //
// Set matrix element to specified value                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row      - Row of the element to be set.                    //
//   (3) col      - Column of the element to be set.                 //
//   (4) value    - Value to set the element to.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the element at position (row, col) to //
//   the specified value. If row or col are out of range, the pro-   //
//   cess will be terminated.                                        //
// ----------------------------------------------------------------- //

public void Matrix_set_value(Matrix *this, const size_t row, const size_t col, const double value)
{
	// Sanity checks
	check_null(this);
	ensure(row < this->rows && col < this->cols, "Matrix row or col out of range.");
	
	this->values[Matrix_get_index(this, row, col)] = value;
	
	return;
}



// ----------------------------------------------------------------- //
// Get matrix element at specified position                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row      - Row of the element to be retrieved.              //
//   (3) col      - Column of the element to be retrieved.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Value of the matrix element at (row, col).                      //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the value of the matrix at the po- //
//   sition specified by row and col. If row or col are out of       //
//   range, the process will be terminated.                          //
// ----------------------------------------------------------------- //

public double Matrix_get_value(const Matrix *this, const size_t row, const size_t col)
{
	// Sanity checks
	check_null(this);
	ensure(row < this->rows && col < this->cols, "Matrix row or col out of range.");
	
	return this->values[Matrix_get_index(this, row, col)];
}



// ----------------------------------------------------------------- //
// Add specified value to matrix element                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row      - Row of the element to be set.                    //
//   (3) col      - Column of the element to be set.                 //
//   (4) value    - Value to add to the element.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding the specified value to the matrix ele- //
//   ment specified by (row, col). If row or col are out of range,   //
//   the process will be terminated.                                 //
// ----------------------------------------------------------------- //

public void Matrix_add_value(Matrix *this, const size_t row, const size_t col, const double value)
{
	// Sanity checks
	check_null(this);
	ensure(row < this->rows && col < this->cols, "Matrix row or col out of range.");
	
	this->values[Matrix_get_index(this, row, col)] += value;
	
	return;
}



// ----------------------------------------------------------------- //
// Multiply matrix element by specified value                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row      - Row of the element to be set.                    //
//   (3) col      - Column of the element to be set.                 //
//   (4) value    - Value to multiply element by.                    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for multiplying the matrix element specified by   //
//   (row, col) by value. If row or col are out of range, the pro-   //
//   cess will be terminated.                                        //
// ----------------------------------------------------------------- //

public void Matrix_mul_value(Matrix *this, const size_t row, const size_t col, const double value)
{
	// Sanity checks
	check_null(this);
	ensure(row < this->rows && col < this->cols, "Matrix row or col out of range.");
	
	this->values[Matrix_get_index(this, row, col)] *= value;
	
	return;
}



// ----------------------------------------------------------------- //
// Multiply matrix by scalar                                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) scalar   - Scalar to multiply the matrix by.                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for multiplying the matrix with the specified     //
//   scalar value. The matrix will be multiplied in situ.            //
// ----------------------------------------------------------------- //

public void Matrix_mul_scalar(Matrix *this, const double scalar)
{
	// Sanity checks
	check_null(this);
	
	for(double *ptr = this->values + this->rows * this->cols; ptr --> this->values;) *ptr *= scalar;
	
	return;
}



// ----------------------------------------------------------------- //
// Matrix multiplication                                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) matrix   - Matrix by which to multiply.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Result of the matrix multiplication.                            //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for multiplying a matrix with another one. The    //
//   result of the multiplication will be written to a new matrix    //
//   that will be returned by the method. For this to work, the num- //
//   ber of columns in the left operand (this) must be equal to the  //
//   number of rows in the right operand (matrix). If not, the pro-  //
//   cess will be terminated. The user is responsible for calling    //
//   the destructor on the returned result matrix once it is no      //
//   longer needed.                                                  //
// ----------------------------------------------------------------- //

public Matrix *Matrix_mul_matrix(const Matrix *this, const Matrix *matrix)
{
	// Sanity checks
	check_null(this);
	check_null(matrix);
	ensure(this->cols == matrix->rows, "Incompatible row and column numbers in matrix multiplication.");
	
	// Create result matrix
	Matrix *result = Matrix_new(this->rows, matrix->cols);
	
	// Calculate entries
	for(size_t i = 0; i < result->rows; ++i)
	{
		for(size_t k = 0; k < result->cols; ++k)
		{
			double value = 0.0;
			for(size_t j = 0; j < this->cols; ++j) value += this->values[Matrix_get_index(this, i, j)] * matrix->values[Matrix_get_index(matrix, j, k)];
			result->values[Matrix_get_index(result, i, k)] += value;
		}
	}
	
	return result;
}



// ----------------------------------------------------------------- //
// Add two matrices                                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) matrix   - Matrix to be added.                              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding one matrix to another. Both matrices   //
//   must have the same size, otherwise the process will be termina- //
//   ted. The input matrix will be modified in situ.                 //
// ----------------------------------------------------------------- //

public void Matrix_add_matrix(Matrix *this, const Matrix *matrix)
{
	// Sanity checks
	check_null(this);
	check_null(matrix);
	ensure(this->rows == matrix->rows && this->cols == matrix->cols, "Incompatible row and column numbers in matrix addition.");
	
	// Calculate entries
	for(size_t i = 0; i < this->rows; ++i)
	{
		for(size_t j = 0; j < this->cols; ++j)
		{
			this->values[Matrix_get_index(this, i, j)] += matrix->values[Matrix_get_index(matrix, i, j)];
		}
	}
	
	return;
}


// ----------------------------------------------------------------- //
// Calculate v^T M v                                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) vector   - Vector to be multiplied.                         //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Result of v^T M v (which is a scalar).                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for calculating the product of v^T M v between    //
//   the specified matrix, M, and vector, v, where v^T denotes the   //
//   transpose of the vector. The matrix must be a square matrix     //
//   with a size equal to that of the vector. The vector is expected //
//   to be a column vector.                                          //
// ----------------------------------------------------------------- //

public double Matrix_vMv(const Matrix *this, const Matrix *vector)
{
	// Sanity checks
	check_null(this);
	check_null(vector);
	ensure(this->rows == this->cols, "Matrix is not square.");
	ensure(vector->cols == 1, "Vector has more than one column.");
	ensure(this->rows == vector->rows, "Vector size (%zu) does not match matrix (%zu x %zu).", vector->rows, this->rows, this->cols);
	
	const size_t size = this->rows;
	
	double *array = (double *)calloc(size, sizeof(double));
	ensure(array != NULL, "Memory allocation error during matrix-vector multiplication.");
	
	for(size_t col = size; col--;) {
		for(size_t row = size; row--;) array[col] += Matrix_get_value(vector, row, 0) * Matrix_get_value(this, row, col);
	}
	
	double result = 0.0;
	for(size_t i = size; i--;) result += array[i] * Matrix_get_value(vector, i, 0);
	
	free(array);
	
	return result;
}



// ----------------------------------------------------------------- //
// Transpose matrix                                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Transpose of the input matrix.                                  //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for transposing the specified matrix. The trans-  //
//   posed matrix will be returned. The user will be responsible for //
//   calling the destructor on the transposed matrix once it is no   //
//   longer needed.                                                  //
// ----------------------------------------------------------------- //

public Matrix *Matrix_transpose(const Matrix *this)
{
	// Sanity checks
	check_null(this);
	
	// Create result matrix with rows and cols swapped
	Matrix *result = Matrix_new(this->cols, this->rows);
	
	// Calculate entries
	for(size_t i = 0; i < this->rows; ++i)
	{
		for(size_t j = 0; j < this->cols; ++j)
		{
			result->values[Matrix_get_index(result, j, i)] = this->values[Matrix_get_index(this, i, j)];
		}
	}
	
	return result;
}



// ----------------------------------------------------------------- //
// Invert matrix                                                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Inverse of the input matrix.                                    //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for inverting the specified matrix. The nethod    //
//   makes use of the Gauss-Jordan elimination algorithm for this    //
//   purpose, unless the matrix size is <= 3, in which case the so-  //
//   lution is calculated analytically. The inverted matrix will be  //
//   returned. If the matrix is not invertible, a NULL pointer will  //
//   instead be returned. NOTE that the Gauss-Jordan elimination al- //
//   gorithm can be numerically unstable, and integer numbers might  //
//   get represented as non-integer values.                          //
// ----------------------------------------------------------------- //

public Matrix *Matrix_invert(const Matrix *this)
{
	// Sanity checks
	check_null(this);
	ensure(this->rows == this->cols, "Cannot invert non-square matrix.");
	
	const size_t size = this->rows;
	
	
	// Special case: matrix size up to 3 x 3 --> use analytic solution
	// ---------------------------------------------------------------
	
	if(size <= 3)
	{
		// Calculate and check determinant
		const double det = Matrix_det(this, 1.0);
		if(det == 0.0)
		{
			warning("Matrix is not invertible.");
			return NULL;
		}
		
		// Create empty inverse matrix
		Matrix *result = Matrix_new(size, size);
		
		// Set inverse matrix entries
		if(size == 1)
		{
			// 1 x 1 matrix
			Matrix_set_value(result, 0, 0, 1.0 / det);
		}
		else if(size == 2)
		{
			// 2 x 2 matrix
			Matrix_set_value(result, 0, 0,  Matrix_get_value(this, 1, 1) / det);
			Matrix_set_value(result, 0, 1, -Matrix_get_value(this, 0, 1) / det);
			Matrix_set_value(result, 1, 0, -Matrix_get_value(this, 1, 0) / det);
			Matrix_set_value(result, 1, 1,  Matrix_get_value(this, 0, 0) / det);
		}
		else if(size == 3)
		{
			// 3 x 3 matrix
			const double a = Matrix_get_value(this, 0, 0);
			const double b = Matrix_get_value(this, 0, 1);
			const double c = Matrix_get_value(this, 0, 2);
			const double d = Matrix_get_value(this, 1, 0);
			const double e = Matrix_get_value(this, 1, 1);
			const double f = Matrix_get_value(this, 1, 2);
			const double g = Matrix_get_value(this, 2, 0);
			const double h = Matrix_get_value(this, 2, 1);
			const double i = Matrix_get_value(this, 2, 2);
			
			Matrix_set_value(result, 0, 0,  (e * i - f * h) / det);
			Matrix_set_value(result, 0, 1,  (c * h - b * i) / det);
			Matrix_set_value(result, 0, 2,  (b * f - c * e) / det);
			Matrix_set_value(result, 1, 0,  (f * g - d * i) / det);
			Matrix_set_value(result, 1, 1,  (a * i - c * g) / det);
			Matrix_set_value(result, 1, 2,  (c * d - a * f) / det);
			Matrix_set_value(result, 2, 0,  (d * h - e * g) / det);
			Matrix_set_value(result, 2, 1,  (b * g - a * h) / det);
			Matrix_set_value(result, 2, 2,  (a * e - b * d) / det);
		}
		
		return result;
	}
	
	
	// General case: N x N matrix --> use Gauss-Jordan elimination
	// -----------------------------------------------------------
	
	// Create initial left and right square matrix
	Matrix *L = Matrix_copy(this);
	Matrix *R = Matrix_identity(size);
	
	// For each column (i) in the matrix
	for(size_t i = 0; i < size; ++i)
	{
		// Find maximum pivot
		double pivot_max = fabs(L->values[Matrix_get_index(L, i, i)]);
		size_t pivot_max_row = i;
		
		for(size_t j = i + 1; j < size; ++j)
		{
			double value = Matrix_get_value(L, j, i);
			if(pivot_max < fabs(value))
			{
				pivot_max = fabs(value);
				pivot_max_row = j;
			}
		}
		
		// Ensure that the maximum pivot is > 0
		// as otherwise the matrix is not invertible
		if(pivot_max == 0.0)
		{
			warning("Matrix is not invertible.");
			Matrix_delete(L);
			Matrix_delete(R);
			return NULL;
		}
		
		// Swap rows if necessary
		if(pivot_max_row != i)
		{
			Matrix_swap_rows(L, i, pivot_max_row);
			Matrix_swap_rows(R, i, pivot_max_row);
		}
		
		// Extract diagonal element (i, i) as pivot
		double pivot = L->values[Matrix_get_index(L, i, i)];
		
		// Divide pivot row (i) by pivot to get a diagonal of 1
		Matrix_mul_row(L, i, 1.0 / pivot);
		Matrix_mul_row(R, i, 1.0 / pivot);
		
		// Subtract multiple of pivot row (i) from every other row (j) to get non-diagonals of 0
		for(size_t j = 0; j < size; ++j)
		{
			if(j != i)
			{
				const double factor = -Matrix_get_value(L, j, i);
				Matrix_add_row(L, j, i, factor);
				Matrix_add_row(R, j, i, factor);
			}
		}
	}
	
	// Clean up
	Matrix_delete(L);
	
	return R;
}



// ----------------------------------------------------------------- //
// Print matrix                                                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) width    - Width of each column in characters.              //
//   (3) decimals - Number of decimals after the decimal point.      //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for printing the matrix to the standard output.   //
//   The width parameter specifies how many characters each printed  //
//   column is wide. The number of decimals after the decimal point  //
//   is specified with the decimals argument.                        //
// ----------------------------------------------------------------- //

public void Matrix_print(const Matrix *this, const unsigned int width, const unsigned int decimals)
{
	// Sanity checks
	check_null(this);
	
	for(size_t row = 0; row < this->rows; ++row)
	{
		for(size_t col = 0; col < this->cols; ++col)
		{
			printf("%*.*f", width, decimals, Matrix_get_value(this, row, col));
		}
		
		printf("\n");
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Calculate determinant                                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) scale_factor - A scale factor by which each element of the  //
//                      matrix is multiplied before calculating the  //
//                      determinant. Set to 1 for no scaling.        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Determinant of specified matrix.                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the determinant of the specified    //
//   matrix. The determinant for matrices of size <= 3 is calculated //
//   analytically, while the determinant of larger matrices is not   //
//   yet implemented.                                                //
//   The purpose of the scale factor is to be able to make use of    //
//   the fact that det(f * M) = f^n * det(M). By setting the scale   //
//   factor to a value != 1, one can efficiently calculate the pro-  //
//   duct f^n * det(M), which is needed in the calculation of the    //
//   PDF of the multivariate normal distribution below.              //
// ----------------------------------------------------------------- //

public double Matrix_det(const Matrix *this, const double scale_factor)
{
	// Sanity checks
	check_null(this);
	ensure(this->rows == this->cols, "Cannot calculate determinant of non-square matrix.");
	
	if(this->rows == 1)
	{
		return scale_factor * *(this->values);
	}
	
	if(this->rows == 2)
	{
		return scale_factor * scale_factor * (Matrix_get_value(this, 0, 0) * Matrix_get_value(this, 1, 1) - Matrix_get_value(this, 0, 1) * Matrix_get_value(this, 1, 0));
	}
	
	if(this->rows == 3)
	{
		return scale_factor * scale_factor * scale_factor
			*  (Matrix_get_value(this, 0, 0) * Matrix_get_value(this, 1, 1) * Matrix_get_value(this, 2, 2)
			+   Matrix_get_value(this, 0, 1) * Matrix_get_value(this, 1, 2) * Matrix_get_value(this, 2, 0)
			+   Matrix_get_value(this, 0, 2) * Matrix_get_value(this, 1, 0) * Matrix_get_value(this, 2, 1)
			-   Matrix_get_value(this, 0, 2) * Matrix_get_value(this, 1, 1) * Matrix_get_value(this, 2, 0)
			-   Matrix_get_value(this, 0, 1) * Matrix_get_value(this, 1, 0) * Matrix_get_value(this, 2, 2)
			-   Matrix_get_value(this, 0, 0) * Matrix_get_value(this, 1, 2) * Matrix_get_value(this, 2, 1));
	}
	
	warning("Determinant calculation for N > 3 not yet implemented.");
	return 0.0;
}



// ----------------------------------------------------------------- //
// Probability density of a multivariate normal distribution         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) covar_inv - Inverse of the covariance matrix. Can be calcu- //
//                   lated with Matrix_invert().                     //
//   (2) vector    - Coordinate (or parameter) vector relative to    //
//                   the mean for which the probability density is   //
//                   to be returned.                                 //
//   (3) scal_fact - Scale factor of 1 divided by the square root of //
//                   the determinant of 2 pi times the covariance    //
//                   matrix, 1 / SQRT(|2 pi covar|).                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Probability density at the position of vector.                  //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for calculating the probability density of a mul- //
//   tivariate normal distribution described by the specified co-    //
//   variance matrix at the location specified by 'vector' (relative //
//   to the mean). For this to work, the covariance matrix must be   //
//   square and invertible, and the vector must be in column form    //
//   with a size equal to that of the covariance matrix. The vector  //
//   entries must be relative to the centroid, (x - <x>) such that 0 //
//   corresponds to the peak of the PDF. The PDF will be correctly   //
//   normalised such that the integral over the entire n-dimensional //
//   space is 1.                                                     //
// ----------------------------------------------------------------- //

public double Matrix_prob_dens(const Matrix *covar_inv, const Matrix *vector, const double scal_fact)
{
	// Sanity checks
	check_null(covar_inv);
	check_null(vector);
	ensure(covar_inv->rows == covar_inv->cols, "Covariance matrix must be square.");
	ensure(covar_inv->rows == vector->rows && vector->cols == 1, "Vector size does not match covariance matrix.");
	
	// Return PDF = exp(-0.5 v^T C^-1 v) / SQRT((2 pi)^n |C|) of multivariate normal distribution
	return scal_fact * exp(-0.5 * Matrix_vMv(covar_inv, vector));
}



// ----------------------------------------------------------------- //
// Get array index from row and column                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row      - Row number.                                      //
//   (3) col      - Column number.                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Array index corresponding to the row and column number.         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for returning the array index corresponding to   //
//   the specified row and column number. This can be used to di-    //
//   rectly access the data array from within public and private     //
//   methods without having to use the Matrix_get_value() and        //
//   Matrix_set_value() methods.                                     //
// ----------------------------------------------------------------- //

private inline size_t Matrix_get_index(const Matrix *this, const size_t row, const size_t col)
{
	return row + this->rows * col;
}



// ----------------------------------------------------------------- //
// Swap two matrix rows                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row1     - First row to be swapped.                         //
//   (3) row2     - Second row to be swapped.                        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for swapping the two rows specified by row1 and  //
//   row2 of the matrix. This method is needed for the Gauss-Jordan  //
//   elimination algorithm.                                          //
// ----------------------------------------------------------------- //

private void Matrix_swap_rows(Matrix *this, const size_t row1, const size_t row2)
{
	for(size_t i = 0; i < this->cols; ++i) swap(this->values + Matrix_get_index(this, row1, i), this->values + Matrix_get_index(this, row2, i));
	return;
}



// ----------------------------------------------------------------- //
// Add factor * row2 to row1                                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row1     - Row to be manipulated.                           //
//   (3) row2     - Row to be scaled and added to row1.              //
//   (4) factor   - Factor to multiply row2 by.                      //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for multiplying row2 by factor and adding the    //
//   result to row1 of the specified matrix. This method is needed   //
//   for the Gauss-Jordan elimination algorithm.                     //
// ----------------------------------------------------------------- //

private void Matrix_add_row(Matrix *this, const size_t row1, const size_t row2, const double factor)
{
	for(size_t i = 0; i < this->cols; ++i) this->values[Matrix_get_index(this, row1, i)] += factor * this->values[Matrix_get_index(this, row2, i)];
	return;
}



// ----------------------------------------------------------------- //
// Multiply row by factor                                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) row      - Row to be multipled.                             //
//   (3) factor   - Factor to multiply row by.                       //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for multiplying the specified row of the matrix  //
//   by the specified factor. This method is needed for the Gauss-   //
//   Jordan elimination algorithm.                                   //
// ----------------------------------------------------------------- //

private void Matrix_mul_row(Matrix *this, const size_t row, const double factor)
{
	for(size_t i = 0; i < this->cols; ++i) this->values[Matrix_get_index(this, row, i)] *= factor;
	return;
}
