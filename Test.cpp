#include "HotSpot.h"
#include <time.h>
#include <iostream>
#include "BlockLevelModel.h"

using namespace std;

int main()
{
	/************************************************************************/
	/*	This is an example of how to use my encapsulated HotSpot code         */
	/************************************************************************/
	/*
	double delta_time=0.001;
	//HotSpot hs;
	//HotSpot hs("ev6.flp", "ev6_steady.ptrace", "ev6.init", "ev6_block.steady", "ev6_block.ttrace", 0.001);
	HotSpot hs("ev6.flp", "ev6_steady.ptrace", "ev6.init", "ev6.steady", "ev6_grid.ttrace", delta_time);

	//hs.hs_setup(false,false);
	//hs.hs_setup(false,false,0.06,0.03,true,false);
	// more specifically setup
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

	hs.hs_setup(temp_clip,use_default,heaksink_side,spreader_side,r_update_used, leakage_used,leakage_mode,grid_model,grid_rows,grid_cols);

	clock_t beginTime=clock();
	hs.hs_steady_state_temp();
	clock_t eclipsedTime=clock()-beginTime;
	hs.hs_dump_temp();

	cout<<"Time eclipsed is "<<(double)eclipsedTime/CLOCKS_PER_SEC<<" seconds"<<endl;

	
	/************************************************************************/
	/*	Extract two core's parameters for ZT                                                               */
	/************************************************************************/
	/*
	double delta_time=0.001;

	char flpFile[64]="ZT.flp";
	char initFile[64]="ZT.init";

	for (int i=0;i<7;i++)
	{
		char powerFile[64]="ZT_steady";
		char steadyTemperatureFile[64]="ZT_steady_";

		sprintf(powerFile,"ZT_steady%d.ptrace",i+1);
		sprintf(steadyTemperatureFile,"ZT_steady_%d.steady",i+1);

		HotSpot hs(flpFile, powerFile, initFile, steadyTemperatureFile, "NotUsed.ttrace", delta_time);

		//hs.hs_setup(false,false);
		//hs.hs_setup(false,false,0.06,0.03,true,false);
		bool temp_clip=false;
		double heaksink_side=60e-3;
		double spreader_side=30e-3;

		bool use_default=true;
		bool leakage_used=false;
		bool leakage_mode=false;

		hs.hs_setup(temp_clip,use_default,heaksink_side,spreader_side);

		clock_t beginTime=clock();
		hs.hs_steady_state_temp();
		clock_t eclipsedTime=clock()-beginTime;
		hs.hs_dump_temp();
	}
	

	/************************************************************************/
	/*  Example of Block Level Model by ZT                                                                  */
	/************************************************************************/
	/*
	int blocks=7;
	BlockLevelModel blm(blocks);

	blm.ReadAreaDataFromFile("Areas.txt");

	blm.ReadNormalTemperatureDataFromFile();
	blm.ExtractBlockLevelParameterMatrix();

	// Test with a power profile
	blm.ComputeTemperatureWithSuchPower("RandomPower.txt");
	blm.OutputComputeResultsToFile();
	*/

	/************************************************************************/
	/*	Compare that, whether R is updated will effect the thermal result       */
	/************************************************************************/

	double delta_time=0.001;
	HotSpot hs("ev6.flp", 
		"ev6_steady.ptrace", 
		"ev6.init", 
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

	return 0;
}