/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (main.c) - Source Finding Application               ///
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

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "Path.h"
#include "Array.h"
#include "Parameter.h"
#include "SourceCatalog.h"
#include "DataCube.h"

int main()
{
	// Record start time
	clock_t start_time = clock();
	
	// Load default parameters
	Parameter *par = Parameter_new();
	Parameter_load(par, "default_parameters.par");
	
	// Check input and output files
	const char *base_dir = Parameter_get_str(par, "output.directory");
	const char *base_name = Parameter_get_str(par, "output.filename");
	
	Path *path_data_in = Path_new();
	Path_set(path_data_in, Parameter_get_str(par, "input.dataFile"));
	
	Path *path_mask_out = Path_new();
	if(strlen(base_dir)) Path_set_dir(path_mask_out, base_dir);
	else Path_set_dir(path_mask_out, path_data_in->dir);
	if(strlen(base_name)) Path_set_file_from_template(path_mask_out, base_name, "_mask", ".fits");
	else Path_set_file_from_template(path_mask_out, path_data_in->file, "_mask", ".fits");
	
	// Load data cube
	DataCube *dataCube = DataCube_new();
	DataCube_load(dataCube, Path_get(path_data_in));
	
	// Print time
	timestamp(start_time);
	
	// Run source finder
	Array *kernels_spat = Array_new_str(Parameter_get_str(par, "scfind.kernelsXY"));
	Array *kernels_spec = Array_new_str(Parameter_get_str(par, "scfind.kernelsZ"));
	
	DataCube *maskCube = DataCube_run_scfind(dataCube, kernels_spat, kernels_spec, Parameter_get_flt(par, "scfind.threshold"), Parameter_get_flt(par, "scfind.replacement"));
	
	Array_delete(kernels_spat);
	Array_delete(kernels_spec);
	
	// Print time
	timestamp(start_time);
	
	// Run linker
	DataCube_run_linker(maskCube, Parameter_get_int(par, "linker.radiusX"), Parameter_get_int(par, "linker.radiusY"), Parameter_get_int(par, "linker.radiusZ"), Parameter_get_int(par, "linker.minSizeX"), Parameter_get_int(par, "linker.minSizeY"), Parameter_get_int(par, "linker.minSizeZ"));
	
	// Print time
	timestamp(start_time);
	
	// Save mask cube
	if(Parameter_get_bool(par, "output.writeMask")) DataCube_save(maskCube, Path_get(path_mask_out), Parameter_get_bool(par, "output.overwrite"));
	
	// Delete data cube and mask cube
	DataCube_delete(maskCube);
	DataCube_delete(dataCube);
	
	// Delete parameters
	Parameter_delete(par);
	
	// Delete paths
	Path_delete(path_data_in);
	Path_delete(path_mask_out);
	
	// Print time
	timestamp(start_time);
	
	message("Pipeline finished.\n\n");
	
	return 0;
}
