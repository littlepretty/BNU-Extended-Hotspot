#pragma once

#include "flp.h"
#include "temperature.h"


class HotSpot
{
public:
	/*
	 *	Constructor and Destructor, do the memory allocate and deallocate automatically
	 */
	HotSpot(void);
	HotSpot(	char* flp_file,
						char* power_file,
						char* init_temp_file,
						char* steady_temp_file,
						char* trace_temp_file,
						double delta_time);

	virtual ~HotSpot(void);

	/*
	 *	Pre-setup before computing 
	 */
	void hs_setup(bool temp_clip=0, 
									bool use_default=0,
									double heaksink_side=60e-3,
									double spreader_side=30e-3,
									bool r_update_used=0,
									bool leakage_used=0,
									bool leakage_mode=0,
									bool grid_model=false,
									int grid_rows=64,
									int grid_cols=64);

	/*
	 *	Transient temperature analysis
	 */
	void hs_compute_temp();

	/*
	 * Update R/G when temperature changes
	 */
	void hs_update_R_model();

	/*
	 *	Steady temperature analysis
	 */
	void hs_steady_state_temp();

	/*
	 *	Dump temperature analysis result into files
	 */
	void hs_dump_temp();

private:

	/*
	 *	Helper functions for construct necessary objects for HotSpot
	 */
	flp_t* hs_read_flp(bool read_connects);
	thermal_config_t hs_default_thermal_config();
	thermal_config_t hs_custom_thermal_config(double heatsink_side, double spreader_side, bool r_update_used, bool leakage_used, bool leakage_mode,bool grid_model, int grid_rows, int grid_cols);
	RC_model_t* hs_alloc_RC_model();

	/*
	 *	Helper functions for setup operation
	 */
	void hs_read_power();
	void hs_read_temp(bool clip);
	void hs_populate_R_model();
	void hs_populate_C_model();

	/*
	 *	Helper functions for destroying all objects for HotSpot
	 */
	void hs_delete_RC_model();
	void hs_free_flp(bool compacted);
	void hs_free_dvector(double* vec);

	/*
	 *	Self-explaining file name variables
	 */
	char* flp_file_name;
	char* power_file_name;
	char* init_temp_file_name;
	char* steady_temp_file_name;
	char* trace_temp_file_name;

	/*
	 *	Structs or variables for HotSpot model and configures
	 */
	flp_t *flp;
	RC_model_t *model;
	thermal_config_t config;
	double *power, *temp;

	//	Transient analysis time step
	double delta_t;

};

