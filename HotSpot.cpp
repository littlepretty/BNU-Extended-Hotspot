#include "HotSpot.h"


HotSpot::HotSpot(void)
{
	flp_file_name="ev6.flp";
	power_file_name="ev6_steady.ptrace";
	init_temp_file_name="ev6.init";
	steady_temp_file_name="ev6.steady";
	trace_temp_file_name="ev6.ttrace";

	delta_t=0.001;

	/*
	flp=hs_read_flp(false);
	config=hs_default_thermal_config();
	model=hs_alloc_RC_model();
	temp = hotspot_vector(model);
	power = hotspot_vector(model);
	*/
}

HotSpot::HotSpot(	char* flp_file,
										char* power_file,
										char* init_temp_file,
										char* steady_temp_file,
										char* trace_temp_file,
										double delta_time)
{
	flp_file_name=flp_file;
	power_file_name=power_file;
	init_temp_file_name=init_temp_file;
	steady_temp_file_name=steady_temp_file;
	trace_temp_file_name=trace_temp_file;

	delta_t=delta_time;
}


HotSpot::~HotSpot(void)
{
	hs_delete_RC_model();
	hs_free_flp(false);
	hs_free_dvector(power);
	hs_free_dvector(temp);
}

flp_t* HotSpot::hs_read_flp(bool read_connected)
{
	return read_flp(flp_file_name,read_connected);
}

void HotSpot::hs_read_power()
{
	read_power(model,power,power_file_name);
}

void HotSpot::hs_read_temp(bool clip)
{
	read_temp(model, temp, init_temp_file_name, clip);
}

thermal_config_t HotSpot::hs_default_thermal_config()
{
	return default_thermal_config();
}

thermal_config_t HotSpot::hs_custom_thermal_config(double heatsink_side, double spreader_side, bool r_update_used, bool leakage_used, bool leakage_model, bool grid_model, int grid_rows, int grid_cols)
{
	return custom_thermal_config(heatsink_side,spreader_side, r_update_used, leakage_used,leakage_model,grid_model,grid_rows,grid_cols);
}

RC_model_t* HotSpot::hs_alloc_RC_model()
{
	return alloc_RC_model(&config,flp);
}

void HotSpot::hs_populate_R_model()
{
	populate_R_model(model,flp);
}

void HotSpot::hs_populate_C_model()
{
	populate_C_model(model,flp);
}

void HotSpot::hs_delete_RC_model()
{
	delete_RC_model(model);
}
void HotSpot::hs_free_flp(bool compacted)
{
	free_flp(flp,compacted);
}
void HotSpot::hs_free_dvector(double* vec)
{
	free_dvector(vec);
}

void HotSpot::hs_setup(bool temp_clip, bool use_default,
												double heaksink_side,
												double spreader_side,
												bool r_update_used,
												bool leakage_used,
												bool leakage_mode,
												bool grid_model,
												int grid_rows,
												int grid_cols)
{
	flp=hs_read_flp(false);
	if (use_default)
		config=hs_default_thermal_config();
	else
		config=hs_custom_thermal_config(heaksink_side,spreader_side, r_update_used, leakage_used,leakage_mode,grid_model,grid_rows,grid_cols);
	
	model=hs_alloc_RC_model();
	temp = hotspot_vector(model);
	power = hotspot_vector(model);

	hs_read_power();
	hs_read_temp(temp_clip);

	hs_populate_R_model();
	hs_populate_C_model();
}

void HotSpot::hs_update_R_model()
{
	update_R_model(model, flp, temp);
}

void HotSpot::hs_compute_temp()
{
	compute_temp(model, power, temp, delta_t);
}
void HotSpot::hs_steady_state_temp()
{
	steady_state_temp(model, power, temp);
}
void HotSpot::hs_dump_temp()
{
	dump_temp(model, temp, steady_temp_file_name);
}