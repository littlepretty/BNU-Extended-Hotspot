#pragma once

class FlpInfo
{
public:
	FlpInfo();

	/*
	  *	Real through a file to get the line numbers, used as block numbers in floorplan file
	  *	FIXIT: needs a strict file format
	  */
	int readBlocksNumber(const char* flp_file);

	/*
	  *	Real through a file to get all the information about every block in the floorplan file
	  *	FIXIT: needs a strict file format
	  */
	void readWHLBInfoFromFile(const char* flp_file);

	/*
	  *	View every block as rectangle and find its center point
	  */
	void calculateCenters();

	/*
	  *	View every block as rectangle and calculate its area
	  */
	void calculateAreas();

	~FlpInfo();

	int num_blks;
	double* blk_center_x;
	double* blk_center_y;

	/*
	  *	View every block as rectangle and try to parse out the whole chip's width and height
	  */
	double getChipWidth();
	double getChipHeight();

private:
	bool alloc_already;

	double width;
	double height;

	/*
	  *	Arrays that store floorplan informations
	  */
	double* blk_width;
	double* blk_height;

	double* blk_left_x;
	double* blk_bottom_y;

	double* blk_areas;
};