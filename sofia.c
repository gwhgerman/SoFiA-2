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

//#include <sys/stat.h>  // Needed for mkdir()

#include "src/common.h"
#include "src/Path.h"
#include "src/Array.h"
#include "src/Parameter.h"
#include "src/Catalog.h"
#include "src/DataCube.h"
#include "src/LinkerPar.h"

int main(int argc, char **argv)
{
	// ---------------------------- //
	// Record start time            //
	// ---------------------------- //
	
	const time_t start_time = time(NULL);
	
	
	
	// ---------------------------- //
	// A few global definitions     //
	// ---------------------------- //
	
	const char *noise_stat_name[] = {"standard deviation", "median absolute deviation", "Gaussian fit to flux histogram"};
	const char *flux_range_name[] = {"negative", "full", "positive"};
	noise_stat statistic = NOISE_STAT_STD;
	int range = 0;
	bool verbosity = false;
	
	
	
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
	message("Version: %s", SOFIA_VERSION);
	message("Time:    %s", ctime(&start_time));
	
	status("Loading parameter settings");
	
	
	
	// ---------------------------- //
	// Load default parameters      //
	// ---------------------------- //
	
	message("Loading SoFiA default parameter file.");
	
	Path *path_sofia = Path_new();
	Path_set_dir(path_sofia, ENV_SOFIA2_PATH);
	Path_set_file(path_sofia, "default_parameters.par");
	
	Parameter *par = Parameter_new(verbosity);
	Parameter_load(par, Path_get(path_sofia), PARAMETER_APPEND);
	
	Path_delete(path_sofia);
	
	
	
	// ---------------------------- //
	// Load user parameters         //
	// ---------------------------- //
	
	message("Loading user parameter file: \'%s\'.", argv[1]);
	Parameter_load(par, argv[1], PARAMETER_UPDATE);
	
	// Update verbosity level
	verbosity = Parameter_get_bool(par, "pipeline.verbose");
	
	
	
	// ---------------------------- //
	// Check input and output files //
	// ---------------------------- //
	
	const char *base_dir = Parameter_get_str(par, "output.directory");
	const char *base_name = Parameter_get_str(par, "output.filename");
	
	Path *path_data_in = Path_new();
	Path_set(path_data_in, Parameter_get_str(par, "input.data"));
	
	Path *path_cat_ascii = Path_new();
	Path *path_cat_xml   = Path_new();
	Path *path_noise     = Path_new();
	Path *path_filtered  = Path_new();
	Path *path_mask_out  = Path_new();
	Path *path_mom0      = Path_new();
	Path *path_mom1      = Path_new();
	Path *path_mom2      = Path_new();
	Path *path_cubelets  = Path_new();
	
	// Set directory names depending on user input
	if(strlen(base_dir))
	{
		Path_set_dir(path_cat_ascii, base_dir);
		Path_set_dir(path_cat_xml,   base_dir);
		Path_set_dir(path_noise,     base_dir);
		Path_set_dir(path_filtered,  base_dir);
		Path_set_dir(path_mask_out,  base_dir);
		Path_set_dir(path_mom0,      base_dir);
		Path_set_dir(path_mom1,      base_dir);
		Path_set_dir(path_mom2,      base_dir);
		Path_set_dir(path_cubelets,  base_dir);
	}
	else
	{
		Path_set_dir(path_cat_ascii, Path_get_dir(path_data_in));
		Path_set_dir(path_cat_xml,   Path_get_dir(path_data_in));
		Path_set_dir(path_noise,     Path_get_dir(path_data_in));
		Path_set_dir(path_filtered,  Path_get_dir(path_data_in));
		Path_set_dir(path_mask_out,  Path_get_dir(path_data_in));
		Path_set_dir(path_mom0,      Path_get_dir(path_data_in));
		Path_set_dir(path_mom1,      Path_get_dir(path_data_in));
		Path_set_dir(path_mom2,      Path_get_dir(path_data_in));
		Path_set_dir(path_cubelets,  Path_get_dir(path_data_in));
	}
	
	//Path_append_dir(path_cubelets, "cubelets");
	
	// Set file names depending on user input
	if(strlen(base_name))
	{
		Path_set_file_from_template(path_cat_ascii, base_name, "_cat", ".txt");
		Path_set_file_from_template(path_cat_xml,   base_name, "_cat", ".xml");
		Path_set_file_from_template(path_noise,     base_name, "_noise", ".fits");
		Path_set_file_from_template(path_filtered,  base_name, "_filtered", ".fits");
		Path_set_file_from_template(path_mask_out,  base_name, "_mask", ".fits");
		Path_set_file_from_template(path_mom0,      base_name, "_mom0", ".fits");
		Path_set_file_from_template(path_mom1,      base_name, "_mom1", ".fits");
		Path_set_file_from_template(path_mom2,      base_name, "_mom2", ".fits");
		Path_set_file(path_cubelets, base_name);
	}
	else
	{
		Path_set_file_from_template(path_cat_ascii, Path_get_file(path_data_in), "_cat", ".txt");
		Path_set_file_from_template(path_cat_xml,   Path_get_file(path_data_in), "_cat", ".xml");
		Path_set_file_from_template(path_noise,     Path_get_file(path_data_in), "_noise", ".fits");
		Path_set_file_from_template(path_filtered,  Path_get_file(path_data_in), "_filtered", ".fits");
		Path_set_file_from_template(path_mask_out,  Path_get_file(path_data_in), "_mask", ".fits");
		Path_set_file_from_template(path_mom0,      Path_get_file(path_data_in), "_mom0", ".fits");
		Path_set_file_from_template(path_mom1,      Path_get_file(path_data_in), "_mom1", ".fits");
		Path_set_file_from_template(path_mom2,      Path_get_file(path_data_in), "_mom2", ".fits");
		Path_set_file_from_template(path_cubelets,  Path_get_file(path_data_in), "", "");
	}
	
	
	
	// ---------------------------- //
	// Load data cube               //
	// ---------------------------- //
	
	status("Loading data cube");
	DataCube *dataCube = DataCube_new(verbosity);
	Array *region = NULL;
	if(strlen(Parameter_get_str(par, "input.region"))) region = Array_new_str(Parameter_get_str(par, "input.region"), ARRAY_TYPE_INT);
	DataCube_load(dataCube, Path_get(path_data_in), region);
	
	// Print time
	timestamp(start_time);
	
	
	
	// ---------------------------- //
	// Scale data by noise level    //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "scaleNoise.enable"))
	{
		status("Scaling data by noise");
		
		// Determine noise measurement method to use
		statistic = NOISE_STAT_STD;
		if(strcmp(Parameter_get_str(par, "scaleNoise.statistic"), "mad") == 0) statistic = NOISE_STAT_MAD;
		else if(strcmp(Parameter_get_str(par, "scaleNoise.statistic"), "gauss") == 0) statistic = NOISE_STAT_GAUSS;
		
		// Determine flux range to use
		range = 0;
		if(strcmp(Parameter_get_str(par, "scaleNoise.fluxRange"), "negative") == 0) range = -1;
		else if(strcmp(Parameter_get_str(par, "scaleNoise.fluxRange"), "positive") == 0) range = 1;
		
		if(strcmp(Parameter_get_str(par, "scaleNoise.mode"), "local") == 0)
		{
			// Local noise scaling
			message("Correcting for local noise variations.");
			DataCube *noiseCube = DataCube_scale_noise_local(dataCube, statistic, range, Parameter_get_int(par, "scaleNoise.windowSpatial"), Parameter_get_int(par, "scaleNoise.windowSpectral"), Parameter_get_int(par, "scaleNoise.gridSpatial"), Parameter_get_int(par, "scaleNoise.gridSpectral"), Parameter_get_bool(par, "scaleNoise.interpolate"));
			DataCube_save(noiseCube, Path_get(path_noise), Parameter_get_bool(par, "output.overwrite"));
			DataCube_delete(noiseCube);
		}
		else
		{
			// Global noise scaling along spectral axis
			message("Correcting for noise variations along spectral axis.");
			message("- Noise statistic:  %s", noise_stat_name[statistic]);
			message("- Flux range:       %s\n", flux_range_name[range + 1]);
			DataCube_scale_noise_spec(dataCube, statistic, range);
		}
		
		// Print time
		timestamp(start_time);
	}
	
	
	
	// ---------------------------- //
	// Write filtered cube          //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "output.writeFiltered") && Parameter_get_bool(par, "scaleNoise.enable"))  // ALERT: Set conditions here as needed.
	{
		status("Writing filtered cube");
		DataCube_save(dataCube, Path_get(path_filtered), Parameter_get_bool(par, "output.overwrite"));
	}
	
	
	
	// ---------------------------- //
	// Run source finder            //
	// ---------------------------- //
	
	// Determine noise measurement method to use
	statistic = NOISE_STAT_STD;
	if(strcmp(Parameter_get_str(par, "scfind.statistic"), "mad") == 0) statistic = NOISE_STAT_MAD;
	else if(strcmp(Parameter_get_str(par, "scfind.statistic"), "gauss") == 0) statistic = NOISE_STAT_GAUSS;
	
	// Determine flux range to use
	range = 0;
	if(strcmp(Parameter_get_str(par, "scfind.fluxRange"), "negative") == 0) range = -1;
	else if(strcmp(Parameter_get_str(par, "scfind.fluxRange"), "positive") == 0) range = 1;
	
	status("Running S+C finder");
	message("Using the following parameters:");
	message("- Kernels");
	message("  - spatial:        %s", Parameter_get_str(par, "scfind.kernelsXY"));
	message("  - spectral:       %s", Parameter_get_str(par, "scfind.kernelsZ"));
	message("- Flux threshold:   %s * rms", Parameter_get_str(par, "scfind.threshold"));
	message("- Noise statistic:  %s", noise_stat_name[statistic]);
	message("- Flux range:       %s\n", flux_range_name[range + 1]);
	
	Array *kernels_spat = Array_new_str(Parameter_get_str(par, "scfind.kernelsXY"), ARRAY_TYPE_FLT);
	Array *kernels_spec = Array_new_str(Parameter_get_str(par, "scfind.kernelsZ"), ARRAY_TYPE_INT);
	
	// Run S+C finder to obtain mask
	DataCube *maskCube = DataCube_run_scfind(dataCube, kernels_spat, kernels_spec, Parameter_get_flt(par, "scfind.threshold"), Parameter_get_flt(par, "scfind.replacement"), statistic, range);
	
	// Set BUNIT keyword of mask cube
	DataCube_puthd_str(maskCube, "BUNIT", " ");
	
	// Clean up
	Array_delete(kernels_spat);
	Array_delete(kernels_spec);
	
	// Print time
	timestamp(start_time);
	
	
	
	// ---------------------------- //
	// Reload data cube if required //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "scaleNoise.enable"))  // ALERT: Set conditions here as needed.
	{
		status("Reloading data cube for parameterisation");
		DataCube_load(dataCube, Path_get(path_data_in), region);
		
		// Print time
		timestamp(start_time);
	}
	
	
	
	// ---------------------------- //
	// Run linker                   //
	// ---------------------------- //
	
	status("Running Linker");
	
	const bool remove_neg_src = true ? true : false;  // ALERT: Set conditions here as needed.
	
	LinkerPar *linker_par = DataCube_run_linker(dataCube, maskCube, Parameter_get_int(par, "linker.radiusX"), Parameter_get_int(par, "linker.radiusY"), Parameter_get_int(par, "linker.radiusZ"), Parameter_get_int(par, "linker.minSizeX"), Parameter_get_int(par, "linker.minSizeY"), Parameter_get_int(par, "linker.minSizeZ"), remove_neg_src);
	
	
	
	// ---------------------------- //
	// Create initial catalogue     //
	// ---------------------------- //
	
	// Extract flux unit from header
	char buffer[FITS_HEADER_VALUE_SIZE + 1] =  "";
	const int flag = DataCube_gethd_str(dataCube, "BUNIT", buffer);
	if(flag)
	{
		warning("No flux unit (\'BUNIT\') defined in header.");
		strcpy(buffer, "???");
	}
	const char *flux_unit = trim_string(buffer);
	
	// Generate catalogue from linker output
	Catalog *catalog = LinkerPar_make_catalog(linker_par, flux_unit);
	
	// Delete linker parameters, as they are no longer needed
	LinkerPar_delete(linker_par);
	
	// Print time
	timestamp(start_time);
	
	// Terminate if catalogue is empty
	ensure(Catalog_get_size(catalog), "No sources left after linking. Terminating pipeline.");
	
	
	
	// ---------------------------- //
	// Parameterise sources         //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "parameters.enable"))
	{
		status("Measuring source parameters");
		DataCube_parameterise(dataCube, maskCube, catalog);
		
		// Print time
		timestamp(start_time);
	}
	
	
	
	// ---------------------------- //
	// Save catalogue(s)            //
	// ---------------------------- //
	
	status("Writing source catalogue");
	
	if(Parameter_get_bool(par, "output.writeCatASCII"))
	{
		message("Writing ASCII file:   %s", Path_get_file(path_cat_ascii));
		Catalog_save(catalog, Path_get(path_cat_ascii), CATALOG_FORMAT_ASCII, Parameter_get_bool(par, "output.overwrite"));
	}
	
	if(Parameter_get_bool(par, "output.writeCatXML"))
	{
		message("Writing VOTable file: %s", Path_get_file(path_cat_xml));
		Catalog_save(catalog, Path_get(path_cat_xml), CATALOG_FORMAT_XML, Parameter_get_bool(par, "output.overwrite"));
	}
	
	// Print time
	timestamp(start_time);
	
	
	
	// ---------------------------- //
	// Save mask cube               //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "output.writeMask"))
	{
		status("Writing mask cube");
		DataCube_save(maskCube, Path_get(path_mask_out), Parameter_get_bool(par, "output.overwrite"));
		
		// Print time
		timestamp(start_time);
	}
	
	
	
	// ---------------------------- //
	// Create and save moment maps  //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "output.writeMoments"))
	{
		status("Creating moment maps");
		
		// Generate moment maps
		DataCube *mom0;
		DataCube *mom1;
		DataCube *mom2;
		DataCube_create_moments(dataCube, maskCube, &mom0, &mom1, &mom2);
		
		// Save moment maps to disk
		DataCube_save(mom0, Path_get(path_mom0), Parameter_get_bool(par, "output.overwrite"));
		DataCube_save(mom1, Path_get(path_mom1), Parameter_get_bool(par, "output.overwrite"));
		DataCube_save(mom2, Path_get(path_mom2), Parameter_get_bool(par, "output.overwrite"));
		
		// Delete moment maps again
		DataCube_delete(mom0);
		DataCube_delete(mom1);
		DataCube_delete(mom2);
		
		// Print time
		timestamp(start_time);
	}
	
	
	
	// ---------------------------- //
	// Create and save cubelets     //
	// ---------------------------- //
	
	if(Parameter_get_bool(par, "output.writeCubelets"))
	{
		status("Creating cubelets");
		DataCube_create_cubelets(dataCube, maskCube, catalog, Path_get(path_cubelets), Parameter_get_bool(par, "output.overwrite"));
		// ALERT: CONTINUE HERE WITH IMPLEMENTATION OF CUBELETS ETC...
	}
	
	
	
	// ---------------------------- //
	// Clean up and exit            //
	// ---------------------------- //
	
	// Delete data cube and mask cube
	DataCube_delete(maskCube);
	DataCube_delete(dataCube);
	
	// Delete sub-cube region
	Array_delete(region);
	
	// Delete input parameters
	Parameter_delete(par);
	
	// Delete file paths
	Path_delete(path_data_in);
	Path_delete(path_cat_ascii);
	Path_delete(path_cat_xml);
	Path_delete(path_mask_out);
	Path_delete(path_noise);
	Path_delete(path_filtered);
	Path_delete(path_mom0);
	Path_delete(path_mom1);
	Path_delete(path_mom2);
	Path_delete(path_cubelets);
	
	// Delete source catalogue
	Catalog_delete(catalog);
	
	// Print time
	status("Pipeline finished.");
	
	return 0;
}
