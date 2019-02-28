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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "src/common.h"
#include "src/Path.h"
#include "src/Array.h"
#include "src/Parameter.h"
#include "src/Catalog.h"
#include "src/DataCube.h"
#include "src/LinkerPar.h"

int main(int argc, char **argv)
{
	// Record start time
	const clock_t start_time   = clock();
	const time_t  current_time = time(NULL);
	
	
	
	// ---------------------------- //
	// Check command line arguments //
	// ---------------------------- //
	
	ensure(argc == 2, "Missing command line argument.\nUsage: sofia <parameter_file>");
	
	
	
	// ---------------------------- //
	// Check SOFIA2_PATH variable   //
	// ---------------------------- //
	
	const char *ENV_SOFIA2_PATH = getenv("SOFIA2_PATH");
	ensure(ENV_SOFIA2_PATH != NULL,
		"Environment variable \'SOFIA2_PATH\' is not defined.\n"
		"       Please follow the instructions provided by the installation\n"
		"       script to define this variable before running SoFiA.");
	
	
	
	// ---------------------------- //
	// Print basic information      //
	// ---------------------------- //
	
	status("Pipeline started");
	message("Using:   Source Finding Application (SoFiA)");
	message("Version: %s", VERSION);
	message("Time:    %s", ctime(&current_time));
	
	status("Loading parameter settings");
	
	
	
	// ---------------------------- //
	// Load default parameters      //
	// ---------------------------- //
	
	message("Loading SoFiA default parameter file.");
	
	Path *path_sofia = Path_new();
	Path_set_dir(path_sofia, ENV_SOFIA2_PATH);
	Path_set_file(path_sofia, "default_parameters.par");
	
	Parameter *par = Parameter_new();
	Parameter_load(par, Path_get(path_sofia), PARAMETER_APPEND);
	
	Path_delete(path_sofia);
	
	
	
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
	
	Path *path_cat_ascii = Path_new();
	Path *path_cat_xml   = Path_new();
	Path *path_mask_out  = Path_new();
	
	// Set directory name depending on user input
	if(strlen(base_dir))
	{
		Path_set_dir(path_cat_ascii, base_dir);
		Path_set_dir(path_cat_xml,   base_dir);
		Path_set_dir(path_mask_out,  base_dir);
	}
	else
	{
		Path_set_dir(path_cat_ascii, Path_get_dir(path_data_in));
		Path_set_dir(path_cat_xml,   Path_get_dir(path_data_in));
		Path_set_dir(path_mask_out,  Path_get_dir(path_data_in));
	}
	
	// Set file name depending on user input
	if(strlen(base_name))
	{
		Path_set_file_from_template(path_cat_ascii, base_name, "_cat", ".txt");
		Path_set_file_from_template(path_cat_xml,   base_name, "_cat", ".xml");
		Path_set_file_from_template(path_mask_out,  base_name, "_mask", ".fits");
	}
	else
	{
		Path_set_file_from_template(path_cat_ascii, Path_get_file(path_data_in), "_cat", ".txt");
		Path_set_file_from_template(path_cat_xml,   Path_get_file(path_data_in), "_cat", ".xml");
		Path_set_file_from_template(path_mask_out,  Path_get_file(path_data_in), "_mask", ".fits");
	}
	
	
	
	// ---------------------------- //
	// Load data cube               //
	// ---------------------------- //
	
	status("Loading data cube");
	DataCube *dataCube = DataCube_new();
	Array *region = NULL;
	if(strlen(Parameter_get_str(par, "input.region"))) region = Array_new_str(Parameter_get_str(par, "input.region"), ARRAY_TYPE_INT);
	DataCube_load(dataCube, Path_get(path_data_in), region);
	Array_delete(region);
	
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
	
	// Generate catalogue from linker output
	Catalog *catalog = LinkerPar_make_catalog(linker_par);
	
	// Delete linker parameters
	LinkerPar_delete(linker_par);
	
	
	
	// ---------------------------- //
	// Save catalogue(s)            //
	// ---------------------------- //
	
	status("Writing source catalogue");
	if(Parameter_get_bool(par, "output.writeCatASCII"))
	{
		message("Writing ASCII format:   %s", Path_get_file(path_cat_ascii));
		Catalog_save(catalog, Path_get(path_cat_ascii), CATALOG_FORMAT_ASCII);
	}
	if(Parameter_get_bool(par, "output.writeCatXML"))
	{
		message("Writing VOTable format: %s", Path_get_file(path_cat_xml));
		Catalog_save(catalog, Path_get(path_cat_xml), CATALOG_FORMAT_XML);
	}
	
	
	
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
	
	// Delete input parameters
	Parameter_delete(par);
	
	// Delete file paths
	Path_delete(path_data_in);
	Path_delete(path_cat_ascii);
	Path_delete(path_cat_xml);
	Path_delete(path_mask_out);
	
	// Delete source catalogue
	Catalog_delete(catalog);
	
	// Print time
	timestamp(start_time);
	status("Pipeline finished.");
	
	return 0;
}
