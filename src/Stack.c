/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Stack.c) - Source Finding Application              ///
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

#include "Stack.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class Stack      //
// ----------------------------------------------------------------- //

class Stack
{
	size_t  size;
	size_t *data;
};



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Stack object.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty Stack object. //
//   Note that the destructor will have to be called explicitly once //
//   the object is no longer require to release its memory again.    //
// ----------------------------------------------------------------- //

public Stack *Stack_new(void)
{
	Stack *this = (Stack *)malloc(sizeof(Stack));
	ensure(this != NULL, "Failed to allocate memory for new Stack object.");
	
	this->size = 0;
	this->data = NULL;
	
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

public void Stack_delete(Stack *this)
{
	if(this != NULL) free(this->data);
	free(this);
	
	return;
}



// ----------------------------------------------------------------- //
// Push element onto stack                                           //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) value    - Value to be pushed.                              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for pushing a new element onto the stack. The     //
//   stack will automatically expand its memory to hold the extra    //
//   item and terminate with a stack overflow error if memory allo-  //
//   cation fails.                                                   //
// ----------------------------------------------------------------- //

public void Stack_push(Stack *this, const size_t value)
{
	// Sanity checks
	check_null(this);
	
	this->size += 1;
	this->data = (size_t *)realloc(this->data, this->size * sizeof(size_t));
	ensure(this->data != NULL, "Stack overflow error at %.5f MB memory usage.", (double)(this->size * sizeof(size_t)) / (1024.0 * 1024.0));
	this->data[this->size - 1] = value;
	
	return;
}



// ----------------------------------------------------------------- //
// Pop element from stack                                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Value of the element to be popped.                              //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for popping the last element from the stack. The  //
//   stack will automatically adjust its memory allocation to the    //
//   new, smaller size after popping. A stack underflow error will   //
//   be raised and the process terminated if the method is called on //
//   an empty stack.                                                 //
// ----------------------------------------------------------------- //

public size_t Stack_pop(Stack *this)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "Stack underflow error.");
	
	check_null(this->data);
	const size_t value = this->data[this->size - 1];
	this->size -= 1;
	
	if(this->size)
	{
		this->data = (size_t *)realloc(this->data, this->size * sizeof(size_t));
		ensure(this->data != NULL, "Stack overflow error at %.5f MB memory usage.", (double)(this->size * sizeof(size_t)) / (1024.0 * 1024.0));
	}
	else
	{
		free(this->data);
		this->data = NULL;
	}
	
	return value;
}



// ----------------------------------------------------------------- //
// Retrieve current stack size                                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Size of the stack (i.e. number of elements).                    //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the current size of the stack.     //
// ----------------------------------------------------------------- //

public size_t Stack_get_size(const Stack *this)
{
	check_null(this);
	return this->size;
}
