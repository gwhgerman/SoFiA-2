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

// WARNING: The following will only work on POSIX-compliant
//          systems, but is needed for mkdir().
#include <errno.h>
#include <sys/stat.h>

#include "src/common.h"
#include "src/Path.h"
#include "src/Array_dbl.h"
#include "src/Array_siz.h"
#include "src/Map.h"
#include "src/Matrix.h"
#include "src/Parameter.h"
#include "src/Catalog.h"
#include "src/DataCube.h"
#include "src/LinkerPar.h"



// ----------------------------------------------------------------- //
// This file contains the actual SoFiA pipeline that will read user  //
// parameters and data files, call the requested processing modules  //
// and write out catalogues and images.                              //
// ----------------------------------------------------------------- //

int main(int argc, char **argv)
{
	// ---------------------------- //
	// Record starting time         //
	// ---------------------------- //
	
	const time_t start_time = time(NULL);
	const clock_t start_clock = clock();
	
	
	
	// ---------------------------- //
	// A few global definitions     //
	// ---------------------------- //
	
	const char *noise_stat_name[] = {"standard deviation", "median absolute deviation", "Gaussian fit to flux histogram"};
	const char *flux_range_name[] = {"negative", "full", "positive"};
	noise_stat statistic = NOISE_STAT_STD;
	int range = 0;
	double global_rms = 1.0;
	
	
	
	// ---------------------------- //
	// Print basic information      //
	// ---------------------------- //
	
	status("Pipeline started");
	message("Using:   Source Finding Application (SoFiA)");
	message("Version: %s (%s)", SOFIA_VERSION, SOFIA_CREATION_DATE);
	message("Time:    %s", ctime(&start_time));
	
	
	
	// ---------------------------- //
	// Check command line arguments //
	// ---------------------------- //
	
	ensure(argc == 2, "Missing command line argument.\nUsage: %s <parameter_file>", argv[0]);
	
	
	
	// ---------------------------- //
	// Check SOFIA2_PATH variable   //
	// ---------------------------- //
	
	const char *ENV_SOFIA2_PATH = getenv("SOFIA2_PATH");
	// WARNING: ENV_SOFIA2_PATH points to static memory and must not be freed!
	// WARNING: Any subsequent call to getenv() might overwrite the content of
	//          the string that ENV_SOFIA2_PATH points to!
	ensure(ENV_SOFIA2_PATH != NULL,
		"Environment variable \'SOFIA2_PATH\' is not defined.\n"
		"       Please follow the instructions provided by the installation\n"
		"       script to define this variable before running SoFiA.");
	
	
	
	// ---------------------------- //
	// Load default parameters      //
	// ---------------------------- //
	
	status("Loading parameter settings");
	
	message("Loading SoFiA default parameter file.");
	
	Path *path_sofia = Path_new();
	Path_set_dir(path_sofia, ENV_SOFIA2_PATH);
	Path_set_file(path_sofia, "default_parameters.par");
	
	Parameter *par = Parameter_new(false);
	Parameter_load(par, Path_get(path_sofia), PARAMETER_APPEND);
	
	Path_delete(path_sofia);
	
	
	
	// ---------------------------- //
	// Load user parameters         //
	// ---------------------------- //
	
	message("Loading user parameter file: \'%s\'.", argv[1]);
	Parameter_load(par, argv[1], PARAMETER_UPDATE);
	
	
	
	// ---------------------------- //
	// Extract important settings   //
	// ---------------------------- //
	
	const bool verbosity         = Parameter_get_bool(par, "pipeline.verbose");
	const bool use_region        = strlen(Parameter_get_str(par, "input.region"))  ? true : false;
	const bool use_weights       = strlen(Parameter_get_str(par, "input.weights")) ? true : false;
	const bool use_mask          = strlen(Parameter_get_str(par, "input.mask"))    ? true : false;
	      bool use_flagging      = strlen(Parameter_get_str(par, "flag.region"))   ? true : false;
	const bool use_noise_scaling = Parameter_get_bool(par, "scaleNoise.enable");
	const bool use_scfind        = Parameter_get_bool(par, "scfind.enable");
	const bool use_threshold     = Parameter_get_bool(par, "threshold.enable");
	const bool use_reliability   = Parameter_get_bool(par, "reliability.enable");
	const bool use_rel_plot      = Parameter_get_bool(par, "reliability.plot");
	const bool use_parameteriser = Parameter_get_bool(par, "parameter.enable");
	const bool use_wcs           = Parameter_get_bool(par, "parameter.wcs");
	const bool use_physical      = Parameter_get_bool(par, "parameter.physical");
	
	const bool write_ascii       = Parameter_get_bool(par, "output.writeCatASCII");
	const bool write_xml         = Parameter_get_bool(par, "output.writeCatXML");
	const bool write_sql         = Parameter_get_bool(par, "output.writeCatSQL");
	const bool write_noise       = Parameter_get_bool(par, "output.writeNoise");
	const bool write_filtered    = Parameter_get_bool(par, "output.writeFiltered");
	const bool write_mask        = Parameter_get_bool(par, "output.writeMask");
	const bool write_moments     = Parameter_get_bool(par, "output.writeMoments");
	const bool write_cubelets    = Parameter_get_bool(par, "output.writeCubelets");
	const bool overwrite         = Parameter_get_bool(par, "output.overwrite");
	
	unsigned int autoflag_mode = 0;
	if     (strcmp(Parameter_get_str(par, "flag.auto"), "channels") == 0) autoflag_mode = 1;
	else if(strcmp(Parameter_get_str(par, "flag.auto"), "pixels")   == 0) autoflag_mode = 2;
	else if(strcmp(Parameter_get_str(par, "flag.auto"), "true")     == 0) autoflag_mode = 3;
	
	
	
	// ---------------------------- //
	// Define file names            //
	// ---------------------------- //
	
	const char *base_dir  = Parameter_get_str(par, "output.directory");
	const char *base_name = Parameter_get_str(par, "output.filename");
	
	Path *path_data_in = Path_new();
	Path_set(path_data_in, Parameter_get_str(par, "input.data"));
	
	Path *path_weights = Path_new();
	if(use_weights) Path_set(path_weights, Parameter_get_str(par, "input.weights"));
	
	Path *path_mask_in = Path_new();
	if(use_mask) Path_set(path_mask_in, Parameter_get_str(par, "input.mask"));
	
	Path *path_cat_ascii = Path_new();
	Path *path_cat_xml   = Path_new();
	Path *path_cat_sql   = Path_new();
	Path *path_noise     = Path_new();
	Path *path_filtered  = Path_new();
	Path *path_mask_out  = Path_new();
	Path *path_mask_2d   = Path_new();
	Path *path_mom0      = Path_new();
	Path *path_mom1      = Path_new();
	Path *path_mom2      = Path_new();
	Path *path_cubelets  = Path_new();
	Path *path_rel_plot  = Path_new();
	
	// Set directory names depending on user input
	if(strlen(base_dir))
	{
		// Use base directory if specified
		Path_set_dir(path_cat_ascii, base_dir);
		Path_set_dir(path_cat_xml,   base_dir);
		Path_set_dir(path_cat_sql,   base_dir);
		Path_set_dir(path_noise,     base_dir);
		Path_set_dir(path_filtered,  base_dir);
		Path_set_dir(path_mask_out,  base_dir);
		Path_set_dir(path_mask_2d,   base_dir);
		Path_set_dir(path_mom0,      base_dir);
		Path_set_dir(path_mom1,      base_dir);
		Path_set_dir(path_mom2,      base_dir);
		Path_set_dir(path_rel_plot,  base_dir);
		Path_set_dir(path_cubelets,  base_dir);
	}
	else if(strlen(Path_get_dir(path_data_in)))
	{
		// Use directory of input file if specified
		Path_set_dir(path_cat_ascii, Path_get_dir(path_data_in));
		Path_set_dir(path_cat_xml,   Path_get_dir(path_data_in));
		Path_set_dir(path_cat_sql,   Path_get_dir(path_data_in));
		Path_set_dir(path_noise,     Path_get_dir(path_data_in));
		Path_set_dir(path_filtered,  Path_get_dir(path_data_in));
		Path_set_dir(path_mask_out,  Path_get_dir(path_data_in));
		Path_set_dir(path_mask_2d,   Path_get_dir(path_data_in));
		Path_set_dir(path_mom0,      Path_get_dir(path_data_in));
		Path_set_dir(path_mom1,      Path_get_dir(path_data_in));
		Path_set_dir(path_mom2,      Path_get_dir(path_data_in));
		Path_set_dir(path_rel_plot,  Path_get_dir(path_data_in));
		Path_set_dir(path_cubelets,  Path_get_dir(path_data_in));
	}
	else
	{
		// Otherwise use current directory by default
		Path_set_dir(path_cat_ascii, ".");
		Path_set_dir(path_cat_xml,   ".");
		Path_set_dir(path_cat_sql,   ".");
		Path_set_dir(path_noise,     ".");
		Path_set_dir(path_filtered,  ".");
		Path_set_dir(path_mask_out,  ".");
		Path_set_dir(path_mask_2d,   ".");
		Path_set_dir(path_mom0,      ".");
		Path_set_dir(path_mom1,      ".");
		Path_set_dir(path_mom2,      ".");
		Path_set_dir(path_rel_plot,  ".");
		Path_set_dir(path_cubelets,  ".");
	}
	
	
	// Set file names depending on user input
	if(strlen(base_name))
	{
		// Use base name if specified
		Path_set_file_from_template(path_cat_ascii,  base_name, "_cat",      ".txt");
		Path_set_file_from_template(path_cat_xml,    base_name, "_cat",      ".xml");
		Path_set_file_from_template(path_cat_sql,    base_name, "_cat",      ".sql");
		Path_set_file_from_template(path_noise,      base_name, "_noise",    ".fits");
		Path_set_file_from_template(path_filtered,   base_name, "_filtered", ".fits");
		Path_set_file_from_template(path_mask_out,   base_name, "_mask",     ".fits");
		Path_set_file_from_template(path_mask_2d,    base_name, "_mask2d",   ".fits");
		Path_set_file_from_template(path_mom0,       base_name, "_mom0",     ".fits");
		Path_set_file_from_template(path_mom1,       base_name, "_mom1",     ".fits");
		Path_set_file_from_template(path_mom2,       base_name, "_mom2",     ".fits");
		Path_set_file_from_template(path_rel_plot,   base_name, "_rel",      ".eps");
		
		Path_append_dir_from_template(path_cubelets, base_name, "_cubelets");
		Path_set_file                (path_cubelets, base_name);
	}
	else
	{
		// Otherwise use input file name by default
		Path_set_file_from_template(path_cat_ascii,  Path_get_file(path_data_in), "_cat",      ".txt");
		Path_set_file_from_template(path_cat_xml,    Path_get_file(path_data_in), "_cat",      ".xml");
		Path_set_file_from_template(path_cat_sql,    Path_get_file(path_data_in), "_cat",      ".sql");
		Path_set_file_from_template(path_noise,      Path_get_file(path_data_in), "_noise",    ".fits");
		Path_set_file_from_template(path_filtered,   Path_get_file(path_data_in), "_filtered", ".fits");
		Path_set_file_from_template(path_mask_out,   Path_get_file(path_data_in), "_mask",     ".fits");
		Path_set_file_from_template(path_mask_2d,    Path_get_file(path_data_in), "_mask2d",   ".fits");
		Path_set_file_from_template(path_mom0,       Path_get_file(path_data_in), "_mom0",     ".fits");
		Path_set_file_from_template(path_mom1,       Path_get_file(path_data_in), "_mom1",     ".fits");
		Path_set_file_from_template(path_mom2,       Path_get_file(path_data_in), "_mom2",     ".fits");
		Path_set_file_from_template(path_rel_plot,   Path_get_file(path_data_in), "_rel",      ".eps");
		
		Path_append_dir_from_template(path_cubelets, Path_get_file(path_data_in), "_cubelets");
		Path_set_file_from_template  (path_cubelets, Path_get_file(path_data_in), "", "");
	}
	
	// ---------------------------- //
	// Check output settings        //
	// ---------------------------- //
	
	// Try to create cubelet directory
	if(write_cubelets)
	{
		errno = 0;
		mkdir(Path_get_dir(path_cubelets), 0755);
		ensure(errno == 0 || errno == EEXIST, "Failed to create cubelet directory; please check write permissions.");
	}
	
	// Check overwrite conditions
	if(!overwrite)
	{
		if(write_cubelets)
			ensure(errno != EEXIST,
				"Cubelet directory already exists. Please delete the directory\n       or set \'output.overwrite = true\'.");
		if(write_ascii)
			ensure(!Path_file_is_readable(path_cat_ascii),
				"ASCII catalogue file already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
		if(write_xml)
			ensure(!Path_file_is_readable(path_cat_xml),
				   "XML catalogue file already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
		if(write_sql)
			ensure(!Path_file_is_readable(path_cat_sql),
				   "SQL catalogue file already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
		if(write_noise)
			ensure(!Path_file_is_readable(path_noise),
				"Noise cube already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
		if(write_filtered)
			ensure(!Path_file_is_readable(path_filtered),
				"Filtered cube already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
		if(write_mask)
		{
			ensure(!Path_file_is_readable(path_mask_out),
				"Mask cube already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
			ensure(!Path_file_is_readable(path_mask_2d),
				"2-D mask cube already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
		}
		if(write_moments)
			ensure(!Path_file_is_readable(path_mom0) && !Path_file_is_readable(path_mom1) && !Path_file_is_readable(path_mom2),
				"Moment maps already exist. Please delete the files\n       or set \'output.overwrite = true\'.");
		if(use_rel_plot)
			ensure(!Path_file_is_readable(path_rel_plot),
				"Reliability plot already exists. Please delete the file\n       or set \'output.overwrite = true\'.");
	}
	
	
	
	// ---------------------------- //
	// Load data cube               //
	// ---------------------------- //
	
	// Set up region if required
	Array_siz *region = use_region ? Array_siz_new_str(Parameter_get_str(par, "input.region")) : NULL;
	
	// Set up flagging region if required
	Array_siz *flag_regions = use_flagging ? Array_siz_new_str(Parameter_get_str(par, "flag.region")) : Array_siz_new(0);
	
	// Load data cube
	status("Loading data cube");
	DataCube *dataCube = DataCube_new(verbosity);
	DataCube_load(dataCube, Path_get(path_data_in), region);
	if(use_flagging) DataCube_flag_regions(dataCube, flag_regions);
	
	// Print time
	timestamp(start_time, start_clock);
	
	
	
	// ---------------------------- //
	// Check linker settings        //
	// ---------------------------- //
	
	// Make sure that the minimum source size requirement does not exceed the axis size
	ensure(DataCube_get_axis_size(dataCube, 2) >= Parameter_get_uint(par, "linker.minSizeZ"), "Minimum source size (%ld) greater than z-axis size (%zu).\n       All detected sources would be discarded! Please set the\n       value of \'linker.minSizeZ\' to no greater than %zu.", Parameter_get_int(par, "linker.minSizeZ"), DataCube_get_axis_size(dataCube, 2), DataCube_get_axis_size(dataCube, 2));
	ensure(DataCube_get_axis_size(dataCube, 1) >= Parameter_get_uint(par, "linker.minSizeY"), "Minimum source size (%ld) greater than y-axis size (%zu).\n       All detected sources would be discarded! Please set the\n       value of \'linker.minSizeY\' to no greater than %zu.", Parameter_get_int(par, "linker.minSizeY"), DataCube_get_axis_size(dataCube, 1), DataCube_get_axis_size(dataCube, 1));
	ensure(DataCube_get_axis_size(dataCube, 0) >= Parameter_get_uint(par, "linker.minSizeX"), "Minimum source size (%ld) greater than x-axis size (%zu).\n       All detected sources would be discarded! Please set the\n       value of \'linker.minSizeX\' to no greater than %zu.", Parameter_get_int(par, "linker.minSizeX"), DataCube_get_axis_size(dataCube, 0), DataCube_get_axis_size(dataCube, 0));
	
	
	
	// ---------------------------- //
	// Load mask cube               //
	// ---------------------------- //
	
	DataCube *maskCube = NULL;
	
	if(use_mask)
	{
		// Load mask cube
		status("Loading mask cube");
		maskCube = DataCube_new(verbosity);
		DataCube_load(maskCube, Path_get(path_mask_in), region);
		
		// Ensure that mask has the right type and size
		ensure(DataCube_gethd_int(maskCube, "BITPIX") == 32, "Mask cube must be of 32-bit integer type.");
		ensure(
			DataCube_gethd_int(maskCube, "NAXIS1") == DataCube_gethd_int(dataCube, "NAXIS1") &&
			DataCube_gethd_int(maskCube, "NAXIS2") == DataCube_gethd_int(dataCube, "NAXIS2") &&
			DataCube_gethd_int(maskCube, "NAXIS3") == DataCube_gethd_int(dataCube, "NAXIS3"),
			"Data cube and mask cube have different sizes."
		);
		
		// Set all masked pixels to -1
		DataCube_reset_mask_32(maskCube, -1);
		
		// Apply flags to mask cube
		if(use_flagging) DataCube_flag_regions(maskCube, flag_regions);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	else
	{
		// Else create an empty mask cube
		maskCube = DataCube_blank(DataCube_get_axis_size(dataCube, 0), DataCube_get_axis_size(dataCube, 1), DataCube_get_axis_size(dataCube, 2), 32, verbosity);
		
		// Copy WCS header elements from data cube to mask cube
		DataCube_copy_wcs(dataCube, maskCube);
		
		// Set BUNIT keyword of mask cube
		DataCube_puthd_str(maskCube, "BUNIT", " ");
	}
	
	
	
	// ---------------------------- //
	// Load and apply weights cube  //
	// ---------------------------- //
	// ALERT: This will require some thought! What is the difference between noise variation and gain variation?
	//        What is the purpose of the weights cube, and at what stage should it be applied?
	
	if(use_weights)
	{
		status("Loading and applying weights cube");
		DataCube *weightsCube = DataCube_new(verbosity);
		DataCube_load(weightsCube, Path_get(path_weights), region);
		
		// Divide by weights cube
		DataCube_divide(dataCube, weightsCube);
		
		// Delete weights cube again
		DataCube_delete(weightsCube);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Scale data by noise level    //
	// ---------------------------- //
	
	if(use_noise_scaling)
	{
		status("Scaling data by noise");
		
		if(strcmp(Parameter_get_str(par, "scaleNoise.mode"), "local") == 0)
		{
			// Local noise scaling
			message("Correcting for local noise variations.");
			
			DataCube *noiseCube = DataCube_scale_noise_local(
				dataCube,
				Parameter_get_int(par, "scaleNoise.windowSpatial"),
				Parameter_get_int(par, "scaleNoise.windowSpectral"),
				Parameter_get_int(par, "scaleNoise.gridSpatial"),
				Parameter_get_int(par, "scaleNoise.gridSpectral"),
				Parameter_get_bool(par, "scaleNoise.interpolate")
			);
			
			if(write_noise)
			{
				// Apply flags to noise cube
				if(use_flagging) DataCube_flag_regions(noiseCube, flag_regions);
				DataCube_save(noiseCube, Path_get(path_noise), overwrite, DESTROY);
			}
			DataCube_delete(noiseCube);
		}
		else
		{
			// Global noise scaling along spectral axis
			message("Correcting for noise variations along spectral axis.\n");
			DataCube_scale_noise_spec(dataCube);
		}
	}
	else
	{
		// Measure global RMS if no noise scaling requested
		// This is necessary so the linker and reliability module can divide all flux values by the RMS later on.
		status("Measuring global noise level");
		size_t cadence = DataCube_get_size(dataCube) / 1000000;  // Stride for noise calculation
		if(cadence < 1) cadence = 1;
		global_rms = MAD_TO_STD * DataCube_stat_mad(dataCube, 0.0, cadence, -1);
		message("Global RMS:  %.3e  (using stride of %zu)", global_rms, cadence);
	}
	
	// Print time
	timestamp(start_time, start_clock);
	
	
	
	// ---------------------------- //
	// Automatic data flagging      //
	// ---------------------------- //
	
	if(autoflag_mode)
	{
		status("Auto-flagging");
		
		// Set up auto-flagging if requested
		Array_siz *autoflag_regions = Array_siz_new(0);
		DataCube_autoflag(dataCube, Parameter_get_flt(par, "flag.threshold"), autoflag_mode, autoflag_regions);
		
		// Apply flags if necessary
		if(Array_siz_get_size(autoflag_regions))
		{
			DataCube_flag_regions(dataCube, autoflag_regions);  // Apply auto-flagging regions
			Array_siz_cat(flag_regions, autoflag_regions);      // Append auto-flagging regions to general flagging regions
			use_flagging = true;                                // Update flagging switch
		}
		else message("No flagging required.");
		
		// Clean up
		Array_siz_delete(autoflag_regions);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Write filtered cube          //
	// ---------------------------- //
	
	if(write_filtered && (use_weights || use_flagging || use_noise_scaling))  // ALERT: Add conditions here as needed.
	{
		status("Writing filtered cube");
		DataCube_save(dataCube, Path_get(path_filtered), overwrite, PRESERVE);
	}
	
	
	
	// ---------------------------- //
	// Run source finder            //
	// ---------------------------- //
	
	// Terminate if no source finder is run, but no input mask is provided either
	ensure(use_scfind || use_threshold || use_mask, "No mask provided and no source finder selected. Cannot proceed.");
	
	// S+C finder
	if(use_scfind)
	{
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
		
		Array_dbl *kernels_spat = Array_dbl_new_str(Parameter_get_str(par, "scfind.kernelsXY"));
		Array_siz *kernels_spec = Array_siz_new_str(Parameter_get_str(par, "scfind.kernelsZ"));
		
		// Run S+C finder to obtain mask
		DataCube_run_scfind(
			dataCube,
			maskCube,
			kernels_spat,
			kernels_spec,
			Parameter_get_flt(par, "scfind.threshold"),
			Parameter_get_flt(par, "scfind.replacement"),
			statistic,
			range,
			start_time,
			start_clock
		);
		
		// Clean up
		Array_dbl_delete(kernels_spat);
		Array_siz_delete(kernels_spec);
		
		// Apply flags to mask cube
		if(use_flagging) DataCube_flag_regions(maskCube, flag_regions);
	}
	
	// Threshold finder
	if(use_threshold)
	{
		// Determine mode
		const bool absolute = (strcmp(Parameter_get_str(par, "threshold.mode"), "absolute") == 0);
		
		// Determine noise measurement method to use
		statistic = NOISE_STAT_STD;
		if(strcmp(Parameter_get_str(par, "threshold.statistic"), "mad") == 0) statistic = NOISE_STAT_MAD;
		else if(strcmp(Parameter_get_str(par, "threshold.statistic"), "gauss") == 0) statistic = NOISE_STAT_GAUSS;
		
		// Determine flux range to use
		range = 0;
		if(strcmp(Parameter_get_str(par, "threshold.fluxRange"), "negative") == 0) range = -1;
		else if(strcmp(Parameter_get_str(par, "threshold.fluxRange"), "positive") == 0) range = 1;
		
		status("Running threshold finder");
		message("Using the following parameters:");
		message("- Mode:             %s", absolute ? "absolute" : "relative");
		message("- Flux threshold:   %s%s", Parameter_get_str(par, "threshold.threshold"), absolute ? "" : " * rms");
		if(!absolute)
		{
			message("- Noise statistic:  %s", noise_stat_name[statistic]);
			message("- Flux range:       %s", flux_range_name[range + 1]);
		}
		
		// Run threshold finder
		DataCube_run_threshold(
			dataCube,
			maskCube,
			absolute,
			Parameter_get_flt(par, "threshold.threshold"),
			statistic,
			range
		);
		
		// Apply flags to mask cube
		if(use_flagging) DataCube_flag_regions(maskCube, flag_regions);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Run linker                   //
	// ---------------------------- //
	
	status("Running Linker");
	
	const bool remove_neg_src = !use_reliability;  // ALERT: Add conditions here as needed.
	
	LinkerPar *lpar = DataCube_run_linker(
		dataCube,
		maskCube,
		Parameter_get_int(par, "linker.radiusX"),
		Parameter_get_int(par, "linker.radiusY"),
		Parameter_get_int(par, "linker.radiusZ"),
		Parameter_get_int(par, "linker.minSizeX"),
		Parameter_get_int(par, "linker.minSizeY"),
		Parameter_get_int(par, "linker.minSizeZ"),
		Parameter_get_int(par, "linker.maxSizeX"),
		Parameter_get_int(par, "linker.maxSizeY"),
		Parameter_get_int(par, "linker.maxSizeZ"),
		remove_neg_src,
		global_rms
	);
	
	// Print time
	timestamp(start_time, start_clock);
	
	// Terminate pipeline if no sources left after linking
	const size_t lpar_size = LinkerPar_get_size(lpar);
	ensure(lpar_size, "No sources left after linking. Terminating pipeline.");
	
	
	
	// ---------------------------- //
	// Run reliability filter       //
	// ---------------------------- //
	
	Map *rel_filter = Map_new();  // Empty container for storing old and new labels of reliable sources
	
	if(use_reliability)
	{
		status("Measuring Reliability");
		
		// Calculate reliability values
		Matrix *covar = LinkerPar_reliability(lpar, Parameter_get_flt(par, "reliability.scaleKernel"), Parameter_get_flt(par, "reliability.fmin"));
		
		// Create plots if requested
		if(use_rel_plot) LinkerPar_rel_plots(lpar, Parameter_get_flt(par, "reliability.threshold"), Parameter_get_flt(par, "reliability.fmin"), covar, Path_get(path_rel_plot), overwrite);
		
		// Delete covariance matrix again
		Matrix_delete(covar);
		
		// Set up relabelling filter by recording old and new label pairs of reliable sources
		size_t new_label = 1;
		
		for(size_t i = 0; i < lpar_size; ++i)
		{
			const size_t old_label = LinkerPar_get_label(lpar, i);
			
			// Keep source if reliability > threshold and fmin parameter satisfied
			if(LinkerPar_get_rel(lpar, old_label) >= Parameter_get_flt(par, "reliability.threshold")
			&& LinkerPar_get_flux(lpar, old_label) / sqrt(LinkerPar_get_npix(lpar, old_label)) > Parameter_get_flt(par, "reliability.fmin"))
			{
				Map_push(rel_filter, old_label, new_label++);
			}
		}
		
		// Check if any reliable sources left
		ensure(Map_get_size(rel_filter), "No reliable sources found. Terminating pipeline.");
		
		// Apply filter to mask cube, so unreliable sources are removed
		// and reliable ones relabelled in consecutive order
		DataCube_filter_mask_32(maskCube, rel_filter);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Create initial catalogue     //
	// ---------------------------- //
	
	// Extract flux unit from header
	String *unit_flux = String_trim(DataCube_gethd_string(dataCube, "BUNIT"));
	if(String_size(unit_flux) == 0)
	{
		warning("No flux unit (\'BUNIT\') defined in header.");
		String_set(unit_flux, "???");
	}
	
	// Generate catalogue of reliable sources from linker output
	Catalog *catalog = LinkerPar_make_catalog(lpar, rel_filter, String_get(unit_flux));
	
	// Delete linker parameters, reliability filter and flux unit string, as they are no longer needed
	LinkerPar_delete(lpar);
	Map_delete(rel_filter);
	String_delete(unit_flux);
	
	// Terminate if catalogue is empty
	ensure(Catalog_get_size(catalog), "No reliable sources found. Terminating pipeline.");
	
	
	
	// ---------------------------- //
	// Reload data cube if required //
	// ---------------------------- //
	
	if(use_weights || use_noise_scaling)  // ALERT: Add conditions here as needed.
	{
		status("Reloading data cube for parameterisation");
		DataCube_load(dataCube, Path_get(path_data_in), region);
		
		if(use_flagging) DataCube_flag_regions(dataCube, flag_regions);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Parameterise sources         //
	// ---------------------------- //
	
	if(use_parameteriser)
	{
		status("Measuring source parameters");
		DataCube_parameterise(dataCube, maskCube, catalog, use_wcs, use_physical, Parameter_get_str(par, "parameter.prefix"));
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Save catalogue(s)            //
	// ---------------------------- //
	
	if(write_ascii || write_xml || write_sql)
	{
		status("Writing source catalogue");
		
		if(write_ascii)
		{
			message("Writing ASCII file:   %s", Path_get_file(path_cat_ascii));
			Catalog_save(catalog, Path_get(path_cat_ascii), CATALOG_FORMAT_ASCII, overwrite);
		}
		
		if(write_xml)
		{
			message("Writing VOTable file: %s", Path_get_file(path_cat_xml));
			Catalog_save(catalog, Path_get(path_cat_xml), CATALOG_FORMAT_XML, overwrite);
		}
		
		if(write_sql)
		{
			message("Writing SQL file:     %s", Path_get_file(path_cat_sql));
			Catalog_save(catalog, Path_get(path_cat_sql), CATALOG_FORMAT_SQL, overwrite);
		}
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Create and save moment maps  //
	// ---------------------------- //
	
	if(write_moments)
	{
		status("Creating moment maps");
		
		// Generate moment maps
		DataCube *mom0 = NULL;
		DataCube *mom1 = NULL;
		DataCube *mom2 = NULL;
		DataCube_create_moments(dataCube, maskCube, &mom0, &mom1, &mom2, use_wcs);
		
		// Save moment maps to disk
		if(mom0 != NULL) DataCube_save(mom0, Path_get(path_mom0), overwrite, DESTROY);
		if(mom1 != NULL) DataCube_save(mom1, Path_get(path_mom1), overwrite, DESTROY);
		if(mom2 != NULL) DataCube_save(mom2, Path_get(path_mom2), overwrite, DESTROY);
		
		// Delete moment maps again
		DataCube_delete(mom0);
		DataCube_delete(mom1);
		DataCube_delete(mom2);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Create and save cubelets     //
	// ---------------------------- //
	
	if(write_cubelets)
	{
		status("Creating cubelets");
		DataCube_create_cubelets(dataCube, maskCube, catalog, Path_get(path_cubelets), overwrite, use_wcs, use_physical, Parameter_get_int(par, "output.marginCubelets"));
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Save mask cube               //
	// ---------------------------- //
	
	if(write_mask)
	{
		status("Writing mask cube");
		
		// Create and save projected 2-D mask image
		DataCube *maskImage = DataCube_2d_mask(maskCube);
		DataCube_save(maskImage, Path_get(path_mask_2d), overwrite, DESTROY);
		DataCube_delete(maskImage);
		
		// Write 3-D mask cube
		DataCube_save(maskCube, Path_get(path_mask_out), overwrite, DESTROY);
		
		// Print time
		timestamp(start_time, start_clock);
	}
	
	
	
	// ---------------------------- //
	// Clean up and exit            //
	// ---------------------------- //
	
	// Delete data cube and mask cube
	DataCube_delete(maskCube);
	DataCube_delete(dataCube);
	
	// Delete sub-cube region
	Array_siz_delete(region);
	
	// Delete flagging regions
	Array_siz_delete(flag_regions);
	
	// Delete input parameters
	Parameter_delete(par);
	
	// Delete file paths
	Path_delete(path_data_in);
	Path_delete(path_weights);
	Path_delete(path_mask_in);
	Path_delete(path_cat_ascii);
	Path_delete(path_cat_xml);
	Path_delete(path_cat_sql);
	Path_delete(path_mask_out);
	Path_delete(path_mask_2d);
	Path_delete(path_noise);
	Path_delete(path_filtered);
	Path_delete(path_mom0);
	Path_delete(path_mom1);
	Path_delete(path_mom2);
	Path_delete(path_rel_plot);
	Path_delete(path_cubelets);
	
	// Delete source catalogue
	Catalog_delete(catalog);
	
	// Print status message
	status("Pipeline finished.");
	
	return 0;
}
