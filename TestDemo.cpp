#include "HotSpot.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include "BlockLevelModel.h"
#include "PreExtract.hpp"
#include "FlpInfo.h"
#include "SelfMadeTimer.h"
#include "hotfloorplan.h"
#include "flp_desc.h"
#include "HotSpotFloorplanner.hpp"


using namespace std;

void NoConductivityUpdateDemo()
{
	double delta_time=0.001;
	HotSpot hs("ev6.flp", 
		"ev6_steady.ptrace", 
		"amb_ev6.init", 
		"ev6.steady_no_r_update", 
		"ev6_grid.ttrace", 
		delta_time);

	bool temp_clip=false;
	double heaksink_side=60e-3;
	double spreader_side=30e-3;

	bool use_default=false;
	bool r_update_used=false;
	bool leakage_used=false;
	bool leakage_mode=false;
	bool grid_model=false;
	int grid_rows=128;
	int grid_cols=128;

	hs.hs_setup(temp_clip,
		use_default,
		heaksink_side,
		spreader_side,
		r_update_used, 
		leakage_used,
		leakage_mode,
		grid_model,
		grid_rows,
		grid_cols);

	clock_t beginTime=clock();
	hs.hs_steady_state_temp();
	clock_t eclipsedTime=clock()-beginTime;
	hs.hs_dump_temp();

	cout<<"Time eclipsed is "<<(double)eclipsedTime/CLOCKS_PER_SEC<<" seconds"<<endl;
}
void ConductivityUpdateDemo()
{
	double delta_time=0.001;
	HotSpot hs("ev6.flp", 
		"ev6_steady.ptrace", 
		"amb_ev6.init", 
		"ev6.steady_r_update", 
		"ev6_grid.ttrace", 
		delta_time);

	bool temp_clip=false;
	double heaksink_side=60e-3;
	double spreader_side=30e-3;

	bool use_default=false;
	bool r_update_used=true;
	bool leakage_used=false;
	bool leakage_mode=false;
	bool grid_model=false;
	int grid_rows=128;
	int grid_cols=128;

	hs.hs_setup(temp_clip,
		use_default,
		heaksink_side,
		spreader_side,
		r_update_used, 
		leakage_used,
		leakage_mode,
		grid_model,
		grid_rows,
		grid_cols);

	clock_t beginTime=clock();
	hs.hs_steady_state_temp();
	clock_t eclipsedTime=clock()-beginTime;
	hs.hs_dump_temp();

	cout<<"Time eclipsed is "<<(double)eclipsedTime/CLOCKS_PER_SEC<<" seconds"<<endl;
}

void HotSpotConsiderConductivityDemo()
{
	/************************************************************************/
	/*	Compare that, whether R is updated will effect the thermal result       */
	/************************************************************************/
	NoConductivityUpdateDemo();
	ConductivityUpdateDemo();
}

void PreExtractDemo()
{
	/**********************************************************************************************/
	/*	This is an example of how to use my PreExtract model.
			It is a little bit complicated because it should cooperate with HotSpot, BlockLevelModel and
			FlpInfo classes                                                                                                                                                      */
	/**********************************************************************************************/
	// three *block.flp are available for test
	char* flp_file="i7x_blocks.flp";
	//char* flp_file="four_cores_blocks.flp";
	//char* flp_file="sixteen_cores_blocks.flp";
	
	// create floorplan infomation
	FlpInfo flp;
	flp.readWHLBInfoFromFile(flp_file);
	flp.calculateCenters();
	flp.calculateAreas();

	double ux=2e-3;
	double uy=2e-3;
	double w=flp.getChipWidth();
	double h=flp.getChipWidth();

	PreExtract preExtract(w,h,ux,uy,4);

	if(preExtract.validateSizeAndUnit())
	{

		std::cout<<"valid flp info with this PreExtract model success, continuing..."<<std::endl;
		// if PreExtract is valid, we can then extrace and convert
		preExtract();

		preExtract.convertWithFlpInfo(flp);
		
		// ready to configure BlockLevelModel
		BlockLevelModel blk(flp.num_blks);
		preExtract.configureBlockLevelModelRI(blk);

		/*
		char* newPower="random_power.txt";
		char* newTemSteady="tem_steady.txt";
		blk.ComputeTemperatureWithSuchPower(newPower);
		blk.OutputComputeResultsToFile(newTemSteady);
		*/
	}else{
		std::cout<<"valid flp info with this PreExtract model fails..."<<std::endl;
	}
	
}

