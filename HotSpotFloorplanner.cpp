#include "HotSpotFloorplanner.hpp"
#include <string.h>

void HotSpotFloorplanner::initializeGlobalConfigToParameterTable(char* desc_file_name, 
																																	char* power_file_name, 
																																	char* output_file_name, 
																																	char* config_file_name)
{
	int count=0;
	strncpy(parameter_table[count].name,"f",STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	strncpy(parameter_table[count].value,desc_file_name,STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	++count;

	strncpy(parameter_table[count].name,"p",STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	strncpy(parameter_table[count].value,power_file_name,STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	++count;


	strncpy(parameter_table[count].name,"o",STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	strncpy(parameter_table[count].value,output_file_name,STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	++count;


	strncpy(parameter_table[count].name,"c",STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	strncpy(parameter_table[count].value,config_file_name,STR_SIZE-1);
	parameter_table[count].name[STR_SIZE-1] = '\0';
	++count;

	parameter_size=count;

}

void HotSpotFloorplanner::insertConfigParameter(char* name, char* value)
{
	if (parameter_size+1<MAX_ENTRIES)
	{
		strncpy(parameter_table[parameter_size].name,name,STR_SIZE-1);
		parameter_table[parameter_size].name[STR_SIZE-1] = '\0';
		strncpy(parameter_table[parameter_size].value,value,STR_SIZE-1);
		parameter_table[parameter_size].name[STR_SIZE-1] = '\0';
		++parameter_size;
		additional_thermal_config_from_strs();
		additional_flp_config_from_strs();
	}

}

HotSpotFloorplanner::HotSpotFloorplanner(char* desc_file_name,
																						char* power_file_name,
																						char* output_file_name,
																						char* config_file_name)
{
	// initialize parameter_table here...
	initializeGlobalConfigToParameterTable(desc_file_name,power_file_name,output_file_name,config_file_name);
	hsf_global_config_from_parameter_table();
	
	// default config of hotspot
	hsf_default_thermal_config();
	hsf_default_flp_config();

	//additional_thermal_config_from_strs();
	//additional_flp_config_from_strs();

	flp_desc=read_flp_desc(global_config.flp_desc,&flp_config);
	flp=flp_placeholder(flp_desc);
	model=alloc_RC_model(&thermal_config,flp);
	power=hotspot_vector(model);
	read_power(model,power,global_config.power_in);

}

HotSpotFloorplanner::~HotSpotFloorplanner()
{
	free_flp_desc(flp_desc);
	delete_RC_model(model);
	free_dvector(power);
	free_flp(flp,compacted);
}

void HotSpotFloorplanner::hsf_print_usage()
{
	fprintf(stdout, "Usage: HotSpotFloorplanner -f <file> -p <file> -o <file> [-c <file>] [-d <file>] [options]\n");
	fprintf(stdout, "Finds a thermally-aware floorplan for the given functional blocks.\n");
	fprintf(stdout, "Options:(may be specified in any order, within \"[]\" means optional)\n");
	fprintf(stdout, "   -f <file>\tfloorplan description input file (e.g. ev6.desc)\n");
	fprintf(stdout, "   -p <file>\tpower input file (e.g. avg.p)\n");
	fprintf(stdout, "   -o <file>\tfloorplan output file\n");
	fprintf(stdout, "  [-c <file>]\tinput configuration parameters from file (e.g. hotspot.config)\n");
	fprintf(stdout, "  [-d <file>]\toutput configuration parameters to file\n");
	fprintf(stdout, "  [options]\tzero or more options of the form \"-<name> <value>\",\n");
	fprintf(stdout, "           \toverride the options from config file\n");
}

void HotSpotFloorplanner::hsf_print_flp(char* flp_file){
	FILE *stream;
	// redirect stdout if fig file is not NULL
	if (flp_file)
	{
		if((stream=freopen(flp_file,"w",stdout))!=NULL){
			print_flp(flp);
			stream=freopen("CON","w",stdout);
		}
	}
	fclose(stream);
}

void HotSpotFloorplanner::hsf_print_flp_fig(char* flp_fig_file){
	
	// redirect stdout if fig file is not NULL
	if (flp_fig_file)
	{
		FILE *stream;
		if((stream=freopen(flp_fig_file,"w",stdout))!=NULL){
			print_flp_fig(flp);
			stream=freopen("CON","w",stdout);
		}
		fclose(stream);
	}else{
		print_flp_fig(flp);
	}
	
}

void HotSpotFloorplanner::hsf_print_flp_stats(char* flp_stats_file){

	// redirect stdout if stats file is not NULL
	if (flp_stats_file)
	{
		FILE *stream;
		if((stream=freopen(flp_stats_file,"w",stdout))!=NULL){
			print_flp_stats(flp,model,flp_config.l2_label,global_config.power_in,global_config.flp_desc);
			stream=freopen("CON","w",stdout);
		}
		fclose(stream);
	}else{
		print_flp_stats(flp,model,flp_config.l2_label,global_config.power_in,global_config.flp_desc);
	}
	
}

void HotSpotFloorplanner::hsf_print_wire_delays(char* wire_delay_file){
	
	// redirect stdout if wire delay file is not NULL
	if (wire_delay_file)
	{
		FILE *stream;
		if((stream=freopen(wire_delay_file,"w",stdout))!=NULL){
			print_wire_delays(flp,thermal_config.base_proc_freq);
			stream=freopen("CON","w",stdout);
		}
		fclose(stream);
	}else{
		print_wire_delays(flp,thermal_config.base_proc_freq);
	}
	
}

int HotSpotFloorplanner::hsf_floorplan()
{
	compacted=floorplan(flp,flp_desc,model,power);
	hsf_print_flp_fig();
	hsf_print_flp_stats();
	hsf_print_wire_delays();
	hsf_dump_flp();

	return compacted;
}





