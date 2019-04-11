/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Matrix.h) - Source Finding Application             ///
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

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include "common.h"


// ----------------------------------------------------------------- //
// Class 'Matrix'                                                    //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a way of storing and      //
// handling matrices.                                                //
// ----------------------------------------------------------------- //

typedef class Matrix Matrix;

// Constructor and destructor
public Matrix       *Matrix_new        (const size_t rows, const size_t cols);  // Standard constructor
public Matrix       *Matrix_copy       (const Matrix *this);                    // Copy constructor
public Matrix       *Matrix_identity   (const size_t size);                     // Constructor for square identity matrix
public void          Matrix_delete     (Matrix *this);

// Methods
public void          Matrix_set_value  (Matrix *this, const size_t row, const size_t col, const double value);
public double        Matrix_get_value  (const Matrix *this, const size_t row, const size_t col);
public void          Matrix_add_value  (Matrix *this, const size_t row, const size_t col, const double value);
public void          Matrix_mul_value  (Matrix *this, const size_t row, const size_t col, const double value);
public void          Matrix_mul_scalar (Matrix *this, const double scalar);
public Matrix       *Matrix_mul_matrix (const Matrix *this, const Matrix *matrix);
public void          Matrix_add_matrix (Matrix *this, const Matrix *matrix);
public Matrix       *Matrix_transpose  (const Matrix *this);
public Matrix       *Matrix_invert     (const Matrix *this);
public void          Matrix_print      (const Matrix *this, const unsigned int width, const unsigned int decimals);
public double        Matrix_det        (const Matrix *this);
public double        Matrix_prob_dens  (const Matrix *covar, const Matrix *vector);

#endif
