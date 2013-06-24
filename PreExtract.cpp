#include "PreExtract.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include "SelfMadeTimer.h"


PreExtract::PreExtract(double w,double h,double ux, double uy,double p):
width(w),height(h),x_unit(ux),y_unit(uy),power_unit(p)
{
}

PreExtract::~PreExtract()
{
	//delete innerHotSpot;

	int num_x=width/x_unit;
	int num_y=height/y_unit;

	for (int i=0;i<num_x*num_y;i++)
	{
		delete [] temperature[i];
		delete [] rExtracted[i];
	}
	delete [] temperature;
	delete [] rExtracted;
}

bool PreExtract::validateSizeAndUnit()
{
	int widthInt=width*1e10;
	int heightInt=height*1e10;
	int uxInt=x_unit*1e10;
	int uyInt=y_unit*1e10;
	return widthInt%uxInt==0 && heightInt%uyInt==0;
}

void PreExtract::generateHotSpotFlpFile(int num_x,int num_y)
{
	char* flpFileName=new char[64];
	sprintf(flpFileName,"units_%d_mul_%d.flp",num_x,num_y);
	std::ofstream flpStream(flpFileName);
	double x_left=0.0;
	double y_bottom=0.0;

	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			x_left=x_unit*i;
			y_bottom=y_unit*j;
			flpStream<<"unit_"<<i<<"_"<<j<<"\t"<<x_unit<<"\t"<<y_unit<<'\t'<<x_left<<"\t"<<y_bottom<<std::endl;
		}
	}
	flpStream.close();
}

void PreExtract::generateHotSpotPowerFiles(int num_x,int num_y)
{
	char* powerFileName=new char[64];
	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			sprintf(powerFileName,"units_%d_%d.ptrace",i,j);
			std::ofstream powerStream(powerFileName);
			for (int k=0;k<num_x*num_y;k++)
			{
				if (k==i*num_y+j)
				{
					powerStream<<"unit_"<<k/num_y<<"_"<<k%num_y<<"\t"<<power_unit<<std::endl;
				}
				else{
					powerStream<<"unit_"<<k/num_y<<"_"<<k%num_y<<"\t"<<0.0<<std::endl;
				}
			}
			powerStream.close();
		}
	}
}

#define HotSpotNodeNumber 12

void PreExtract::generateHotSpotInitialFile(int num_x,int num_y)
{
	char* initTemperatureFileName=new char[64];
	sprintf(initTemperatureFileName,"units_%d_mul_%d.init",num_x,num_y);
	std::ofstream initTemperatureStrem(initTemperatureFileName);
	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			initTemperatureStrem<<"unit_"<<i<<"_"<<j<<"\t"<<318.15<<std::endl;
		}
	}
	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			initTemperatureStrem<<"iface_unit_"<<i<<"_"<<j<<"\t"<<318.15<<std::endl;
		}
	}
	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			initTemperatureStrem<<"hsp_unit_"<<i<<"_"<<j<<"\t"<<318.15<<std::endl;
		}
	}
	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			initTemperatureStrem<<"hsink_unit_"<<i<<"_"<<j<<"\t"<<318.15<<std::endl;
		}
	}
	for (int i=0;i<HotSpotNodeNumber;i++)
	{
		initTemperatureStrem<<"inode_"<<i<<"\t"<<318.15<<std::endl;
	}
	initTemperatureStrem.close();
}

void PreExtract::generateHotSpotInputs()
{
	int num_x=width/x_unit;
	int num_y=height/y_unit;

	//	write flp file
	generateHotSpotFlpFile(num_x,num_y);

	//	write power file
	generateHotSpotPowerFiles(num_x,num_y);
	
	//	write init temperature file
	generateHotSpotInitialFile(num_x,num_y);

}

