#pragma once

#include "BlockLevelModel.h"
#include "FlpInfo.h"
#include "HotSpot.h"


class PreExtract
{
	friend class BlockLevelModel;

public:
	PreExtract(double w,double h,double ux, double uy,double p);
	~PreExtract();

	/*
	  *	Decide whether we can go on: if the grid size is compatible with chip size
	  */
	bool validateSizeAndUnit();

	/*
	  *	To run HotSpot, we need to generate: flp_file, power_files (a lot), init_temp_file
	 */
	void generateHotSpotInputs();

	/*
	  *	Use HotSpot's result, extract a huge r parameter matrix
	 */
	void extractUsingHotSpot();
	/*
	  *	first use HotSpot calculate outputs: steady_temp_files (a lot), then extract r
	  */	
	void operator()(){
		generateHotSpotInputs();
		extractUsingHotSpot();
	}

	/*
	  *	Convert extracted r parameters into (maybe) one of many floorplan's R matrix 
	  */	
	void convertWithFlpInfo(const FlpInfo& flpInfo);

	/*
	  *	Deliver converted R matrix to a BlockLevelModel and clear up original R matrix
	  */	
	void configureBlockLevelModelRI(BlockLevelModel& blm);

private:
	/*
	  *	Helper function for generateHotSpotInputs()
	  */	
	void generateHotSpotFlpFile(int num_x,int num_y);
	void generateHotSpotPowerFiles(int num_x,int num_y);
	void generateHotSpotInitialFile(int num_x,int num_y);

	/*
	  *	Helper function for extractUsingHotSpot()
	  */	
	void readUnitTemperatures();
	void runHotSpot();
	void extract();

	/*
	  *	Helper function for convertWithFlpInfo()
	  */
	int mapAxisX(double x);
	int mapAxisY(double y);
	double distanceBetweenUnits(int,int,int,int);


	double power_unit;				// what is the power on units

	double width;		// in mm
	double height;

	double x_unit;		// in mm
	double y_unit;

	double** temperature;			// read HotSpot result into this matrix

	/*
	  *	R that extracted from many units and converted to some block floorplan
	  */
	double** rExtracted;		
	double** rConverted;		// allocate in convertWithFlpInfo, released in configureBlockLevelModelRI

	HotSpot* innerHotSpot;			// use it to run HotSpot

};