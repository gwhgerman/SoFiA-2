/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (DataCube.c) - Source Finding Application           ///
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
#include <stdint.h>
#include <limits.h>

#include "DataCube.h"
#include "Source.h"
#include "statistics_flt.h"
#include "statistics_dbl.h"



// ----------------------------------------------------------------- //
// Compile-time checks to ensure that                               //
//                                                                   //
//   1. the number of bits per byte is 8 (CHAR_BIT),                 //
//                                                                   //
//   2. sizeof(int) is greater than sizeof(char),                    //
//                                                                   //
//   3. the size of int8_t, int16_t, int32_t and int64_t is exactly  //
//      1, 2, 4 and 8, respectively, and                             //
//                                                                   //
//   4. the size of float and double is 4 and 8, respectively.       //
//                                                                   //
// Without these conditions the code would not function properly. If //
// any of these conditions is not met, a compiler error should be    //
// raised, e.g.: "size of array is negative". This is to ensure that //
// the code can only be run on compliant architectures, as it would  //
// not work correctly on machines that don't fulfil these require-   //
// ments. For that reason these checks must not be disabled to en-   //
// force compilation on non-compliant systems.                       //
// ----------------------------------------------------------------- //

COMPILE_TIME_CHECK ( CHAR_BIT == 8,              FATAL_Number_of_bits_per_byte_is_not_equal_to_8 );
COMPILE_TIME_CHECK ( sizeof(int) > sizeof(char), FATAL_Size_of_int_is_not_greater_than_size_of_char );
COMPILE_TIME_CHECK ( sizeof(int8_t)  == 1,       FATAL_Size_of_uint8_is_not_equal_to_1 );
COMPILE_TIME_CHECK ( sizeof(int16_t) == 2,       FATAL_Size_of_int16_is_not_equal_to_2 );
COMPILE_TIME_CHECK ( sizeof(int32_t) == 4,       FATAL_Size_of_int32_is_not_equal_to_4 );
COMPILE_TIME_CHECK ( sizeof(int64_t) == 8,       FATAL_Size_of_int64_is_not_equal_to_8 );
COMPILE_TIME_CHECK ( sizeof(float)   == 4,       FATAL_Size_of_float_is_not_equal_to_4 );
COMPILE_TIME_CHECK ( sizeof(double)  == 8,       FATAL_Size_of_double_is_not_equal_to_8 );



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class DataCube   //
// ----------------------------------------------------------------- //

class DataCube
{
	char   *data;
	size_t  data_size;
	char   *header;
	size_t  header_size;
	int     data_type;
	int     word_size;
	size_t  dimension;
	size_t  axis_size[4];
	double  bscale;
	double  bzero;
};

private        int     DataCube_gethd_raw(const DataCube *this, const char *key, char *buffer);
private        int     DataCube_puthd_raw(DataCube *this, const char *key, const char *buffer);
private inline size_t  DataCube_get_index(const DataCube *this, const size_t x, const size_t y, const size_t z);
private        void    DataCube_mark_neighbours(const DataCube *this, DataCube *mask, const size_t x, const size_t y, const size_t z, const size_t radius_x, const size_t radius_y, const size_t radius_z, const int32_t label, LinkerPar *lpar);
private        void    DataCube_swap_byte_order(const DataCube *this);



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created DataCube object.                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty DataCube ob-  //
//   ject and return a pointer to the newly created object. No me-   //
//   mory will be allocated other than for the object itself. Note   //
//   that the destructor will need to be called explicitly once the  //
//   object is no longer required to release any memory allocated    //
//   during the lifetime of the object.                              //
// ----------------------------------------------------------------- //

public DataCube *DataCube_new(void)
{
	DataCube *this = (DataCube*)malloc(sizeof(DataCube));
	ensure(this != NULL, "Failed to allocate memory for new data cube object.");
	
	// Initialise properties
	this->data         = NULL;
	this->data_size    = 0;
	this->header       = NULL;
	this->header_size  = 0;
	this->data_type    = 0;
	this->word_size    = 0;
	this->dimension    = 0;
	this->axis_size[0] = 0;
	this->axis_size[1] = 0;
	this->axis_size[2] = 0;
	this->axis_size[3] = 0;
	
	return this;
}



// ----------------------------------------------------------------- //
// Copy constructor                                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) source - Pointer to DataCube object to be copied.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created DataCube object.                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Copy constructor. Will create a new DataCube object that is a   //
//   physical copy of the object pointed to by source. A pointer to  //
//   the newly created object will be returned. Note that the de-    //
//   structor will need to be called explicitly once the object is   //
//   no longer required to release any memory allocated to the       //
//   object.                                                         //
// ----------------------------------------------------------------- //

public DataCube *DataCube_copy(const DataCube *source)
{
	ensure(source != NULL, "Invalid DataCube object passed to copy constructor.");
	
	DataCube *this = DataCube_new();
	
	// Copy header
	this->header = (char *)malloc(source->header_size * sizeof(char));
	ensure(this->header != NULL, "Failed to reserve memory for FITS header.");
	memcpy(this->header, source->header, source->header_size);
	
	// Copy data
	this->data = (char *)malloc(source->word_size * source->data_size * sizeof(char));
	ensure(this->data != NULL, "Failed to reserve memory for FITS data.");
	memcpy(this->data, source->data, source->word_size * source->data_size);
	
	// Copy remaining properties
	this->data_size    = source->data_size;
	this->header_size  = source->header_size;
	this->data_type    = source->data_type;
	this->word_size    = source->word_size;
	this->dimension    = source->dimension;
	this->axis_size[0] = source->axis_size[0];
	this->axis_size[1] = source->axis_size[1];
	this->axis_size[2] = source->axis_size[2];
	this->axis_size[3] = source->axis_size[3];
	
	return this;
}



// ----------------------------------------------------------------- //
// Variant of standard constructor                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) nx     - Size of first axis of data array.                  //
//   (2) ny     - Size of second axis of data array.                 //
//   (3) nz     - Size of third axis of data array.                  //
//   (4) type   - Standard FITS data type (-64, -32, 8, 16, 32, 64). //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created DataCube object.                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Alternative standard constructor. Will create a new DataCube    //
//   object with the dimensions and data type specified. Memory for  //
//   the data array and basic header will be allocated. The array    //
//   will be initialised with a value of 0. A pointer to the newly   //
//   created object will be returned. Note that the destructor will  //
//   need to be called explicitly once the object is no longer re-   //
//   quired to release any memory allocated to the object.           //
// ----------------------------------------------------------------- //

