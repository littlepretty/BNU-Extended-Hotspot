#include "FlpInfo.h"
#include <iostream>
#include <fstream>
#include <string>


FlpInfo::FlpInfo(){
	alloc_already=false;
}

int FlpInfo::readBlocksNumber(const char* flp_file)
{
	std::ifstream read_num_lines(flp_file);
	std::string blk_name;
	double a,b,c,d;
	int temp=0;
	while (read_num_lines.good())
	{
		read_num_lines>>blk_name>>a>>b>>c>>d;
		//read_num_lines.clear();
		++temp;
	}
	return temp;
}

void FlpInfo::readWHLBInfoFromFile(const char* flp_file)
{
	num_blks=readBlocksNumber(flp_file);

	blk_width=new double[num_blks];
	blk_height=new double[num_blks];

	blk_left_x=new double[num_blks];
	blk_bottom_y=new double[num_blks];

	blk_center_x=new double[num_blks];
	blk_center_y=new double[num_blks];
	blk_areas=new double[num_blks];

	alloc_already=true;

	std::ifstream flpStream(flp_file);
	std::string blk_name;
	for (int i=0;i<num_blks;i++)
	{
		if (flpStream.good())
		{
			flpStream>>blk_name>>blk_width[i]>>blk_height[i]>>blk_left_x[i]>>blk_bottom_y[i];
		}
		std::cout<<blk_name<<"\t"<<blk_width[i]<<"\t"<<blk_height[i]<<"\t"<<blk_left_x[i]<<"\t"<<blk_bottom_y[i]<<std::endl;
	}
	flpStream.close();
}

FlpInfo::~FlpInfo(){
	if (alloc_already)
	{
		delete [] blk_width;
		delete [] blk_height;
		delete [] blk_left_x;
		delete [] blk_bottom_y;
		delete [] blk_center_x;
		delete [] blk_center_y;
		delete [] blk_areas;
	}
}

void FlpInfo::calculateCenters()
{
	for (int i=0;i<num_blks;i++)
	{
		blk_center_x[i]=blk_left_x[i]+blk_width[i]/2;
		blk_center_y[i]=blk_bottom_y[i]+blk_height[i]/2;
	}
}

void FlpInfo::calculateAreas()
{
	// for now assume block is rectangle
	for (int i=0;i<num_blks;i++)
	{
		blk_areas[i]=blk_width[i]*blk_height[i];
	}
}

double FlpInfo::getChipHeight()
{
	height=0;
	double x_position=blk_left_x[0];
	for (int i=0;i<num_blks;i++)
	{
		if (blk_left_x[i]==x_position)
		{
			height+=blk_height[i];
		}
	}
	return height;
}

double FlpInfo::getChipWidth()
{
	width=0;
	double y_position=blk_bottom_y[0];
	for (int i=0;i<num_blks;i++)
	{
		if (blk_bottom_y[i]==y_position)
		{
			width+=blk_width[i];
		}
	}
	return width;
}