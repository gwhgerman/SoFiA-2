#include <stdlib.h>

#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/wcsfix.h>

#include "WCS.h"



// Private properties and methods

CLASS WCS
{
	bool valid;
	struct wcsprm *wcs_pars;
	int  n_wcs_rep;
};

PRIVATE void WCS_setup(WCS *self, char *header, const int n_keys, const int n_axes, const int *dim_axes);



// Constructor

PUBLIC WCS *WCS_new(char *header, const int n_keys, const int n_axes, const int *dim_axes)
{
	// Sanity checks
	check_null(header);
	check_null(dim_axes);
	ensure(n_axes, "Failed to set up WCS; FITS header has no WCS axes.");
	
	WCS *self = (WCS *)malloc(sizeof(WCS));
	ensure(self != NULL, "Memory allocation error while creating WCS object.");
	
	self->valid = false;
	self->wcs_pars = NULL;
	self->n_wcs_rep = 0;
	
	WCS_setup(self, header, n_keys, n_axes, dim_axes);
	
	return self;
}



// Destructor

PUBLIC void WCS_delete(WCS *self)
{
	if(WCS_is_valid(self))
	{
		wcsvfree(&self->n_wcs_rep, &self->wcs_pars);
		wcsfree(self->wcs_pars);
		free(self->wcs_pars);
	}
	
	free(self);
	
	return;
}



// ----------------------------------------------- //
// Function to return whether valid WCS is defined //
// ----------------------------------------------- //

PUBLIC bool WCS_is_valid(const WCS *self)
{
	return self != NULL && self->wcs_pars != NULL && self->valid;
}



// Set up WCS information from header

PRIVATE void WCS_setup(WCS *self, char *header, const int n_keys, const int n_axes, const int *dim_axes)
{
	// Some variables needed by WCSlib
	int status = 0;
	int n_rejected = 0;
	int stat[NWCSFIX];
	
	// Allocate memory for wcsprm struct
	self->wcs_pars = (struct wcsprm *)calloc(1, sizeof(struct wcsprm));
	ensure(self->wcs_pars != NULL, "Memory allocation error while setting up WCS object.");
	self->wcs_pars->flag = -1;
	
	// Initialise wcsprm struct
	status = wcsini(true, n_axes, self->wcs_pars);
	
	// Parse the FITS header to fill in the wcsprm struct
	if(!status) status = wcspih(header, n_keys, 1, 0, &n_rejected, &self->n_wcs_rep, &self->wcs_pars);
	
	// Apply all necessary corrections to wcsprm struct
	// (missing cards, non-standard units or spectral types, etc.)
	if(!status) status = wcsfix(1, dim_axes, self->wcs_pars, stat);
	
	// Set up additional parameters in wcsprm struct derived from imported data
	if(!status) status = wcsset(self->wcs_pars);
	
	// Redo the corrections to account for things like NCP projections
	if(!status) status = wcsfix(1, dim_axes, self->wcs_pars, stat);
	
	if(status)
	{
		warning("WCSlib error %d: %s\n         Failed to parse WCS information.", status, wcs_errmsg[status]);
		self->valid = false;
	}
	else self->valid = true;
	
	return;
}



// --------------------------------------------------- //
// Function to convert from pixel to world coordinates //
// --------------------------------------------------- //

PUBLIC void WCS_convertToWorld(const WCS *self, const double x, const double y, const double z, double *longitude, double *latitude, double *spectral)
{
	// Sanity checks
	check_null(self);
	ensure(WCS_is_valid(self), "Failed to convert coordinates; no valid WCS definition found.");
	check_null(longitude);
	check_null(latitude);
	check_null(spectral);
	
	// Determine number of WCS axes
	const size_t n_axes = self->wcs_pars->naxis;
	ensure(n_axes, "Failed to convert coordinates; no valid WCS axes found.");
	
	// Allocate memory for coordinate arrays
	double *coord_pixel = (double *)malloc(n_axes * sizeof(double));
	double *coord_world = (double *)malloc(n_axes * sizeof(double));
	double *tmp_world   = (double *)malloc(n_axes * sizeof(double));
	ensure(coord_pixel != NULL && coord_world != NULL && tmp_world != NULL, "Memory allocation error during WCS conversion.");
	
	// Initialise pixel coordinates
	for(size_t i = 0; i < n_axes; ++i)
	{
		// NOTE: WCS pixel arrays are 1-based!!!
		if(i == 0)      coord_pixel[i] = 1.0 + x;
		else if(i == 1) coord_pixel[i] = 1.0 + y;
		else if(i == 2) coord_pixel[i] = 1.0 + z;
		else            coord_pixel[i] = 1.0;
	}
	
	// Declare a few variables
	double phi;
	double theta;
	int stat;
	
	// Call WCS conversion module
	int status = wcsp2s(self->wcs_pars, 1, n_axes, coord_pixel, tmp_world, &phi, &theta, coord_world, &stat);
	ensure(!status, "WCSlib error %d: %s", status, wcs_errmsg[status]);
	
	// Pass back world coordinates
	*longitude = coord_world[0];
	if(n_axes > 1) *latitude = coord_world[1];
	if(n_axes > 2) *spectral = coord_world[2];
	
	// Clean up
	free(coord_pixel);
	free(coord_world);
	free(tmp_world);
	
	return;
}



// --------------------------------------------------- //
// Function to convert from world to pixel coordinates //
// --------------------------------------------------- //

PUBLIC void WCS_convertToPixel(const WCS *self, const double longitude, const double latitude, const double spectral, double *x, double *y, double *z)
{
	// Sanity checks
	check_null(self);
	ensure(WCS_is_valid(self), "Failed to convert coordinates; no valid WCS definition found.");
	check_null(x);
	check_null(y);
	check_null(z);
	
	// Determine number of WCS axes
	const size_t n_axes = self->wcs_pars->naxis;
	ensure(n_axes, "Failed to convert coordinates; no valid WCS axes found.");
	
	// Allocate memory for coordinate arrays
	double *coord_pixel = (double *)malloc(n_axes * sizeof(double));
	double *coord_world = (double *)malloc(n_axes * sizeof(double));
	double *tmp_world   = (double *)malloc(n_axes * sizeof(double));
	ensure(coord_pixel != NULL && coord_world != NULL && tmp_world != NULL, "Memory allocation error during WCS conversion.");
	
	// Initialise pixel coordinates
	for(size_t i = 0; i < n_axes; ++i)
	{
		if(i == 0)      coord_world[i] = 1.0 + longitude;
		else if(i == 1) coord_world[i] = 1.0 + latitude;
		else if(i == 2) coord_world[i] = 1.0 + spectral;
		else            coord_world[i] = 1.0;
	}
	
	// Declare a few variables
	double phi;
	double theta;
	int stat;
	
	int status = wcss2p(self->wcs_pars, 1, n_axes, coord_world, &phi, &theta, tmp_world, coord_pixel, &stat);
	ensure(!status, "WCSlib error %d: %s", status, wcs_errmsg[status]);
	
	// Pass back pixel coordinates
	// NOTE: WCS pixel arrays are 1-based!!!
	*x = coord_pixel[0] - 1.0;
	if(n_axes > 1) *y = coord_pixel[1] - 1.0;
	if(n_axes > 2) *z = coord_pixel[2] - 1.0;
	
	// Clean up
	free(coord_pixel);
	free(coord_world);
	free(tmp_world);
	
	return;
}
