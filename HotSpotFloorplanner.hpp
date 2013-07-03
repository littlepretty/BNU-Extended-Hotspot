#include "flp.h"
#include "temperature.h"
#include "hotfloorplan.h"
#include <stdio.h>
#include <stdlib.h>

class HotSpotFloorplanner
{
public:

	/************************************************************************/
	/* Constructor with several configuration files' name, include:
			desc flp file name
			average power file name
			flp output file name
			configuration file name
	*/
	/************************************************************************/
	HotSpotFloorplanner(char* desc_file_name,
												char* power_file_name,
												char* output_file_name,
												char* config_file_name);

	/************************************************************************/
	/* Release HotSpot objects in deconstructor                                                          */
	/************************************************************************/
	~HotSpotFloorplanner();

private:

	/************************************************************************/
	/* Setup global config from parameter table                                                         */
	/************************************************************************/
	void hsf_global_config_from_parameter_table(){
		global_config_from_strs(&global_config,parameter_table,parameter_size);
	}

	/************************************************************************/
	/* Write global configs back to parameter table                                                   */
	/************************************************************************/
	void hsf_global_config_to_parameter_table(){
		global_config_to_strs(&global_config,parameter_table,MAX_ENTRIES);
	}

	/************************************************************************/
	/* Dump result floorplan file                                                                                       */
	/************************************************************************/
	void hsf_dump_flp(){
		dump_flp(flp,global_config.flp_out,FALSE);
	}

	/************************************************************************/
	/* Just use default thermal config first, custom setup can be added then  */
	/************************************************************************/
	void hsf_default_thermal_config(){
		thermal_config=default_thermal_config();
	}

	/***************************************************************************/
	/* Just use default floorplan config first, custom setup can be added then     */
	/***************************************************************************/
	void hsf_default_flp_config(){
		flp_config=default_flp_config();
	}

	/************************************************************************/
	/* At least these global configuration parameters should be given:
			desc flp file
			average power file
			output destination 
			config file*/
	/************************************************************************/
	void initializeGlobalConfigToParameterTable(char* desc_file_name,
																								char* power_file_name,
																								char* output_file_name,
																								char* config_file_name);

	/********************************************************************************************/
	/* After insert parameters into parameter table, update thermal && flp config respectively    */
	/********************************************************************************************/
	void additional_thermal_config_from_strs(){
		thermal_config_add_from_strs(&thermal_config,&parameter_table[parameter_size],MAX_ENTRIES-parameter_size);
	}
	void additional_flp_config_from_strs(){
		flp_config_add_from_strs(&flp_config,&parameter_table[parameter_size],MAX_ENTRIES-parameter_size);
	}
	
public:

	/************************************************************************/
	/* Tell about myself                                                                                                         */
	/************************************************************************/
	void hsf_print_usage();

	/************************************************************************/
	/* This is the function you should call 
			when you want to associate some value with a configuration at your will*/
	/************************************************************************/
	void insertConfigParameter(char* name, char* value);

	/************************************************************************/
	/* Do the floorplan algorithm and print informations on console                */
	/************************************************************************/
    int hsf_floorplan();

	/************************************************************************/
	/* Also you can print extra auxiliary results from floorplan process
			with no filename given, it will only show on console
			whereas giving a filename redirects stdout into that file*/
	/************************************************************************/
	void hsf_print_flp(char* flp_file=NULL);
	void hsf_print_flp_fig(char* flp_fig_file=NULL);
	void hsf_print_flp_stats(char* flp_stats_file=NULL);
	void hsf_print_wire_delays(char* wire_delay_file=NULL);

	/************************************************************************/
	/* For your convenience if you want all auxiliary informations                         */
	/************************************************************************/
	void hsf_print_auxilary_results(char* flp_fig_file,char* flp_stats_file,char* wire_delay_file){
		hsf_print_flp_fig(flp_fig_file);
		hsf_print_flp_stats(flp_stats_file);
		hsf_print_wire_delays(wire_delay_file);
	}


private:
	int compacted;

	/************************************************************************/
	/* Parameter table stores key-value pair that specifies configuration         */
	/************************************************************************/
	str_pair parameter_table[MAX_ENTRIES];
	/************************************************************************/
	/* Track the parameter table's size 
			because you can add in new configurations so that modify default values */
	/************************************************************************/
	int parameter_size;

	/************************************************************************/
	/* HotSpot objects needed in floorplanning                                                          */
	/************************************************************************/
	flp_desc_t *flp_desc;
	flp_t *flp;
	RC_model_t *model;
	double *power;

	thermal_config_t thermal_config;
	flp_config_t flp_config;
	global_config_t global_config;

};

