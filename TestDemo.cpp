#include "HotSpot.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include "BlockLevelModel.h"
#include "PreExtract.hpp"
#include "FlpInfo.h"
#include "SelfMadeTimer.h"

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

int main()
{
	/***************************************/
	/*	Test demos                                                */
	/***************************************/

	//HotSpotDemo();	
	//BlockLevelModelDemo();
	//HotSpotConsiderConductivityDemo();
	//SelfMadeTimerDemo();
	PreExtractDemo();

	return 0;
}