void HotSpotDemo()
{
	/************************************************************************/
	/*	This is an example of how to use my encapsulated HotSpot code         */
	/************************************************************************/
	double delta_time=0.001;
	//HotSpot hs;
	HotSpot hs("ev6.flp", "ev6_steady.ptrace", "ev6.init", "ev6.steady", "ev6_grid.ttrace", delta_time);

	// specifically setup
	bool temp_clip=false;

	bool use_default=false;

	double heaksink_side=60e-3;
	double spreader_side=30e-3;
	bool r_update_used=false;
	bool leakage_used=false;
	bool leakage_mode=false;
	bool grid_model=false;
	int grid_rows=128;
	int grid_cols=128;

	hs.hs_setup(temp_clip,use_default,heaksink_side,spreader_side,r_update_used, leakage_used,leakage_mode,grid_model,grid_rows,grid_cols);

	clock_t beginTime=clock();
	hs.hs_steady_state_temp();
	clock_t eclipsedTime=clock()-beginTime;
	hs.hs_dump_temp();

	cout<<"Time eclipsed is "<<(double)eclipsedTime/CLOCKS_PER_SEC<<" seconds"<<endl;
}

void BlockLevelModelDemo()
{
	/************************************************************************/
	/*  Example of Block Level Model                                                                              */
	/************************************************************************/
	
	int blocks=7;
	BlockLevelModel blm(blocks);

	blm.ReadAreaDataFromFile("Areas.txt");

	blm.ReadNormalTemperatureDataFromFile();
	blm.ExtractBlockLevelParameterMatrix();

	// Test with a power profile
	blm.ComputeTemperatureWithSuchPower("RandomPower.txt");
	blm.OutputComputeResultsToFile();
	
}

void SelfMadeTimerDemo()
{
	/************************************************************************/
	/*	This is an example of how to use SelfMadeTimer with HotSpot             */
	/************************************************************************/
	double delta_time=0.001;
	HotSpot hs("ev6.flp", "ev6_steady.ptrace", "ev6.init", "ev6.t_steady", "ev6_grid.ttrace", delta_time);

	// specifically setup
	bool temp_clip=false;
	bool use_default=false;
	double heaksink_side=60e-3;
	double spreader_side=30e-3;
	bool r_update_used=false;
	bool leakage_used=false;
	bool leakage_mode=false;
	bool grid_model=true;
	int grid_rows=128;
	int grid_cols=128;

	hs.hs_setup(temp_clip,use_default,heaksink_side,spreader_side,r_update_used, leakage_used,leakage_mode,grid_model,grid_rows,grid_cols);

	std::string projectName("HotSpotGridPractice");
	SelfMadeTimer myTimer(projectName);
	int runTimes=4;
	std::string task_name;

	for (int i=0;i<runTimes;i++)
	{
		std::ostringstream stringStream;
		stringStream<<"run_hotspot_"<<i;
		task_name=stringStream.str();

		myTimer.startTimer(task_name);
		hs.hs_steady_state_temp();
		myTimer.pauseTimer();

		myTimer.printLastEclipsedTime(task_name);

		hs.hs_dump_temp();
	}
	myTimer.printOverallEclipsedTime();
	myTimer.resetTimer();

	for (int i=0;i<runTimes/2;i++)
	{
		std::ostringstream stringStream;
		stringStream<<"run_hotspot_"<<i;
		task_name=stringStream.str();

		myTimer.startTimer(task_name);
		hs.hs_steady_state_temp();
		myTimer.pauseTimer();

		myTimer.printLastEclipsedTime(task_name);

		hs.hs_dump_temp();
	}
	myTimer.printAverageElipsedTime();

}

