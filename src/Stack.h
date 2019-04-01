/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Stack.h) - Source Finding Application              ///
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

#ifndef STACK_H
#define STACK_H

#include "common.h"


// ----------------------------------------------------------------- //
// Class 'Stack'                                                     //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a simple stack for a re-  //
// cursive pixel linking algorithm. The stack is capable of storing  //
// the indices of the pixels identified as part of a source so their //
// neighbours can be checked recursively (using LIFO).               //
// ----------------------------------------------------------------- //

typedef class Stack Stack;

// Constructor and destructor
public Stack        *Stack_new      (void);
public void          Stack_delete   (Stack *this);

// Public methods
public void          Stack_push     (Stack *this, const size_t value);
public size_t        Stack_pop      (Stack *this);
public size_t        Stack_get_size (const Stack *this);

#endif