void PreExtract::runHotSpot()
{
	int num_x=width/x_unit;
	int num_y=height/y_unit;



	char* flpFileName=new char[64];
	sprintf(flpFileName,"units_%d_mul_%d.flp",num_x,num_y);

	char* initTemperatureFileName=new char[64];
	sprintf(initTemperatureFileName,"units_%d_mul_%d.init",num_x,num_y);

	std::string name("RunHotSpotInExtract");
	char* spec=new char[64];
	sprintf(spec,"Units%dMul%d",num_x,num_y);
	name.append(spec);
	SelfMadeTimer timer(name);

	// shared hotspot configuration
	char* traceTemperatureFileName="NotAFile.txt";
	bool temp_clip=false;
	double heaksink_side=3*std::max(width,height);
	double spreader_side=5*std::max(width,height);

	bool use_default=false;
	bool r_update_used=true;
	bool leakage_used=false;
	bool leakage_mode=false;
	bool grid_model=false;
	int grid_rows=128;
	int grid_cols=128;

	// start to run hotspot
	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			char* powerFileName=new char[64];
			sprintf(powerFileName,"units_%d_%d.ptrace",i,j);
			char* stedyTemperatureFileName=new char[64];
			sprintf(stedyTemperatureFileName,"units_%d_%d.t_steady",i,j);

			innerHotSpot=new HotSpot(flpFileName,powerFileName,initTemperatureFileName,
				stedyTemperatureFileName,traceTemperatureFileName,0.001);
			innerHotSpot->hs_setup(temp_clip,use_default,heaksink_side,spreader_side,
				r_update_used, 
				leakage_used,
				leakage_mode,
				grid_model,grid_rows,grid_cols);
			timer.startTimer();
			innerHotSpot->hs_steady_state_temp();
			timer.pauseTimer();
			innerHotSpot->hs_dump_temp();
			delete innerHotSpot;
		}
	}
	timer.printOverallEclipsedTime();
}

void PreExtract::extractUsingHotSpot()
{
	// run HotSpot as many times as the num_x*num_y
	runHotSpot();

	// use BlockLevelModel extract unit r matrix
	readUnitTemperatures();

	extract();

}

void PreExtract::readUnitTemperatures()
{
	int num_x=width/x_unit;
	int num_y=height/y_unit;

	temperature=new double*[num_x*num_y];
	for (int i=0;i<num_x*num_y;i++)
	{
		temperature[i]=new double[num_x*num_y];
	}

	for (int i=0;i<num_x;i++)
	{
		for (int j=0;j<num_y;j++)
		{
			char* temperatureFileName=new char[64];
			sprintf(temperatureFileName,"units_%d_%d.t_steady",i,j);
			std::ifstream temperatureStream(temperatureFileName);
			if (!temperatureStream)
			{
				std::cout<<"IO error: cannot open units_"<<i<<"_"<<j<<".t_steady"<<std::endl;
				exit(-1);
			}
			std::string unit_name;
			for (int k=0;k<num_x*num_y;k++)
			{
				temperatureStream>>unit_name>>temperature[i*num_y+j][k];
			}
			temperatureStream.close();
		}
	}
}

#define AbsoluteTemperatureBase 273.15
#define AmbientTemperature 45.00

void PreExtract::extract()
{
	int num_x=width/x_unit;
	int num_y=height/y_unit;
	double base_temperature=AbsoluteTemperatureBase+AmbientTemperature;

	double **delta=new double*[num_x*num_y];
	rExtracted=new double*[num_x*num_y];
	for(int i=0;i<num_x*num_y;i++)
	{
		delta[i]=new double[num_x*num_y];
		rExtracted[i]=new double[num_x*num_y];
	}

	std::string name("AccualExtract");
	char* spec=new char[64];
	sprintf(spec,"Units%dMul%d",num_x,num_y);
	name.append(spec);
	SelfMadeTimer timer(name);
	
	std::cout<<"Extracted R Parameter Matrix"<<std::endl;
	
	for (int i=0;i<num_x*num_y;i++)
	{
		for(int j=0;j<num_x*num_y;j++)
		{
			timer.startTimer();
			delta[i][j]=temperature[i][j]-base_temperature;
			rExtracted[i][j]=delta[i][j]/power_unit;
			timer.pauseTimer();
			std::cout<<rExtracted[i][j]<<'\t';
		}
		std::cout<<std::endl;
	}
	
	timer.printOverallEclipsedTime();

	for (int i=0;i<num_x*num_y;i++)
	{
		delete [] delta[i];
	}
	delete [] delta;

}