public DataCube *DataCube_blank(const size_t nx, const size_t ny, const size_t nz, const int type)
{
	// Sanity checks
	ensure(nx > 0 && ny > 0 && nz > 0, "Illegal data cube size of (%zu, %zu, %zu) requested.", nx, ny, nz);
	ensure(abs(type) == 64 || abs(type) == 32 || type == 8 || type == 16, "Invalid FITS data type of %d requested.", type);
	
	DataCube *this = DataCube_new();
	
	// Set up properties
	this->data_size    = nx * ny * nz;
	this->header_size  = FITS_HEADER_BLOCK_SIZE;
	this->data_type    = type;
	this->word_size    = abs(type / 8);
	this->dimension    = nz > 1 ? 3 : (ny > 1 ? 2 : 1);
	this->axis_size[0] = nx;
	this->axis_size[1] = ny;
	this->axis_size[2] = nz;
	this->axis_size[3] = 0;
	
	// Create data array filled with 0
	this->data = (char *)calloc(this->data_size, this->word_size * sizeof(char));
	ensure(this->data != NULL, "Failed to reserve memory for FITS data.");
	
	// Create basic header
	this->header = (char *)malloc(this->header_size * sizeof(char));
	ensure(this->header != NULL, "Failed to reserve memory for FITS header.");
	
	// Fill entire header with spaces
	memset(this->header, ' ', this->header_size);
	
	// Insert required header information
	memcpy(this->header, "END", 3);
	DataCube_puthd_bool(this, "SIMPLE", true);
	DataCube_puthd_int(this, "BITPIX", this->data_type);
	DataCube_puthd_int(this, "NAXIS",  this->dimension);
	DataCube_puthd_int(this, "NAXIS1", this->axis_size[0]);
	if(this->dimension > 1) DataCube_puthd_int (this, "NAXIS2", this->axis_size[1]);
	if(this->dimension > 2) DataCube_puthd_int (this, "NAXIS3", this->axis_size[2]);
	DataCube_puthd_flt(this, "CRPIX1", 1.0);
	DataCube_puthd_flt(this, "CDELT1", 1.0);
	DataCube_puthd_flt(this, "CRVAL1", 1.0);
	if(this->dimension > 1)
	{
		DataCube_puthd_flt(this, "CRPIX2", 1.0);
		DataCube_puthd_flt(this, "CDELT2", 1.0);
		DataCube_puthd_flt(this, "CRVAL2", 1.0);
	}
	if(this->dimension > 2)
	{
		DataCube_puthd_flt(this, "CRPIX3", 1.0);
		DataCube_puthd_flt(this, "CDELT3", 1.0);
		DataCube_puthd_flt(this, "CRVAL3", 1.0);
	}
	
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

public void DataCube_delete(DataCube *this)
{
	if(this != NULL)
	{
		free(this->data);
		free(this->header);
		free(this);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Read data cube from FITS file                                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) filename - Name of the input FITS file.                     //
//   (3) region   - Array of 6 values denoting a region of the cube  //
//                  to be read in (format: x_min, x_max, y_min,      //
//                  y_max, z_min, z_max). Set to NULL to read entire //
//                  data cube.                                       //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for reading a data cube from a FITS file. The     //
//   data cube must have between 1 and 3 dimensions. 4-dimensional   //
//   FITS cubes are also supported as long as the 4th axis is of     //
//   size 1 (e.g. Stokes I). A region can be specified to read only  //
//   a portion of the image. The region must be of the form x_min,   //
//   x_max, y_min, y_max, z_min, z_max. If NULL, the full cube will  //
//   be read in.                                                     //
// ----------------------------------------------------------------- //

public void DataCube_load(DataCube *this, const char *filename, const Array *region)
{
	// Sanity checks
	check_null(this);
	check_null(filename);
	ensure(strlen(filename), "Empty file name provided.");
	
	// Check region specification
	if(region != NULL)
	{
		ensure(Array_get_size(region) == 6, "Invalid region supplied; must contain 6 values.");
		for(size_t i = 0; i < Array_get_size(region); i += 2) ensure(Array_get_int(region, i) <= Array_get_int(region, i + 1), "Invalid region supplied; minimum greater than maximum.");
	}
	
	// Open FITS file
	message("Opening FITS file \'%s\'.", filename);
	FILE *fp = fopen(filename, "rb");
	ensure(fp != NULL, "Failed to open FITS file \'%s\'.", filename);
		
	// Read entire header
	bool end_reached = false;
	this->header_size = 0;
	char *ptr;
	
	while(!end_reached)
	{
		// (Re-)allocate memory as needed
		this->header = (char *)realloc(this->header, (this->header_size + FITS_HEADER_BLOCK_SIZE) * sizeof(char));
		ensure(this->header != NULL, "Failed to reserve memory for FITS header.");
		
		// Read header block
		ensure(fread(this->header + this->header_size, 1, FITS_HEADER_BLOCK_SIZE, fp) == FITS_HEADER_BLOCK_SIZE, "FITS file ended unexpectedly while reading header.");
		
		// Check if we have reached the end of the header
		ptr = this->header + this->header_size;
		
		while(!end_reached && ptr < this->header + this->header_size + FITS_HEADER_BLOCK_SIZE)
		{
			if(strncmp(ptr, "END", 3) == 0) end_reached = true;
			else ptr += FITS_HEADER_LINE_SIZE;
		}
		
		// Set header size parameter
		this->header_size += FITS_HEADER_BLOCK_SIZE;
	}
	
	// Check if valid FITS file
	ensure(strncmp(this->header, "SIMPLE", 6) == 0, "Missing \'SIMPLE\' keyword; file does not appear to be a FITS file.");
	
	// Extract crucial header elements
	this->data_type    = DataCube_gethd_int(this, "BITPIX");
	this->dimension    = DataCube_gethd_int(this, "NAXIS");
	this->axis_size[0] = DataCube_gethd_int(this, "NAXIS1");
	this->axis_size[1] = DataCube_gethd_int(this, "NAXIS2");
	this->axis_size[2] = DataCube_gethd_int(this, "NAXIS3");
	this->axis_size[3] = DataCube_gethd_int(this, "NAXIS4");
	this->word_size    = abs(this->data_type) / 8;             // WARNING: Assumes 8 bits per char; see CHAR_BIT in limits.h.
	this->data_size    = this->axis_size[0];
	for(size_t i = 1; i < this->dimension; ++i) this->data_size *= this->axis_size[i];
	
	// Sanity checks
	ensure(this->data_type == -64
		|| this->data_type == -32
		|| this->data_type == 8
		|| this->data_type == 16
		|| this->data_type == 32
		|| this->data_type == 64,
		"Invalid BITPIX keyword encountered.");
	
	ensure(this->dimension > 0
		&& this->dimension < 5,
		"Only FITS files with 1-4 dimensions supported.");
	
	ensure(this->dimension < 4
		|| this->axis_size[3] == 1,
		"The size of the 4th axis must be 1.");
	
	ensure(this->data_size > 0, "Invalid NAXISn keyword encountered.");
	
	// Handle BSCALE and BZERO if necessary (not yet supported)
	const double bscale = DataCube_gethd_flt(this, "BSCALE");
	const double bzero  = DataCube_gethd_flt(this, "BZERO");
	
	// Check for non-trivial BSCALE and BZERO (not currently supported)
	ensure((IS_NAN(bscale) || bscale == 1.0) && (IS_NAN(bzero) || bzero == 0.0), "Non-trivial BSCALE and BZERO not currently supported.");
	
	// Work out region
	const size_t x_min = (region != NULL && Array_get_int(region, 0) > 0) ? Array_get_int(region, 0) : 0;
	const size_t x_max = (region != NULL && Array_get_int(region, 1) < this->axis_size[0] - 1) ? Array_get_int(region, 1) : this->axis_size[0] - 1;
	const size_t y_min = (region != NULL && Array_get_int(region, 2) > 0) ? Array_get_int(region, 2) : 0;
	const size_t y_max = (region != NULL && Array_get_int(region, 3) < this->axis_size[1] - 1) ? Array_get_int(region, 3) : this->axis_size[1] - 1;
	const size_t z_min = (region != NULL && Array_get_int(region, 4) > 0) ? Array_get_int(region, 4) : 0;
	const size_t z_max = (region != NULL && Array_get_int(region, 5) < this->axis_size[2] - 1) ? Array_get_int(region, 5) : this->axis_size[2] - 1;
	const size_t region_nx = x_max - x_min + 1;
	const size_t region_ny = y_max - y_min + 1;
	const size_t region_nz = z_max - z_min + 1;
	const size_t region_size = region_nx * region_ny * region_nz;
	
	// Allocate memory for data array
	this->data = (char *)realloc(this->data, this->word_size * region_size * sizeof(char));
	ensure(this->data != NULL, "Failed to reserve memory for FITS data.");
	
	// Print status information
	message("Reading FITS data with the following specifications:");
	message("  Data type:    %d", this->data_type);
	message("  No. of axes:  %zu", this->dimension);
	message("  Axis sizes:   %zu, %zu, %zu", this->axis_size[0], this->axis_size[1], this->axis_size[2]);
	message("  Region:       %zu-%zu, %zu-%zu, %zu-%zu", x_min, x_max, y_min, y_max, z_min, z_max);
	message("  Memory used:  %.1f MB", (double)(region_size * this->word_size) / 1048576.0);
	
	// Read data
	if(region == NULL)
	{
		// No region supplied -> read full cube
		ensure(fread(this->data, this->word_size, this->data_size, fp) == this->data_size, "FITS file ended unexpectedly while reading data.");
	}
	else
	{
		// Region supplied -> read sub-cube
		char *ptr = this->data;
		const size_t fp_start = (size_t)ftell(fp); // Start position of data array in file
		
		// Read relevant data segments
		for(size_t z = z_min; z <= z_max; ++z)
		{
			for(size_t y = y_min; y <= y_max; ++y)
			{
				// Get index of start of current data segment
				size_t index = DataCube_get_index(this, x_min, y, z);
				
				// Move file pointer to start of current data segment
				ensure(!fseek(fp, fp_start + index * this->word_size, SEEK_SET), "Error while reading FITS file.");
				
				// Read data segment into memory
				ensure(fread(ptr, this->word_size, region_nx, fp) == region_nx, "FITS file ended unexpectedly while reading data.");
				
				// Increment data pointer by segment size
				ptr += region_nx * this->word_size;
			}
		}
		
		// Update object properties
		// NOTE: This must happen after reading the sub-cube, as the full
		//       cube dimensions must be known during data extraction.
		this->data_size = region_size;
		this->axis_size[0] = region_nx;
		this->axis_size[1] = region_ny;
		this->axis_size[2] = region_nz;
		
		// Adjust WCS information in header
		if(DataCube_chkhd(this, "NAXIS1")) DataCube_puthd_int(this, "NAXIS1", region_nx);
		if(DataCube_chkhd(this, "NAXIS2")) DataCube_puthd_int(this, "NAXIS2", region_ny);
		if(DataCube_chkhd(this, "NAXIS3")) DataCube_puthd_int(this, "NAXIS3", region_nz);
		if(DataCube_chkhd(this, "CRPIX1")) DataCube_puthd_flt(this, "CRPIX1", DataCube_gethd_flt(this, "CRPIX1") - x_min);
		if(DataCube_chkhd(this, "CRPIX2")) DataCube_puthd_flt(this, "CRPIX2", DataCube_gethd_flt(this, "CRPIX2") - y_min);
		if(DataCube_chkhd(this, "CRPIX3")) DataCube_puthd_flt(this, "CRPIX3", DataCube_gethd_flt(this, "CRPIX3") - z_min);
	}
	
	// Swap byte order if required
	DataCube_swap_byte_order(this);
	
	// Close FITS file
	fclose(fp);
	
	return;
}



// ----------------------------------------------------------------- //
// Write data cube into FITS file                                    //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this      - Object self-reference.                          //
//   (2) filename  - Name of output FITS file.                       //
//   (3) overwrite - If true, overwrite existing file. Otherwise     //
//                   terminate if the file already exists.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for writing the current data cube object (re-     //
//   ferenced by *this) into a FITS file. The function will termi-   //
//   nate the current programme execution if an error is encountered //
//   during the write process. If the output file already exists, it //
//   will be overwritten only if overwrite is set to true.           //
// ----------------------------------------------------------------- //

public void DataCube_save(const DataCube *this, const char *filename, const bool overwrite)
{
	// Sanity checks
	check_null(this);
	check_null(filename);
	ensure(strlen(filename), "Empty file name provided.");
	
	// Open FITS file
	FILE *fp;
	if(overwrite) fp = fopen(filename, "wb");
	else fp = fopen(filename, "wxb");
	ensure(fp != NULL, "Failed to create new FITS file \'%s\'.\n       Does the file already exist?", filename);
	
	message("Creating FITS file \'%s\'.", filename);
	
	// Write entire header
	ensure(fwrite(this->header, 1, this->header_size, fp) == this->header_size, "Failed to write header to FITS file.");
	
	// Swap byte order of array in memory if necessary
	DataCube_swap_byte_order(this);
	
	// Write entire data array
	ensure(fwrite(this->data, this->word_size, this->data_size, fp) == this->data_size, "Failed to write data to FITS file.");
	
	// Fill file with 0x00 if necessary
	size_t size_footer = ((this->data_size * this->word_size) % FITS_HEADER_BLOCK_SIZE);
	const char footer = '\0';
	if(size_footer)
	{
		size_footer = FITS_HEADER_BLOCK_SIZE - size_footer;
		for(size_t counter = size_footer; counter--;) fwrite(&footer, 1, 1, fp);
	}
	
	// Revert to original byte order if necessary
	DataCube_swap_byte_order(this);
	
	return;
}



// ----------------------------------------------------------------- //
// Retrieve header element as raw string buffer                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this   - Object self-reference.                             //
//   (2) key    - Name of the header element to be retrieved.        //
//   (3) buffer - Pointer to char buffer for holding result.         //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 on success or 1 if the header keyword was not found.  //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for retrieving the specified header element as a //
//   raw string buffer. The resulting value will be written to the   //
//   char array pointed to by buffer, which needs to be large enough //
//   to hold the maximum permissible FITS header value size. If the  //
//   header keyword is not found, the buffer will remain unchanged   //
//   and a value of 1 will be returned by the function.              //
// ----------------------------------------------------------------- //

private int DataCube_gethd_raw(const DataCube *this, const char *key, char *buffer)
{
	// Sanity checks (done here, as the respective public methods all call this function)
	check_null(this);
	check_null(this->header);
	check_null(buffer);
	check_null(key);
	
	const char *ptr = this->header;
	
	while(ptr < this->header + this->header_size)
	{
		if(strncmp(ptr, key, strlen(key)) == 0)
		{
			memcpy(buffer, ptr + FITS_HEADER_KEY_SIZE, FITS_HEADER_VALUE_SIZE);
			buffer[FITS_HEADER_VALUE_SIZE] = '\0';
			return 0;
		}
		
		ptr += FITS_HEADER_LINE_SIZE;
	}
	
	warning("Header keyword \'%s\' not found.", key);
	return 1;
}



// ----------------------------------------------------------------- //
// Retrieve header element as bool, int or float                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this   - Object self-reference.                             //
//   (2) key    - Name of the header element to be retrieved.        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the requested header value as a Boolean, integer or     //
//   floating point value. If the header keyword was not found, then //
//   the return value will be false, 0 or NaN for bool, int, and     //
//   float types, respectively.                                      //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public methods for retrieving the specified header element as a //
//   Boolean, integer or floating-point value. These functions will  //
//   call DataCube_gethd_raw(); see there for more information.      //
// ----------------------------------------------------------------- //

public long int DataCube_gethd_int(const DataCube *this, const char *key)
{
	char buffer[FITS_HEADER_VALUE_SIZE + 1] = ""; // Note that "" initialises entire array with 0
	const int flag = DataCube_gethd_raw(this, key, buffer);
	return flag ? 0 : strtol(buffer, NULL, 10);
}

public double DataCube_gethd_flt(const DataCube *this, const char *key)
{
	char buffer[FITS_HEADER_VALUE_SIZE + 1] = "";
	const int flag = DataCube_gethd_raw(this, key, buffer);
	return flag ? NAN : strtod(buffer, NULL);
}

public bool DataCube_gethd_bool(const DataCube *this, const char *key)
{
	char buffer[FITS_HEADER_VALUE_SIZE + 1] = "";
	const int flag = DataCube_gethd_raw(this, key, buffer);
	
	if(!flag)
	{
		const char *ptr = buffer;
		while(ptr < buffer + FITS_HEADER_VALUE_SIZE + 1)
		{
			if(*ptr == ' ') ++ptr;
			else return *ptr == 'T';
		}
	}
	
	return false;
}



// ----------------------------------------------------------------- //
// Retrieve header element as string                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this   - Object self-reference.                             //
//   (2) key    - Name of the header element to be retrieved.        //
//   (3) value  - Pointer to char buffer for holding result.         //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 on success and 1 if the header keyword was not found. //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the specified header element as a  //
//   string. The string value will be written to the char array      //
//   pointed to by value, which will need to be large enough to hold //
//   the maximum permissible FITS header value size. This function   //
//   will call DataCube_gethd_raw(); see there for more information. //
// ----------------------------------------------------------------- //

public int DataCube_gethd_str(const DataCube *this, const char *key, char *value)
{
	// WARNING: This function will fail if there are quotation marks inside a comment!
	char buffer[FITS_HEADER_VALUE_SIZE + 1] =  "";
	const int flag = DataCube_gethd_raw(this, key, buffer);
	
	if(flag) return 1;
	
	const char *left = strchr(buffer, '\'');
	ensure(left != NULL, "FITS header entry is not a string.");
	
	const char *right = strchr(left + 1, '\'');
	while(right != NULL && *(right + 1) == '\'') right = strchr(right + 2, '\'');
	ensure(right != NULL, "Unbalanced quotation marks in FITS header entry.");
	
	memcpy(value, left + 1, right - left - 1);
	value[right - left - 1] = '\0';
	return 0;
}



// ----------------------------------------------------------------- //
// Write raw string to header                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this   - Object self-reference.                             //
//   (2) key    - Name of the header element to be written.          //
//   (3) buffer - Character buffer to be written to header.          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 if a new header entry was created and 1 if an         //
//   existing header entry was overwritten.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for writing a raw string buffer into the header. //
//   The buffer needs to of size FITS_HEADER_VALUE_SIZE and padded   //
//   with spaces (ASCII 32) at the end. If the specified keyword al- //
//   ready exists, its first occurrence will be overwritten with the //
//   new buffer. If the keyword does not exists, a new entry will be //
//   inserted at the end of the header just before the END keyword.  //
//   If necessary, the header size will be automatically adjusted to //
//   be able to accommodate the new entry.                           //
// ----------------------------------------------------------------- //

private int DataCube_puthd_raw(DataCube *this, const char *key, const char *buffer)
{
	// Sanity checks
	check_null(this);
	check_null(this->header);
	check_null(key);
	check_null(buffer);
	ensure(strlen(key) > 0 && strlen(key) <= FITS_HEADER_KEYWORD_SIZE, "Illegal length of header keyword.");
	
	char *ptr = this->header;
	size_t line = DataCube_chkhd(this, key);
	
	// Overwrite header entry if already present
	if(line > 0)
	{
		memcpy(ptr + (line - 1) * FITS_HEADER_LINE_SIZE + FITS_HEADER_KEY_SIZE, buffer, FITS_HEADER_VALUE_SIZE);
		return 0;
	}
	
	// Create a new entry
	warning("Header keyword \'%s\' not found. Creating new entry.", key);
	
	// Check current length
	line = DataCube_chkhd(this, "END");
	ensure(line > 0, "No END keyword found in header of DataCube object.");
	
	// Expand header if necessary
	if(line % FITS_HEADER_LINES == 0)
	{
		warning("Expanding header to fit new entry.");
		this->header_size += FITS_HEADER_BLOCK_SIZE;
		this->header = (char *)realloc(this->header, this->header_size);
		ensure(this->header != NULL, "Failed to reserve memory for FITS header.");
		memset(this->header + this->header_size - FITS_HEADER_BLOCK_SIZE, ' ', FITS_HEADER_BLOCK_SIZE); // fill with space
	}
	
	ptr = this->header;
	
	// Add new header keyword at end
	memcpy(ptr + (line - 1) * FITS_HEADER_LINE_SIZE, key, strlen(key)); // key
	memcpy(ptr + (line - 1) * FITS_HEADER_LINE_SIZE + FITS_HEADER_KEYWORD_SIZE, "=", 1); // =
	memcpy(ptr + (line - 1) * FITS_HEADER_LINE_SIZE + FITS_HEADER_KEY_SIZE, buffer, FITS_HEADER_VALUE_SIZE); // value
	memcpy(ptr + line * FITS_HEADER_LINE_SIZE, "END", 3); // new end
	
	return 1;
}



// ----------------------------------------------------------------- //
// Write bool, int, float or string value to header                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this   - Object self-reference.                             //
//   (2) key    - Name of the header element to be written.          //
//   (3) value  - Value to be written to header.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 if a new header entry was created and 1 if an         //
//   existing header entry was overwritten.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public methods for writing a bool, int, float or string value   //
//   into the header. All functions will call DataCube_puthd_raw();  //
//   see there for more information.                                 //
// ----------------------------------------------------------------- //

public int DataCube_puthd_int(DataCube *this, const char *key, const long int value)
{
	char buffer[FITS_HEADER_VALUE_SIZE];
	memset(buffer, ' ', FITS_HEADER_VALUE_SIZE);
	int size = snprintf(buffer, FITS_HEADER_FIXED_WIDTH + 1, "%20ld", value);
	ensure(size > 0 && size <= FITS_HEADER_FIXED_WIDTH, "Creation of new header entry failed for unknown reasons.");
	buffer[size] = ' '; // get rid of NUL character
	
	return DataCube_puthd_raw(this, key, buffer);
}

public int DataCube_puthd_flt(DataCube *this, const char *key, const double value)
{
	char buffer[FITS_HEADER_VALUE_SIZE];
	memset(buffer, ' ', FITS_HEADER_VALUE_SIZE);
	int size = snprintf(buffer, FITS_HEADER_FIXED_WIDTH + 1, "%20.11E", value);
	ensure(size > 0 && size <= FITS_HEADER_FIXED_WIDTH, "Creation of new header entry failed for unknown reasons.");
	buffer[size] = ' '; // get rid of NUL character
	
	return DataCube_puthd_raw(this, key, buffer);
}

public int DataCube_puthd_bool(DataCube *this, const char *key, const bool value)
{
	char buffer[FITS_HEADER_VALUE_SIZE];
	memset(buffer, ' ', FITS_HEADER_VALUE_SIZE);
	buffer[FITS_HEADER_FIXED_WIDTH - 1] = value ? 'T' : 'F';
	
	return DataCube_puthd_raw(this, key, buffer);
}

public int DataCube_puthd_str(DataCube *this, const char *key, const char *value)
{
	const size_t size = strlen(value);
	ensure(size <= FITS_HEADER_VALUE_SIZE - 2, "String too long for FITS header line.");
	char buffer[FITS_HEADER_VALUE_SIZE];
	memset(buffer, ' ', FITS_HEADER_VALUE_SIZE);
	memcpy(buffer, "\'", 1); // opening quotation mark
	memcpy(buffer + 1, value, size); // string
	memcpy(buffer + 1 + size, "\'", 1); // closing quotation mark
	
	return DataCube_puthd_raw(this, key, buffer);
}



// ----------------------------------------------------------------- //
// Check for header keyword                                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this - Object self-reference.                               //
//   (2) key  - Name of the header element to be checked for.        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Line number of the first occurrence of the specified key in the //
//   header. If the key was not found, 0 is returned.                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Searches for the first occurrence of the specified header key-  //
//   word and returns the corresponding line number. If the header   //
//   keyword is not found, the function will return 0.               //
// ----------------------------------------------------------------- //

public size_t DataCube_chkhd(const DataCube *this, const char *key)
{
	// Sanity checks
	check_null(this);
	check_null(this->header);
	check_null(key);
	const size_t size = strlen(key);
	ensure(size > 0 && size <= FITS_HEADER_KEYWORD_SIZE, "Illegal FITS header keyword: %s.", key);
	
	char *ptr = this->header;
	size_t line = 1;
	
	while(ptr < this->header + this->header_size)
	{
		if(strncmp(ptr, key, size) == 0 && (*(ptr + size) == ' ' || *(ptr + size) == '=')) return line;
		ptr += FITS_HEADER_LINE_SIZE;
		++line;
	}
	
	warning("Header keyword \'%s\' not found.", key);
	return 0;
}



// ----------------------------------------------------------------- //
// Delete header keyword                                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this - Object self-reference.                               //
//   (2) key  - Name of the header element to be deleted.            //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 1 if the header keyword was not found, 0 otherwise.     //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Deletes all occurrences of the specified header keyword. Any    //
//   empty blocks at the end of the new header will be removed.      //
// ----------------------------------------------------------------- //

public int DataCube_delhd(DataCube *this, const char *key)
{
	size_t line = DataCube_chkhd(this, key);
	if(line == 0) return 1;
	
	// Header keyword found; shift all subsequent lines up by 1
	// and fill last line with spaces. Do this repeatedly until
	// the keyword is no longer found.
	while(line)
	{
		memmove(this->header + (line - 1) * FITS_HEADER_LINE_SIZE, this->header + line * FITS_HEADER_LINE_SIZE, this->header_size - line * FITS_HEADER_LINE_SIZE);
		memset(this->header + this->header_size - FITS_HEADER_LINE_SIZE, ' ', FITS_HEADER_LINE_SIZE);
		line = DataCube_chkhd(this, key);
	}
	
	// Check if the header block can be shortened.
	line = DataCube_chkhd(this, "END");
	ensure(line, "END keyword missing from FITS header.");
	const size_t last_line = this->header_size / FITS_HEADER_LINE_SIZE;
	const size_t empty_blocks = (last_line - line) / FITS_HEADER_LINES;
	
	if(empty_blocks)
	{
		warning("Reducing size of header to remove empty block(s).");
		this->header_size -= empty_blocks * FITS_HEADER_BLOCK_SIZE;
		this->header = (char *)realloc(this->header, this->header_size);
		ensure(this->header != NULL, "Failed to reserve memory for FITS header.");
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Read data value as double-precision floating-point number         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this - Object self-reference.                               //
//   (2) x    - First coordinate.                                    //
//   (3) y    - Second coordinate.                                   //
//   (4) z    - Third coordinate.                                    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the value of the data array at the given position as a  //
//   double-precision floating-point value.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method to extract the data value at the specified posi-  //
//   tion (x, y, z), where x indexes the first axis, y the second    //
//   axis and z the third axis of the cube. The function will return //
//   the result as a double-precision floating-point value irrespec- //
//   tive of the native data type of the FITS file.                  //
// ----------------------------------------------------------------- //

public double DataCube_get_data_flt(const DataCube *this, const size_t x, const size_t y, const size_t z)
{
	check_null(this);
	check_null(this->data);
	ensure(x < this->axis_size[0] && y < this->axis_size[1] && z < this->axis_size[2], "Position (%zu, %zu, %zu) outside of image boundaries.", x, y, z);
	const size_t i = DataCube_get_index(this, x, y, z);
	
	switch(this->data_type)
	{
		case -64:
			return *((double *)(this->data + i * this->word_size));
		case -32:
			return (double)(*((float *)(this->data + i * this->word_size)));
		case 8:
			return (double)(*((uint8_t *)(this->data + i * this->word_size)));
		case 16:
			return (double)(*((int16_t *)(this->data + i * this->word_size)));
		case 32:
			return (double)(*((int32_t *)(this->data + i * this->word_size)));
		case 64:
			return (double)(*((int64_t *)(this->data + i * this->word_size)));
	}
	
	return NAN;
}



// ----------------------------------------------------------------- //
// Read data value as long integer number                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this - Object self-reference.                               //
//   (2) x    - First coordinate.                                    //
//   (3) y    - Second coordinate.                                   //
//   (4) z    - Third coordinate.                                    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the value of the data array at the given position as a  //
//   long integer value.                                             //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method to extract the data value at the specified posi-  //
//   tion (x, y, z), where x indexes the first axis, y the second    //
//   axis and z the third axis of the cube. The function will return //
//   the result as a long integer value irrespective of the native   //
//   data type of the FITS file.                                     //
// ----------------------------------------------------------------- //

public long int DataCube_get_data_int(const DataCube *this, const size_t x, const size_t y, const size_t z)
{
	check_null(this);
	check_null(this->data);
	ensure(x < this->axis_size[0] && y < this->axis_size[1] && z < this->axis_size[2], "Position (%zu, %zu, %zu) outside of image boundaries.", x, y, z);
	const size_t i = DataCube_get_index(this, x, y, z);
	
	switch(this->data_type)
	{
		case -64:
			return (long int)(*((double *)(this->data + i * this->word_size)));
		case -32:
			return (long int)(*((float *)(this->data + i * this->word_size)));
		case 8:
			return (long int)(*((uint8_t *)(this->data + i * this->word_size)));
		case 16:
			return (long int)(*((int16_t *)(this->data + i * this->word_size)));
		case 32:
			return (long int)(*((int32_t *)(this->data + i * this->word_size)));
		case 64:
			return (long int)(*((int64_t *)(this->data + i * this->word_size)));
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Set data value as double-precision floating-point number          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this  - Object self-reference.                              //
//   (2) x     - First coordinate.                                   //
//   (3) y     - Second coordinate.                                  //
//   (4) z     - Third coordinate.                                   //
//   (4) value - Data value to be written to array.                  //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method to write the data value to the specified position //
//   (x, y, z), where x indexes the first axis, y the second axis    //
//   and z the third axis of the cube. Note that the data value will //
//   be cast to the native data type of the array before being writ- //
//   ten.                                                            //
// ----------------------------------------------------------------- //

public void DataCube_set_data_flt(DataCube *this, const size_t x, const size_t y, const size_t z, const double value)
{
	check_null(this);
	check_null(this->data);
	ensure(x < this->axis_size[0] && y < this->axis_size[1] && z < this->axis_size[2], "Position outside of image boundaries.");
	const size_t i = DataCube_get_index(this, x, y, z);
	
	switch(this->data_type)
	{
		case -64:
			*((double *)(this->data + i * this->word_size))  = value;
			break;
		case -32:
			*((float *)(this->data + i * this->word_size))   = (float)value;
			break;
		case 8:
			*((uint8_t *)(this->data + i * this->word_size)) = (uint8_t)value;
			break;
		case 16:
			*((int16_t *)(this->data + i * this->word_size)) = (int16_t)value;
			break;
		case 32:
			*((int32_t *)(this->data + i * this->word_size)) = (int32_t)value;
			break;
		case 64:
			*((int64_t *)(this->data + i * this->word_size)) = (int64_t)value;
			break;
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Set data value as long integer number                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this  - Object self-reference.                              //
//   (2) x     - First coordinate.                                   //
//   (3) y     - Second coordinate.                                  //
//   (4) z     - Third coordinate.                                   //
//   (5) value - Data value to be written to array.                  //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method to write the data value to the specified position //
//   (x, y, z), where x indexes the first axis, y the second axis    //
//   and z the third axis of the cube. Note that the data value will //
//   be cast to the native data type of the array before being writ- //
//   ten.                                                            //
// ----------------------------------------------------------------- //

public void DataCube_set_data_int(DataCube *this, const size_t x, const size_t y, const size_t z, const long int value)
{
	check_null(this);
	check_null(this->data);
	ensure(x < this->axis_size[0] && y < this->axis_size[1] && z < this->axis_size[2], "Position outside of image boundaries.");
	const size_t i = DataCube_get_index(this, x, y, z);
	
	switch(this->data_type) {
		case -64:
			*((double *)(this->data + i * this->word_size))  = (double)value;
			break;
		case -32:
			*((float *)(this->data + i * this->word_size))   = (float)value;
			break;
		case 8:
			*((uint8_t *)(this->data + i * this->word_size)) = (uint8_t)value;
			break;
		case 16:
			*((int16_t *)(this->data + i * this->word_size)) = (int16_t)value;
			break;
		case 32:
			*((int32_t *)(this->data + i * this->word_size)) = (int32_t)value;
			break;
		case 64:
			*((int64_t *)(this->data + i * this->word_size)) = (int64_t)value;
			break;
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Calculate the standard deviation about a value                    //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this    - Object self-reference.                            //
//   (2) value   - Value about which to calculate the standard       //
//                 deviation.                                        //
//   (3) cadence - Cadence used in the calculation, i.e. a cadence   //
//                 of N will calculate the standard deviation using  //
//                 every N-th element from the array.                //
//   (4) range   - Flux range to be used in the calculation. Options //
//                 are 0 (entire flux range), -1 (negative fluxes    //
//                 only) and +1 (positive fluxes only).              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Standard deviation about the specified value.                   //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for calculating the standard deviation of the     //
//   data array about a specified value. The cadence specifies which //
//   fraction of the elements in the array will be used in the cal-  //
//   culation; it can be set to > 1 for large arrays in order to     //
//   reduce the processing time of the algorithm. The range defines  //
//   the flux range to be used; if set to 0, all pixels will be      //
//   used, whereas a positive or negative value indicates that only  //
//   positive or negative pixels should be used, respectively. This  //
//   is useful for increasing the robustness of the standard devia-  //
//   tion in the presence of negative or positive flux or artefacts  //
//   in the data.                                                    //
// ----------------------------------------------------------------- //

public double DataCube_stat_std(const DataCube *this, const double value, const size_t cadence, const int range)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Cannot evaluate standard deviation for integer array.");
	
	if(this->data_type == -32) return std_dev_val_flt((float *)this->data, this->data_size, value, cadence ? cadence : 1, range);
	else return std_dev_val_dbl((double *)this->data, this->data_size, value, cadence ? cadence : 1, range);
}



// ----------------------------------------------------------------- //
// Calculate the median absolute deviation of the array              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this    - Object self-reference.                            //
//   (2) value   - Value relative to which to calculate the MAD.     //
//   (3) cadence - Cadence used in the calculation, i.e. a cadence   //
//                 of N will calculate the standard deviation using  //
//                 every N-th element from the array.                //
//   (4) range   - Flux range to be used in the calculation. Options //
//                 are 0 (entire flux range), -1 (negative fluxes    //
//                 only) and +1 (positive fluxes only).              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Median absolute deviation of the data array.                    //
//   were found.                                                     //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for calculating the median absolute deviation re- //
//   lative to the specified value. NOTE that a copy of (parts of)   //
//   the data array will need to be made in order to calculate the   //
//   median of the data as part of this process.                     //
// ----------------------------------------------------------------- //

public double DataCube_stat_mad(const DataCube *this, const double value, const size_t cadence, const int range)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Cannot evaluate MAD for integer array.");
	
	// Derive MAD of data copy
	if(this->data_type == -32) return mad_val_flt((float *)this->data, this->data_size, value, cadence, range);
	return mad_val_dbl((double *)this->data, this->data_size, value, cadence, range);
}



// ----------------------------------------------------------------- //
// Calculate the noise via Gaussian fitting to flux histogram        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this    - Object self-reference.                            //
//   (2) cadence - Cadence used in the calculation, i.e. a cadence   //
//                 of N will calculate the flux histogram using      //
//                 every N-th element from the array.                //
//   (3) range   - Flux range to be used in the calculation. Options //
//                 are 0 (entire flux range), -1 (negative fluxes    //
//                 only) and +1 (positive fluxes only).              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Standard deviation of the Gaussian fitted to the histogram.     //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for determining the noise level in the data array //
//   by fitting a Gaussian function to the flux histogram and deter- //
//   mining the standard deviation of that Gaussian. The cadence     //
//   specifies which fraction of the elements in the array will be   //
//   used in the calculation; it can be set to > 1 for large arrays  //
//   in order to reduce the processing time of the algorithm. The    //
//   range defines the flux range to be used; if set to 0, all pix-  //
//   els will be used, whereas a positive or negative value indi-    //
//   cates that only positive or negative pixels should be used, re- //
//   spectively. This is useful for increasing the robustness of the //
//   standard deviation in the presence of negative or positive flux //
//   or artefacts in the data.                                       //
// ----------------------------------------------------------------- //

public double DataCube_stat_gauss(const DataCube *this, const size_t cadence, const int range)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Cannot evaluate standard deviation for integer array.");
	
	if(this->data_type == -32) return gaufit_flt((float *)this->data, this->data_size, cadence ? cadence : 1, range);
	else return gaufit_dbl((double *)this->data, this->data_size, cadence ? cadence : 1, range);
}



// ----------------------------------------------------------------- //
// Global noise scaling along spectral axis                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this      - Object self-reference.                          //
//   (2) statistic - Statistic to use in noise measurement. Can be   //
//                   NOISE_STAT_STD for standard deviation,          //
//                   NOISE_STAT_MAD for median absolute deviation or //
//                   NOISE_STAT_GAUSS for Gaussian fitting to the    //
//                   flux histogram.                                 //
//   (3) range     - Flux range to be used in noise measurement. Can //
//                   be -1, 0 or +1 for negative range, full range   //
//                   or positive range, respectively.                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for dividing the data cube by the global noise    //
//   level as a function of frequency as measured in each spatial    //
//   plane of the cube. The statistic and flux range used in the     //
//   noise measurement can be selected to ensure a robust noise mea- //
//   surement. This method should be applied prior to source finding //
//   on data cubes where the noise level varies with frequency, but  //
//   is constant along the two spatial axes in each channel.         //
// ----------------------------------------------------------------- //

public void DataCube_scale_noise_spec(const DataCube *this, const noise_stat statistic, const int range)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Cannot run noise scaling on integer array.");
	
	// A few settings
	const size_t size_xy = this->axis_size[0] * this->axis_size[1];
	const size_t size_z  = this->axis_size[2];
	const size_t size    = size_xy * size_z;
	double rms;
	float  *ptr_flt = (float *)this->data;
	double *ptr_dbl = (double *)this->data;
	
	// Scaling along spectral axis
	if(this->data_type == -32)
	{
		while(ptr_flt < (float *)(this->data) + size)
		{
			if(statistic == NOISE_STAT_STD) rms = std_dev_val_flt(ptr_flt, size_xy, 0.0, 1, range);
			else if(statistic == NOISE_STAT_MAD) rms = MAD_TO_STD * mad_val_flt(ptr_flt, size_xy, 0.0, 1, range);
			else rms = gaufit_flt(ptr_flt, size_xy, 1, range);
			
			float *ptr2 = ptr_flt;
			while(ptr2 < ptr_flt + size_xy)
			{
				*ptr2 /= rms;
				++ptr2;
			}
			
			ptr_flt += size_xy;
		}
	}
	else
	{
		while(ptr_dbl < (double *)(this->data) + size)
		{
			if(statistic == NOISE_STAT_STD) rms = std_dev_val_dbl(ptr_dbl, size_xy, 0.0, 1, range);
			else if(statistic == NOISE_STAT_MAD) rms = MAD_TO_STD * mad_val_dbl(ptr_dbl, size_xy, 0.0, 1, range);
			else rms = gaufit_dbl(ptr_dbl, size_xy, 1, range);
			
			double *ptr2 = ptr_dbl;
			while(ptr2 < ptr_dbl + size_xy)
			{
				*ptr2 /= rms;
				++ptr2;
			}
			
			ptr_dbl += size_xy;
		}
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Apply boxcar filter to spectral axis                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this    - Object self-reference.                            //
//   (2) radius  - Filter radius in channels.                        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for convolving each spectrum of the data cube     //
//   with a boxcar filter of size 2 * radius + 1. The algorithm is   //
//   NaN-safe by setting all NaN values to 0 prior to filtering. Any //
//   pixel outside of the cube's spectral range is also assumed to   //
//   be 0.                                                           //
// ----------------------------------------------------------------- //

public void DataCube_boxcar_filter(DataCube *this, size_t radius)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Cannot run boxcar filter on integer array.");
	if(radius < 1) radius = 1;
	
	// Allocate memory for a single spectrum
	char *spectrum = (char *)malloc(this->axis_size[2] * this->word_size * sizeof(char));
	ensure(spectrum != NULL, "Memory allocation for boxcar filter failed.");
	
	// Request memory for boxcar filter to operate on
	float *data_box_flt = NULL;
	double *data_box_dbl = NULL;
	if(this->data_type == -32)
	{
		data_box_flt = (float *)malloc((this->axis_size[2] + 2 * radius) * sizeof(float));
		ensure(data_box_flt != NULL, "Memory allocation for boxcar filter failed.");
	}
	else
	{
		data_box_dbl = (double *)malloc((this->axis_size[2] + 2 * radius) * sizeof(double));
		ensure(data_box_dbl != NULL, "Memory allocation for boxcar filter failed.");
	}
	
	for(size_t x = this->axis_size[0]; x--;)
	{
		for(size_t y = this->axis_size[1]; y--;)
		{
			// Extract spectrum
			for(size_t z = this->axis_size[2]; z--;)
			{
				memcpy(spectrum + z * this->word_size, this->data + DataCube_get_index(this, x, y, z) * this->word_size, this->word_size);
			}
			
			// Apply filter
			if(this->data_type == -32)
			{
				filter_boxcar_1d_flt((float *)spectrum, data_box_flt, this->axis_size[2], radius, contains_nan_flt((float *)spectrum, this->axis_size[2]));
			}
			else
			{
				filter_boxcar_1d_dbl((double *)spectrum, data_box_dbl, this->axis_size[2], radius, contains_nan_dbl((double *)spectrum, this->axis_size[2]));
			}
			
			// Copy filtered spectrum back into array
			for(size_t z = 0; z < this->axis_size[2]; ++z)
			{
				memcpy(this->data + DataCube_get_index(this, x, y, z) * this->word_size, spectrum + z * this->word_size, this->word_size);
			}
		}
	}
	
	// Release memory
	free(spectrum);
	free(data_box_flt);
	free(data_box_dbl);
	
	return;
}



// ----------------------------------------------------------------- //
// Apply 2D Gaussian filter to spatial planes                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this    - Object self-reference.                            //
//   (2) sigma   - Standard deviation of the Gaussian in pixels.     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for convolving each spatial image plane (x-y) of  //
//   the data cube with a Gaussian function of standard deviation    //
//   sigma. The Gaussian convolution is approximated through a set   //
//   of 1D boxcar filters, which makes the algorithm extremely fast. //
//   Limitations from this approach are that the resulting convolu-  //
//   tion kernel is only an approximation of a Gaussian (although a  //
//   fairly accurate one) and the value of sigma can only be appro-  //
//   ximated (typically within +/- 0.2 sigma) and must be at least   //
//   1.5 pixels.                                                     //
//   The algorithm is NaN-safe by setting all NaN values to 0. Any   //
//   pixel outside of the image boundaries is also assumed to be 0.  //
// ----------------------------------------------------------------- //

public void DataCube_gaussian_filter(DataCube *this, const double sigma)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Cannot run boxcar filter on integer array.");
	
	// Set up parameters required for boxcar filter
	size_t n_iter;
	size_t filter_radius;
	optimal_filter_size_dbl(sigma, &filter_radius, &n_iter);
	
	// Request memory for one column
	float  *column_flt = NULL;
	double *column_dbl = NULL;
	
	// Request memory for boxcar filter to operate on
	float  *data_row_flt = NULL;
	float  *data_col_flt = NULL;
	double *data_row_dbl = NULL;
	double *data_col_dbl = NULL;
	
	if(this->data_type == -32)
	{
		data_row_flt = (float *)malloc((this->axis_size[0] + 2 * filter_radius) * sizeof(float));
		data_col_flt = (float *)malloc((this->axis_size[1] + 2 * filter_radius) * sizeof(float));
		column_flt   = (float *)malloc(this->axis_size[1] * sizeof(float));
		ensure(data_row_flt != NULL && data_col_flt != NULL, "Memory allocation error in Gaussian filter.");
	}
	else
	{
		data_row_dbl = (double *)malloc((this->axis_size[0] + 2 * filter_radius) * sizeof(double));
		data_col_dbl = (double *)malloc((this->axis_size[1] + 2 * filter_radius) * sizeof(double));
		column_dbl   = (double *)malloc(this->axis_size[1] * sizeof(double));
		ensure(data_row_dbl != NULL && data_col_dbl != NULL, "Memory allocation error in Gaussian filter.");
	}
	
	// NOTE: We don't need to extract a copy of each image plane, as
	//       x-y planes are contiguous in memory.
	char *ptr = this->data + this->data_size * this->word_size;
	const size_t size_1 = this->axis_size[0] * this->axis_size[1];
	const size_t size_2 = size_1 * this->word_size;
	
	// Apply filter
	while(ptr > this->data)
	{
		ptr -= size_2;
		if(this->data_type == -32)
		{
			filter_gauss_2d_flt((float *)ptr, column_flt, data_row_flt, data_col_flt, this->axis_size[0], this->axis_size[1], n_iter, filter_radius, contains_nan_flt((float *)ptr, size_1));
		}
		else
		{
			filter_gauss_2d_dbl((double *)ptr, column_dbl, data_row_dbl, data_col_dbl, this->axis_size[0], this->axis_size[1], n_iter, filter_radius, contains_nan_dbl((double *)ptr, size_1));
		}
	}
	
	// Release memory
	free(data_row_flt);
	free(data_col_flt);
	free(data_row_dbl);
	free(data_col_dbl);
	free(column_flt);
	free(column_dbl);
	
	return;
}



// ----------------------------------------------------------------- //
// Mask pixels of abs(value) > threshold                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this      - Object self-reference.                          //
//   (2) maskCube  - Pointer to mask cube.                           //
//   (3) threshold - Flux threshold for masking operation.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 on success, 1 otherwise.                              //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting pixels in the mask cube to 1 when     //
//   their absolute value in the data cube is greater than the spe-  //
//   cified threshold.                                               //
// ----------------------------------------------------------------- //

public int DataCube_mask(const DataCube *this, DataCube *maskCube, const double threshold)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	check_null(maskCube);
	check_null(maskCube->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Data cube must be of floating-point type.");
	ensure(maskCube->data_type == 8 || maskCube->data_type == 16 || maskCube->data_type == 32 || maskCube->data_type == 64, "Mask cube must be of integer type.");
	ensure(this->axis_size[0] == maskCube->axis_size[0] && this->axis_size[1] == maskCube->axis_size[1] && this->axis_size[2] == maskCube->axis_size[2], "Data cube and mask cube have different sizes.");
	ensure(threshold > 0.0, "Negative threshold provided.");
	
	// Declaration of variables
	char *ptr_data = this->data + this->data_size * this->word_size;
	char *ptr_mask = maskCube->data + maskCube->data_size * maskCube->word_size;
	const float  thresh_flt_pos =  threshold;
	const float  thresh_flt_neg = -threshold;
	const double thresh_dbl_pos =  threshold;
	const double thresh_dbl_neg = -threshold;
	float  *ptr_flt;
	double *ptr_dbl;
	
	while(ptr_data > this->data)
	{
		ptr_data -= this->word_size;
		ptr_mask -= maskCube->word_size;
		
		if(this->data_type == -32)
		{
			ptr_flt = (float *)ptr_data;
			if(*ptr_flt > thresh_flt_pos || *ptr_flt < thresh_flt_neg)
			{
				if     (maskCube->data_type ==  8) *((uint8_t *)ptr_mask) = 1;
				else if(maskCube->data_type == 16) *((int16_t *)ptr_mask) = 1;
				else if(maskCube->data_type == 32) *((int32_t *)ptr_mask) = 1;
				else if(maskCube->data_type == 64) *((int64_t *)ptr_mask) = 1;
			}
		}
		else
		{
			ptr_dbl = (double *)ptr_data;
			if(*ptr_dbl > thresh_dbl_pos || *ptr_dbl < thresh_dbl_neg)
			{
				if     (maskCube->data_type ==  8) *((uint8_t *)ptr_mask) = 1;
				else if(maskCube->data_type == 16) *((int16_t *)ptr_mask) = 1;
				else if(maskCube->data_type == 32) *((int32_t *)ptr_mask) = 1;
				else if(maskCube->data_type == 64) *((int64_t *)ptr_mask) = 1;
			}
		}
	}
	
	return 0;
}

/* Same, but for 32-bit mask cubes (faster) */

public int DataCube_mask_32(const DataCube *this, DataCube *maskCube, const double threshold)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	check_null(maskCube);
	check_null(maskCube->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Data cube must be of floating-point type.");
	ensure(maskCube->data_type == 32, "Mask cube must be of 32-bit integer type.");
	ensure(this->axis_size[0] == maskCube->axis_size[0] && this->axis_size[1] == maskCube->axis_size[1] && this->axis_size[2] == maskCube->axis_size[2], "Data cube and mask cube have different sizes.");
	ensure(threshold > 0.0, "Threshold must be positive.");
	
	if(this->data_type == -32)
	{
		const float *ptr      = (float *)(this->data);
		const float *ptr_data = (float *)(this->data) + this->data_size;
		int32_t     *ptr_mask = (int32_t *)(maskCube->data) + maskCube->data_size;
		const float  thresh_p = threshold;
		const float  thresh_n = -threshold;
		
		while(ptr_data --> ptr)
		{
			--ptr_mask;
			if(*ptr_data > thresh_p || *ptr_data < thresh_n) *ptr_mask = 1;
		}
	}
	else
	{
		const double *ptr      = (double *)(this->data);
		const double *ptr_data = (double *)(this->data) + this->data_size;
		int32_t      *ptr_mask = (int32_t *)(maskCube->data) + maskCube->data_size;
		const double  thresh_p = threshold;
		const double  thresh_n = -threshold;
		
		while(ptr_data --> ptr)
		{
			--ptr_mask;
			if(*ptr_data > thresh_p || *ptr_data < thresh_n) *ptr_mask = 1;
		}
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Set masked pixels to constant value                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this      - Object self-reference.                          //
//   (2) maskCube  - Pointer to mask cube.                           //
//   (3) value     - Flux value to replace pixels with.              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 on success, 1 otherwise.                              //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for replacing the values of all pixels in the     //
//   data cube that have their first bit set in the mask cube to     //
//   their signum multiplied by the specified value.                 //
// ----------------------------------------------------------------- //

public int DataCube_set_masked(DataCube *this, const DataCube *maskCube, const double value)
{
	check_null(this);
	check_null(this->data);
	check_null(maskCube);
	check_null(maskCube->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Data cube must be of floating-point type.");
	ensure(maskCube->data_type == 8 || maskCube->data_type == 16 || maskCube->data_type == 32 || maskCube->data_type == 64, "Mask cube must be of integer type.");
	ensure(this->axis_size[0] == maskCube->axis_size[0] && this->axis_size[1] == maskCube->axis_size[1] && this->axis_size[2] == maskCube->axis_size[2], "Data cube and mask cube have different sizes.");
	
	char *ptr_data = this->data + this->data_size * this->word_size;
	char *ptr_mask = maskCube->data + maskCube->data_size * maskCube->word_size;
	const float  value_flt =  value;
	float *ptr_flt;
	double *ptr_dbl;
	const int mask_type = maskCube->data_type;
	
	while(ptr_data > this->data)
	{
		ptr_data -= this->word_size;
		ptr_mask -= maskCube->word_size;
		
		if((mask_type == 8 && *((uint8_t *)ptr_mask)) || (mask_type == 16 && *((int16_t *)ptr_mask)) || (mask_type == 32 && *((int32_t *)ptr_mask)) || (mask_type == 64 && *((int64_t *)ptr_mask)))
		{
			if(this->data_type == -32)
			{
				ptr_flt  = (float *)ptr_data;
				*ptr_flt = copysign(value_flt, *ptr_flt);
			}
			else
			{
				ptr_dbl  = (double *)ptr_data;
				*ptr_dbl = copysign(value, *ptr_dbl);
			}
		}
	}
	
	return 0;
}

// Same, but for 32-bit mask cube (faster) //

public int DataCube_set_masked_32(DataCube *this, const DataCube *maskCube, const double value)
{
	check_null(this);
	check_null(this->data);
	check_null(maskCube);
	check_null(maskCube->data);
	ensure(this->data_type == -32 || this->data_type == -64, "Data cube must be of floating-point type.");
	ensure(maskCube->data_type == 32, "Mask cube must be of 32-bit integer type.");
	ensure(this->axis_size[0] == maskCube->axis_size[0] && this->axis_size[1] == maskCube->axis_size[1] && this->axis_size[2] == maskCube->axis_size[2], "Data cube and mask cube have different sizes.");
	
	if(this->data_type == -32)
	{
		const float   *ptr      = (float *)(this->data);
		float         *ptr_data = (float *)(this->data) + this->data_size;
		const int32_t *ptr_mask = (int32_t *)(maskCube->data) + maskCube->data_size;
		
		while(ptr_data --> ptr)
		{
			--ptr_mask;
			if(*ptr_mask) *ptr_data = copysign(value, *ptr_data);
		}
	}
	else
	{
		const double  *ptr      = (double *)(this->data);
		double        *ptr_data = (double *)(this->data) + this->data_size;
		const int32_t *ptr_mask = (int32_t *)(maskCube->data) + maskCube->data_size;
		
		while(ptr_data --> ptr)
		{
			--ptr_mask;
			if(*ptr_mask) *ptr_data = copysign(value, *ptr_data);
		}
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Return array index from x, y and z                                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this - Object self-reference.                               //
//   (2) x    - First coordinate.                                    //
//   (3) y    - Second coordinate.                                   //
//   (4) z    - Third coordinate.                                    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the 1-D array index corresponding to the 3 coordinate   //
//   values specified under the assumption that the cube is 3-D.     //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method to turn a 3-D pixel coordinate into a 1-D array  //
//   index position under the fundamental assumption that the cube   //
//   is three-dimensional. Note that this function will still work   //
//   for 2-D arrays by simply setting z = 0 and the size of the      //
//   third axis to 1 (likewise for 1-D arrays).                      //
// ----------------------------------------------------------------- //

private inline size_t DataCube_get_index(const DataCube *this, const size_t x, const size_t y, const size_t z)
{
	return x + this->axis_size[0] * (y + this->axis_size[1] * z);
}



// ----------------------------------------------------------------- //
// Run Smooth + Clip (S+C) finder on data cube                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this         - Data cube to run the S+C finder on.          //
//   (2) kernels_spat - List of spatial smoothing lengths correspon- //
//                      ding to the FWHM of the Gaussian kernels to  //
//                      be applied; 0 = no smoothing.                //
//   (3) kernels_spec - List of spectral smoothing lengths corre-    //
//                      sponding to the widths of the boxcar filters //
//                      to be applied. Must be odd or 0.             //
//   (4) threshold    - Relative flux threshold to be applied.       //
//   (5) maskScaleXY  - Already detected pixels will be set to this  //
//                      value times the original rms of the data be- //
//                      fore smoothing the data again.               //
//   (6) method        - Method to use for measuring the noise in    //
//                      the smoothed copies of the cube; can be      //
//                      NOISE_STAT_STD, NOISE_STAT_MAD or        //
//                      NOISE_STAT_GAUSS for standard deviation,   //
//                      median absolute deviation and Gaussian fit   //
//                      to flux histogram, respectively.             //
//   (7) range        - Flux range to used in noise measurement, Can //
//                      be -1, 0 or 1 for negative only, all or po-  //
//                      sitive only.                                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns a pointer to a 32-bit integer mask cube where all de-   //
//   tected pixels are marked as 1 while background pixels are 0.    //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for running the Smooth + Clip (S+C) finder on the //
//   specified data cube. The S+C finder will smooth the data on the //
//   specified spatial and spectral scales, applying a Gaussian fil- //
//   ter in the spatial domain and a boxcar filter in the spectral   //
//   domain. It will then measure the noise level in each iteration  //
//   and mark all pixels with absolute values greater than or equal  //
//   to the specified threshold (relative to the noise level) as 1   //
//   in a 32-bit integer mask, while non-detected pixels will be set //
//   to a value of 0. Pixels already detected in a previous iter-    //
//   ation will be set to maskScaleXY times the original rms noise   //
//   level of the data before smoothing. A pointer to the final mask //
//   cube will be returned.                                          //
//   The input data cube must be a 32 or 64 bit floating point data  //
//   array. The spatial kernel sizes must be positive floating point //
//   values that represent the FWHM of the Gaussian kernels to be    //
//   applied in the spatial domain. The spectral kernel sizes must   //
//   be positive, odd integer numbers representing the widths of the //
//   boxcar filters to be applied in the spectral domain. The thre-  //
//   shold is relative to the noise level and should be a floating   //
//   point number greater than about 3.0. Lastly, the value of       //
//   maskScaleXY times the original rms of the data will be used to  //
//   replace pixels in the data cube that were already detected in a //
//   previous iteration. This is to ensure that any sources in the   //
//   data will not be smeared out beyond the extent of the source    //
//   when convolving with large kernel sizes.                        //
//   Several methods are available for measuring the noise in the    //
//   data cube, including the standard deviation, median absolute    //
//   deviation and a Gaussian fit to the flux histogram. These dif-  //
//   fer in their speed and robustness. In addition, the flux range  //
//   used in the noise measurement can be restricted to negative or  //
//   positive pixels only to reduce the impact or actual emission or //
//   absorption featured on the noise measurement.                   //
// ----------------------------------------------------------------- //

public DataCube *DataCube_run_scfind(const DataCube *this, const Array *kernels_spat, const Array *kernels_spec, const double threshold, const double maskScaleXY, const noise_stat method, const int range)
{
	// Sanity checks
	check_null(this);
	ensure(this->data_type < 0, "The S+C finder can only be applied to floating-point data.");
	check_null(kernels_spat);
	check_null(kernels_spec);
	ensure(Array_get_size(kernels_spat) && Array_get_size(kernels_spec), "Invalid spatial or spectral kernel list encountered.");
	ensure(threshold >= 0.0, "Negative flux threshold encountered.");
	ensure(method == NOISE_STAT_STD || method == NOISE_STAT_MAD || method == NOISE_STAT_GAUSS, "Invalid noise measurement method: %d.", method);
	
	// Create mask cube
	size_t nx = this->axis_size[0];
	size_t ny = this->axis_size[1];
	size_t nz = this->axis_size[2];
	DataCube *maskCube = DataCube_blank(nx, ny, nz, 32);
	
	// Copy WCS header elements from data cube to mask cube
	char value[FITS_HEADER_VALUE_SIZE];
	if(DataCube_chkhd(this, "CTYPE1"))
	{
		DataCube_gethd_str(this, "CTYPE1", value);
		DataCube_puthd_str(maskCube, "CTYPE1", value);
	}
	if(DataCube_chkhd(this, "CTYPE2"))
	{
		DataCube_gethd_str(this, "CTYPE2", value);
		DataCube_puthd_str(maskCube, "CTYPE2", value);
	}
	if(DataCube_chkhd(this, "CTYPE3"))
	{
		DataCube_gethd_str(this, "CTYPE3", value);
		DataCube_puthd_str(maskCube, "CTYPE3", value);
	}
	if(DataCube_chkhd(this, "CRVAL1")) DataCube_puthd_flt(maskCube, "CRVAL1", DataCube_gethd_flt(this, "CRVAL1"));
	if(DataCube_chkhd(this, "CRVAL2")) DataCube_puthd_flt(maskCube, "CRVAL2", DataCube_gethd_flt(this, "CRVAL2"));
	if(DataCube_chkhd(this, "CRVAL3")) DataCube_puthd_flt(maskCube, "CRVAL3", DataCube_gethd_flt(this, "CRVAL3"));
	if(DataCube_chkhd(this, "CRPIX1")) DataCube_puthd_flt(maskCube, "CRPIX1", DataCube_gethd_flt(this, "CRPIX1"));
	if(DataCube_chkhd(this, "CRPIX2")) DataCube_puthd_flt(maskCube, "CRPIX2", DataCube_gethd_flt(this, "CRPIX2"));
	if(DataCube_chkhd(this, "CRPIX3")) DataCube_puthd_flt(maskCube, "CRPIX3", DataCube_gethd_flt(this, "CRPIX3"));
	if(DataCube_chkhd(this, "CDELT1")) DataCube_puthd_flt(maskCube, "CDELT1", DataCube_gethd_flt(this, "CDELT1"));
	if(DataCube_chkhd(this, "CDELT2")) DataCube_puthd_flt(maskCube, "CDELT2", DataCube_gethd_flt(this, "CDELT2"));
	if(DataCube_chkhd(this, "CDELT3")) DataCube_puthd_flt(maskCube, "CDELT3", DataCube_gethd_flt(this, "CDELT3"));
	if(DataCube_chkhd(this, "EPOCH"))  DataCube_puthd_flt(maskCube, "EPOCH",  DataCube_gethd_flt(this, "EPOCH"));
	
	// A few additional settings
	const double FWHM_CONST = 2.0 * sqrt(2.0 * log(2.0));  // Conversion between sigma and FWHM of Gaussian function
	const double MAX_PIX_CONST = 1.0e+6;                   // Maximum number of pixels for noise calculation; sampling is set accordingly
	
	// Set sampling sampleRms for rms measurement
	size_t sampleRms = (size_t)pow((double)(this->data_size) / MAX_PIX_CONST, 1.0 / 3.0);
	if(sampleRms < 1) sampleRms = 1;
	
	// Measure noise in original cube with sampling "sampleRms"
	double rms;
	double rms_smooth;
	
	if(method == NOISE_STAT_STD) {
		rms = DataCube_stat_std(this, 0.0, sampleRms, range);
	}
	else if(method == NOISE_STAT_MAD) {
		rms = DataCube_stat_mad(this, 0.0, sampleRms, range) * MAD_TO_STD;
	}
	else {
		rms = DataCube_stat_gauss(this, sampleRms, range);
	}
	
	// Apply threshold to original cube to get an initial mask without smoothing
	DataCube_mask_32(this, maskCube, threshold * rms);
	
	// Run S+C finder for all smoothing kernels
	for(size_t i = 0; i < Array_get_size(kernels_spat); ++i)
	{
		for(size_t j = 0; j < Array_get_size(kernels_spec); ++j)
		{
			message("Smoothing kernel:  [%.1f] x [%d]", Array_get_flt(kernels_spat, i), Array_get_int(kernels_spec, j));
			
			// Check if any smoothing requested
			if(Array_get_flt(kernels_spat, i) || Array_get_int(kernels_spec, j))
			{
				// Smoothing required; create a copy of the original cube
				DataCube *smoothedCube = DataCube_copy(this);
				
				// Set flux of already detected pixels to maskScaleXY * rms
				DataCube_set_masked_32(smoothedCube, maskCube, maskScaleXY * rms);
				
				// Spatial and spectral smoothing
				if(Array_get_flt(kernels_spat, i) > 0.0) DataCube_gaussian_filter(smoothedCube, Array_get_flt(kernels_spat, i) / FWHM_CONST);
				if(Array_get_int(kernels_spec, j) > 0) DataCube_boxcar_filter(smoothedCube, Array_get_int(kernels_spec, j) / 2);
				
				// Calculate the RMS of the smoothed cube
				if(method == NOISE_STAT_STD) rms_smooth = DataCube_stat_std(smoothedCube, 0.0, sampleRms, range);
				else if(method == NOISE_STAT_MAD) rms_smooth = MAD_TO_STD * DataCube_stat_mad(smoothedCube, 0.0, sampleRms, range);
				else rms_smooth = DataCube_stat_gauss(smoothedCube, sampleRms, range);
				message("Noise level:       %.3e\n", rms_smooth);
				
				// Add pixels above threshold to mask
				DataCube_mask_32(smoothedCube, maskCube, threshold * rms_smooth);
				
				// Delete smoothed cube again
				DataCube_delete(smoothedCube);
			}
			else
			{
				// No smoothing required; apply threshold to original cube
				message("Noise level:       %.3e\n", rms);
				DataCube_mask_32(this, maskCube, threshold * rms);
			}
		}
	}
	
	// Return the mask
	return maskCube;
}



// ----------------------------------------------------------------- //
// Link objects in an integer mask                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this       - Object self-reference.                         //
//   (2) mask       - 32-bit integer mask cube.                      //
//   (3) radius_x   - Merging radius in x.                           //
//   (4) radius_y   - Merging radius in y.                           //
//   (5) radius_z   - Merging radius in z.                           //
//   (6) min_size_x - Minimum size requirement for objects in x.     //
//   (7) min_size_y - Minimum size requirement for objects in y.     //
//   (8) min_size_z - Minimum size requirement for objects in z.     //
//   (9) positivity - If true, negative sources will be discarded.   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for linking objects recorded in an integer mask   //
//   within the specified merging radii. The mask must be a 32-bit   //
//   integer array with a background value of 0, while objects must  //
//   have a value of 1. The linker will first give objects that are  //
//   connected within the specified radii a unique label. In a sec-  //
//   ond step, objects that fall below the minimum size requirements //
//   will be removed, and all remaining objects will be relabelled   //
//   in consecutive order starting from 1. If positivity is set to   //
//   true, sources with negative total flux will also be removed.    //
// ----------------------------------------------------------------- //

public LinkerPar *DataCube_run_linker(const DataCube *this, DataCube *mask, const size_t radius_x, const size_t radius_y, const size_t radius_z, const size_t min_size_x, const size_t min_size_y, const size_t min_size_z, const bool positivity)
{
	check_null(this);
	check_null(mask);
	ensure(mask->data_type == 32, "Linker will only accept 32-bit integer masks.");
	ensure(this->axis_size[0] == mask->axis_size[0] && this->axis_size[1] == mask->axis_size[1] && this->axis_size[2] == mask->axis_size[2], "Data cube and mask cube have different sizes.");
	
	// Create linker parameter object
	LinkerPar *lpar = LinkerPar_new();
	// Create two dummy objects (as our labelling starts with 2, not 0)
	LinkerPar_push(lpar, 0, 0, 0, 0.0);
	LinkerPar_push(lpar, 0, 0, 0, 0.0);
	
	int32_t label = 2;
	size_t index;
	int32_t *ptr;
	
	// Link pixels into sources
	for(size_t z = mask->axis_size[2]; z--;)
	{
		progress_bar("Linking:  ", mask->axis_size[2] - 1 - z, mask->axis_size[2] - 1);
		
		for(size_t y = mask->axis_size[1]; y--;)
		{
			for(size_t x = mask->axis_size[0]; x--;)
			{
				index = DataCube_get_index(mask, x, y, z);
				ptr = (int32_t *)(mask->data + index * mask->word_size);
				
				if(*ptr == 1)
				{
					ensure(label > 0, "Too many sources for 32-bit dynamic range of mask.");
					*ptr = label;
					LinkerPar_push(lpar, x, y, z, DataCube_get_data_flt(this, x, y, z));
					DataCube_mark_neighbours(this, mask, x, y, z, radius_x, radius_y, radius_z, label, lpar);
					label += 1;
					if(label < 2) label = 2;
				}
			}
		}
	}
	
	LinkerPar_print_info(lpar);
	label = 1;
	
	// Filter and relabel sources
	for(size_t z = mask->axis_size[2]; z--;)
	{
		progress_bar("Filtering:", mask->axis_size[2] - 1 - z, mask->axis_size[2] - 1);
		
		for(size_t y = mask->axis_size[1]; y--;)
		{
			for(size_t x = mask->axis_size[0]; x--;)
			{
				index = DataCube_get_index(mask, x, y, z);
				ptr = (int32_t *)(mask->data + index * mask->word_size);
				
				if(*ptr > 0)
				{
					if(LinkerPar_get_size(lpar, *ptr, 0) < min_size_x
						|| LinkerPar_get_size(lpar, *ptr, 1) < min_size_y 
						|| LinkerPar_get_size(lpar, *ptr, 2) < min_size_z
						|| (positivity && LinkerPar_get_flux(lpar, *ptr) < 0.0)) *ptr = 0;
					else
					{
						if(LinkerPar_get_label(lpar, *ptr) == 0)
						{
							LinkerPar_set_label(lpar, *ptr, label);
							label += 1;
						}
						
						*ptr = LinkerPar_get_label(lpar, *ptr);
					}
				}
			}
		}
	}
	
	// Discard unwanted objects from list
	LinkerPar_reduce(lpar);
	LinkerPar_print_info(lpar);
	
	// Return LinkerPar object
	return lpar;
}



// ----------------------------------------------------------------- //
// Recursive function for labelling neighbouring pixels              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1)  this      - Object self-reference.                         //
//   (2)  mask      - 32-bit mask cube.                              //
//   (3)  x         - x position of pixel the neighbours of which    //
//                    are to be labelled.                            //
//   (4)  y         - y position of pixel the neighbours of which    //
//                    are to be labelled.                            //
//   (5)  z         - z position of pixel the neighbours of which    //
//                    are to be labelled.                            //
//   (6)  radius_x  - Merging radius in x.                           //
//   (7)  radius_y  - Merging radius in y.                           //
//   (8)  radius_z  - Merging radius in z.                           //
//   (9)  label     - Label to be assigned to detected neighbours.   //
//   (10) lpar      - Pointer to LinkerPar object containing the re- //
//                    corded object parameters. This will be updated //
//                    whenever a new pixel is assigned to the same   //
//                    object currently getting linked.               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for checking whether any neighbouring pixels of  //
//   the specified location (x, y, z) within the specified merging   //
//   radii are detected by the source finder (value of 1). If so,    //
//   their value will be set to the same label as (x, y, z) and the  //
//   LinkerPar object will be updated to include the new pixel.      //
//   The function will then call itself to recursively check and     //
//   label all of the neighbours of the new pixel.                   //
// ----------------------------------------------------------------- //

private void DataCube_mark_neighbours(const DataCube *this, DataCube *mask, const size_t x, const size_t y, const size_t z, const size_t radius_x, const size_t radius_y, const size_t radius_z, const int32_t label, LinkerPar *lpar)
{
	size_t x1 = (x > radius_x) ? (x - radius_x) : 0;
	size_t y1 = (y > radius_y) ? (y - radius_y) : 0;
	size_t z1 = (z > radius_z) ? (z - radius_z) : 0;
	size_t x2 = (x < mask->axis_size[0] - 1 - radius_x) ? (x + radius_x) : (mask->axis_size[0] - 1);
	size_t y2 = (y < mask->axis_size[1] - 1 - radius_y) ? (y + radius_y) : (mask->axis_size[1] - 1);
	size_t z2 = (z < mask->axis_size[2] - 1 - radius_z) ? (z + radius_z) : (mask->axis_size[2] - 1);
	
	for(size_t zz = z1; zz <= z2; ++zz)
	{
		for(size_t yy = y1; yy <= y2; ++yy)
		{
			for(size_t xx = x1; xx <= x2; ++xx)
			{
				if((xx - x) * (xx - x) + (yy - y) * (yy - y) < radius_x * radius_y) continue;
				
				size_t index = DataCube_get_index(mask, xx, yy, zz);
				int32_t *ptr = (int32_t *)(mask->data + index * mask->word_size);
				
				if(*ptr == 1)
				{
					*ptr = label;
					LinkerPar_update(lpar, label, xx, yy, zz, DataCube_get_data_flt(this, xx, yy, zz));
					DataCube_mark_neighbours(this, mask, xx, yy, zz, radius_x, radius_y, radius_z, label, lpar);
				}
			}
		}
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Source parameterisation                                           //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1)  this      - Object self-reference.                         //
//   (2)  mask      - 32-bit mask cube.                              //
//   (3)  cat       - Catalogue of sources to be parameterised.      //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for measuring advanced parameters of all sources  //
//   contained in the specified catalogue. The mask cube must be of  //
//   32-bit integer type and must have the same dimensions as the    //
//   data cube. All sources found in the catalogue must also be re-  //
//   corded in the mask with their catalogued source ID number.      //
//                                                                   //
//   All parameters derived by this method will be appended at the   //
//   end of the catalogue. Currently determined parameters include:  //
//                                                                   //
//   - Local RMS noise level                                         //
// ----------------------------------------------------------------- //

public void DataCube_parameterise(const DataCube *this, const DataCube *mask, Catalog *cat)
{
	// Sanity checks
	check_null(this);
	check_null(this->data);
	check_null(mask);
	check_null(mask->data);
	check_null(cat);
	ensure(this->data_type == -32 || this->data_type == -64, "Parameterisation only possible with floating-point data.");
	ensure(mask->data_type > 0, "Mask must be of integer type.");
	ensure(this->axis_size[0] == mask->axis_size[0] && this->axis_size[1] == mask->axis_size[1] && this->axis_size[2] == mask->axis_size[2], "Data cube and mask cube have different sizes.");
	
	// Determine catalogue size
	const size_t cat_size = Catalog_get_size(cat);
	if(cat_size == 0)
	{
		warning("No sources in catalogue; nothing to parameterise.");
		return;
	}
	message("Found %zu source(s) in need of parameterisation.\n", cat_size);
	
	// Extract flux unit from header
	char buffer[FITS_HEADER_VALUE_SIZE + 1] =  "";
	int flag = DataCube_gethd_str(this, "BUNIT", buffer);
	if(flag)
	{
		warning("No flux unit (\'BUNIT\') defined in header.");
		strcpy(buffer, "???");
	}
	char *flux_unit = trim_string(buffer);
	
	// Loop over all sources in catalogue
	for(size_t i = 0; i < cat_size; ++i)
	{
		// Extract source
		Source *src = Catalog_get_source(cat, i);
		
		// Get source ID
		const size_t src_id = Source_get_par_by_name_int(src, "id");
		ensure(src_id, "Source ID missing from catalogue; cannot parameterise.");
		progress_bar("Progress: ", i + 1, cat_size);
		
		// Get source bounding box
		const size_t x_min = Source_get_par_by_name_int(src, "x_min");
		const size_t x_max = Source_get_par_by_name_int(src, "x_max");
		const size_t y_min = Source_get_par_by_name_int(src, "y_min");
		const size_t y_max = Source_get_par_by_name_int(src, "y_max");
		const size_t z_min = Source_get_par_by_name_int(src, "z_min");
		const size_t z_max = Source_get_par_by_name_int(src, "z_max");
		ensure(x_min && x_max && y_min && y_max && z_min && z_max, "Source bounding box missing from catalogue.");
		ensure(x_min <= x_max && y_min <= y_max && z_min <= z_max, "Illegal source bounding box: min > max!");
		ensure(x_max < this->axis_size[0] && y_max < this->axis_size[1] && z_max < this->axis_size[2], "Source bounding box outside data cube boundaries.");
		
		// Initialise parameters
		double rms = 0.0;
		
		Array *array_rms = Array_new(0, ARRAY_TYPE_FLT);
		
		// Loop over source bounding box
		for(size_t z = z_min; z <= z_max; ++z)
		{
			for(size_t y = y_min; y <= y_max; ++y)
			{
				for(size_t x = x_min; x <= x_max; ++x)
				{
					const size_t id    = DataCube_get_data_int(mask, x, y, z);
					const double value = DataCube_get_data_flt(this, x, y, z);
					
					if(id == src_id)
					{
						// Measure basic source parameters
						// ...
					}
					else if(id == 0)
					{
						// Measure local noise level
						Array_push_flt(array_rms, value);
					}
				}
			}
		}
		
		if(Array_get_size(array_rms)) rms = MAD_TO_STD * mad_val_dbl(Array_get_ptr(array_rms), Array_get_size(array_rms), 0.0, 1, 0);
		else warning("Failed to measure local noise level for source %zu.", src_id);
		
		// Update catalogue entry
		Source_set_par_flt(src, "rms", rms, flux_unit, "instr.det.noise");
		
		// Clean up
		Array_delete(array_rms);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Swap byte order of data array                                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this       - Object self-reference.                         //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for swapping the byte order of the data array    //
//   stored in the object referred to by this. The function will     //
//   check if byte order swapping is necessary and, if so, loop over //
//   the entire array and call the corresponding swapping function   //
//   defined in common.c on each array element.                      //
// ----------------------------------------------------------------- //

private void DataCube_swap_byte_order(const DataCube *this)
{
	if(is_little_endian() && this->word_size > 1)
	{
		for(char *ptr = this->data; ptr < this->data + this->data_size * this->word_size; ptr += this->word_size)
		{
			swap_byte_order(ptr, this->word_size);
		}
	}
	
	return;
}
