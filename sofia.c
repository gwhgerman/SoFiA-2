/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (sofia.c) - Source Finding Application              ///
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

#include "src/common.h"
#include "src/Path.h"
#include "src/Array.h"
#include "src/Parameter.h"
#include "src/SourceCatalog.h"
#include "src/DataCube.h"
#include "src/LinkerPar.h"

int main(int argc, char **argv)
{
	// Record start time
	clock_t start_time = clock();
	
	
	// ---------------------------- //
	// Check command line arguments //
	// ---------------------------- //
	ensure(argc == 2, "Missing command line argument.\nUsage: sofia <parameter_file>");
	
	
	// ---------------------------- //
	// Setup and basic information  //
	// ---------------------------- //
	status("Source Finding Application (SoFiA)\n Version " VERSION);
	message("Pipeline started");
	
	status("Loading parameter settings");
	
	
	// ---------------------------- //
	// Load default parameters      //
	// ---------------------------- //
	message("Loading SoFiA default parameter file.");
	Parameter *par = Parameter_new();
	Parameter_load(par, "default_parameters.par", PARAMETER_APPEND);
	
	
	// ---------------------------- //
	// Load user parameters         //
	// ---------------------------- //
	message("Loading user parameter file: \'%s\'.", argv[1]);
	Parameter_load(par, argv[1], PARAMETER_UPDATE);
	
	
	// ---------------------------- //
	// Check input and output files //
	// ---------------------------- //
	const char *base_dir = Parameter_get_str(par, "output.directory");
	const char *base_name = Parameter_get_str(par, "output.filename");
	
	Path *path_data_in = Path_new();
	Path_set(path_data_in, Parameter_get_str(par, "input.dataCube"));
	
	Path *path_mask_out = Path_new();
	if(strlen(base_dir)) Path_set_dir(path_mask_out, base_dir);
	else Path_set_dir(path_mask_out, Path_get_dir(path_data_in));
	if(strlen(base_name)) Path_set_file_from_template(path_mask_out, base_name, "_mask", ".fits");
	else Path_set_file_from_template(path_mask_out, Path_get_file(path_data_in), "_mask", ".fits");
	
	
	// ---------------------------- //
	// Load data cube               //
	// ---------------------------- //
	status("Loading data cube");
	DataCube *dataCube = DataCube_new();
	Array *region = NULL;
	if(strlen(Parameter_get_str(par, "input.region"))) region = Array_new_str(Parameter_get_str(par, "input.region"), ARRAY_TYPE_INT);
	DataCube_load(dataCube, Path_get(path_data_in), region);
	Array_delete(region);  // NOTE: region could be retained at this point if needed later on!
	
	// Print time
	timestamp(start_time);
	
	
	// ---------------------------- //
	// Run source finder            //
	// ---------------------------- //
	status("Running S+C finder");
	Array *kernels_spat = Array_new_str(Parameter_get_str(par, "scfind.kernelsXY"), ARRAY_TYPE_FLT);
	Array *kernels_spec = Array_new_str(Parameter_get_str(par, "scfind.kernelsZ"), ARRAY_TYPE_INT);
	
	DataCube *maskCube = DataCube_run_scfind(dataCube, kernels_spat, kernels_spec, Parameter_get_flt(par, "scfind.threshold"), Parameter_get_flt(par, "scfind.replacement"));
	
	Array_delete(kernels_spat);
	Array_delete(kernels_spec);
	
	// Print time
	timestamp(start_time);
	
	
	// ---------------------------- //
	// Run linker                   //
	// ---------------------------- //
	status("Running Linker");
	LinkerPar *linker_par = DataCube_run_linker(maskCube, Parameter_get_int(par, "linker.radiusX"), Parameter_get_int(par, "linker.radiusY"), Parameter_get_int(par, "linker.radiusZ"), Parameter_get_int(par, "linker.minSizeX"), Parameter_get_int(par, "linker.minSizeY"), Parameter_get_int(par, "linker.minSizeZ"));
	
	// Print time
	timestamp(start_time);
	
	
	// ---------------------------- //
	// Create initial catalogue     //
	// ---------------------------- //
	Catalog *catalog = LinkerPar_make_catalog(linker_par);
	Catalog_save(catalog, "test.ascii", CATALOG_FORMAT_ASCII);
	
	// Delete linker parameters
	LinkerPar_delete(linker_par);
	
	
	// ---------------------------- //
	// Save mask cube               //
	// ---------------------------- //
	status("Writing mask cube");
	if(Parameter_get_bool(par, "output.writeMaskCube")) DataCube_save(maskCube, Path_get(path_mask_out), Parameter_get_bool(par, "output.overwrite"));
	
	
	// ---------------------------- //
	// Clean up and exit            //
	// ---------------------------- //
	
	// Delete data cube and mask cube
	DataCube_delete(maskCube);
	DataCube_delete(dataCube);
	
	// Delete parameters
	Parameter_delete(par);
	
	// Delete paths
	Path_delete(path_data_in);
	Path_delete(path_mask_out);
	
	// Delete catalogue
	Catalog_delete(catalog);
	
	// Print time
	timestamp(start_time);
	
	message("Pipeline finished.\n");
	
	return 0;
}