void OriginalHotFloorplanDemo()
{
	flp_desc_t *flp_desc;
	flp_t *flp;
	RC_model_t *model;
	double *power;
	thermal_config_t thermal_config;
	flp_config_t flp_config;
	global_config_t global_config;
	str_pair table[MAX_ENTRIES];
	int size, compacted;

	//if (!(argc >= 7 && argc % 2)) {
	//	usage(argc, argv);
	//	return 1;
	//}
	int argc=9;
	char** argv=new char*[9];
	argv[0]="hotfloorplan",argv[1]="-f",argv[2]="ev6.desc",argv[3]="-p", argv[4]="avg.p",
		argv[5]="-o", argv[6]="demo.flp",argv[7]="-c",argv[8]= "hotspot.config";
	
	size = parse_cmdline(table, MAX_ENTRIES, argc, argv);
	global_config_from_strs(&global_config, table, size);

	/* read configuration file	*/
	if (strcmp(global_config.config, NULLFILE))
		size += read_str_pairs(&table[size], MAX_ENTRIES, global_config.config);

	/* 
	 * in the str_pair 'table', earlier entries override later ones.
	 * so, command line options have priority over config file 
	 */
	size = str_pairs_remove_duplicates(table, size);

	/* get defaults */
	thermal_config = default_thermal_config();
	flp_config = default_flp_config();
	/* modify according to command line / config file	*/
	thermal_config_add_from_strs(&thermal_config, table, size);
	flp_config_add_from_strs(&flp_config, table, size);

	/* dump configuration if specified	*/
	if (strcmp(global_config.dump_config, NULLFILE)) {
		size = global_config_to_strs(&global_config, table, MAX_ENTRIES);
		size += thermal_config_to_strs(&thermal_config, &table[size], MAX_ENTRIES-size);
		size += flp_config_to_strs(&flp_config, &table[size], MAX_ENTRIES-size);
		/* prefix the name of the variable with a '-'	*/
		dump_str_pairs(table, size, global_config.dump_config, "-");
	}

	/* If the grid model is used, things could be really slow!
	 * Also make sure it is not in the 3-d chip mode (specified 
	 * with the layer configuration file)
	 */
	if (!strcmp(thermal_config.model_type, GRID_MODEL_STR)) {
		if (strcmp(thermal_config.grid_layer_file, NULLFILE))
			fatal("lcf file specified with the grid model. 3-d chips not supported\n");
		warning("grid model is used. HotFloorplan could be REALLY slow\n");
	}

	/* description of the functional blocks to be floorplanned	*/
	flp_desc = read_flp_desc(global_config.flp_desc, &flp_config);
	/* 
	 * just an empty frame with blocks' names.
	 * block positions not known yet.
	 */
	flp = flp_placeholder(flp_desc);
	/* temperature model	*/
	model = alloc_RC_model(&thermal_config, flp);
	/* input power vector	*/
	power = hotspot_vector(model);
	read_power(model, power, global_config.power_in);

	/* main floorplanning routine	*/
	compacted = floorplan(flp, flp_desc, model, power);
	/* 
	 * print the finally selected floorplan in a format that can 
	 * be understood by tofig.pl (which converts it to a FIG file)
	 */
	print_flp_fig(flp);
	/* print the floorplan statistics	*/
	if (flp_config.wrap_l2 &&
		!strcasecmp(flp_desc->units[flp_desc->n_units-1].name, flp_config.l2_label))
		print_flp_stats(flp, model, flp_config.l2_label, 
						global_config.power_in, global_config.flp_desc);
	
	/* print the wire delays between blocks	*/
	print_wire_delays(flp, thermal_config.base_proc_freq);

	/* also output the floorplan to a file readable by hotspot	*/
	dump_flp(flp, global_config.flp_out, FALSE);

	free_flp_desc(flp_desc);
	delete_RC_model(model);
	free_dvector(power);

	/* while deallocating, free the compacted blocks too	*/
	free_flp(flp, compacted);
}

void HotSpotFloorplannerDemo()
{
	char* flp_desc_file="ev6.desc";
	char* power_file="avg.p";
	char* output_file="demo.flp";
	char* config_file="hotspot.config";

	char* fig_file="ev6_flp.fig";
	char* stats_file="ev6.stats";
	char* wire_delay_file="ev6.wire_delay";

	HotSpotFloorplanner hsf(flp_desc_file,power_file,output_file,config_file);
	hsf.hsf_print_usage();

	hsf.insertConfigParameter("s_spreader","0.04");
	hsf.insertConfigParameter("s_sink","0.08");
	
	int compacted=hsf.hsf_floorplan();
	//hsf.hsf_print_flp_fig();
	//hsf.hsf_print_flp_fig(fig_file);
	hsf.hsf_print_auxilary_results(fig_file,stats_file,wire_delay_file);
}

int main()
{
	/***************************************/
	/*	Test demos                                                */
	/***************************************/

	//HotSpotDemo();	
	//BlockLevelModelDemo();
	//HotSpotConsiderConductivityDemo();
	//SelfMadeTimerDemo();
	//PreExtractDemo();
	//OriginalHotFloorplanDemo();
	HotSpotFloorplannerDemo();


	std::cout<<"End of Simulation."<<std::endl;

	return 0;
}