double distanceBetweenPoints(double x1,double y1,double x2,double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

int PreExtract::mapAxisX(double x)
{
	int p=x/x_unit;
	return p;
}

int PreExtract::mapAxisY(double y)
{
	int q=y/y_unit;
	return q;
}

double PreExtract::distanceBetweenUnits(int p1,int q1,int p2,int q2)
{
	double pp1=p1*x_unit;
	double qq1=q1*y_unit;
	double pp2=p2*x_unit;
	double qq2=q2*y_unit;
	return distanceBetweenPoints(pp1,qq1,pp2,qq2);
}

#define NO_DISTANCE 1

void PreExtract::convertWithFlpInfo(const FlpInfo& flpInfo)
{
	int num_x=width/x_unit;
	int num_y=height/y_unit;

	int num_blks=flpInfo.num_blks;
	
	// allocate for rConverted
	rConverted=new double*[num_blks];
	double** distanceFlp=new double*[num_blks];

	for (int i=0;i<num_blks;i++)
	{
		rConverted[i]=new double[num_blks];
		distanceFlp[i]=new double[num_blks];
	}

	// compute distance between blocks
	for (int i=0;i<num_blks;i++)
	{
		for (int j=0;j<num_blks;j++)
		{
			if (i==j)
			{
				distanceFlp[i][j]=NO_DISTANCE;
			}else{
				distanceFlp[i][j]=distanceBetweenPoints(flpInfo.blk_center_x[i],flpInfo.blk_center_y[i],
					flpInfo.blk_center_x[j],flpInfo.blk_center_y[j]);
			}
			if (distanceFlp[i][j]<1e-8)
			{
				std::cout<<"Distance between blocks"<<i<<'\t'<<j<<'\t'<<"is nil\n";
			}
		}
	}

	// map blocks into unit
	int* p=new int[num_blks];
	int* q=new int[num_blks];
	for (int i=0;i<num_blks;i++)
	{
		p[i]=mapAxisX(flpInfo.blk_center_x[i]);
		q[i]=mapAxisY(flpInfo.blk_center_y[i]);
		if (p[i]>=num_x || q[i]>=num_y)
		{
			std::cout<<"Map error: #block "<<i<<"is mapped to ("<<p[i]<<"\t"<<q[i]<<")"<<std::endl;
		}
	}
	// convert units' r into blocks' r
	char* rFileName=new char[64];
	sprintf(rFileName,"units_%d_mul_%d.rConverted",num_x,num_y);
	std::ofstream rConvertedStream(rFileName);
	std::cout<<"Converted R Parameter Matrix"<<std::endl;
	for (int i=0;i<num_blks;i++)
	{
		for (int j=0;j<num_blks;j++)
		{
			double temp;
			if (i==j)
			{
				temp=NO_DISTANCE;
			}else{
				temp=distanceBetweenUnits(p[i],q[i],p[j],q[j]);
			}
			if (temp<1e-8)
			{
				std::cout<<"Distance between units"<<p[i]<<'\t'<<q[i]<<'\t'<<p[j]<<'\t'<<q[j]<<"is nil\n";
			}
			rConverted[i][j]=rExtracted[p[i]*num_y+q[i]][p[j]*num_y+q[j]]*distanceFlp[i][j]/temp;
			std::cout<<rConverted[i][j]<<'\t';
			rConvertedStream<<rConverted[i][j]<<"\t";
		}
		std::cout<<std::endl;
		rConvertedStream<<std::endl;
	}

	// release distance matrix
	for (int i=0;i<num_blks;i++)
	{
		delete [] distanceFlp[i];
	}
	delete [] distanceFlp;

}

void PreExtract::configureBlockLevelModelRI(BlockLevelModel& blm)
{
	blm.setRI(rConverted);

	// release rConverted
	for (int i=0;i<blm.num_blocks;i++)
	{
		delete [] rConverted[i];
	}
	delete [] rConverted;
}