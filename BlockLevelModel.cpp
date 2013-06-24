#include "BlockLevelModel.h"
#include<iostream>
#include<fstream>
#include<iomanip>
#include<cmath>
using namespace std;

#define AbsoluteTemperatureBase 273.15
#define AmbientTemperature 45.00


BlockLevelModel::BlockLevelModel(int numBlocks)
{
	 num_blocks=numBlocks;
	 base_temperature=AbsoluteTemperatureBase+AmbientTemperature;
	 RI=new double*[num_blocks];
	 areas=new double[num_blocks];
	normal_temperature_data=new double*[num_blocks];
	power_data=new double[num_blocks];
	Templast=new double [num_blocks];

	ReadPowerDataFromFile("NormalPower.txt");

	for (int i=0;i<num_blocks;i++)
	{
		Templast[i]=AbsoluteTemperatureBase+AmbientTemperature;
		normal_temperature_data[i]=new double[num_blocks];
		RI[i]=new double[num_blocks];
	}

}
BlockLevelModel::~BlockLevelModel()
{
	for (int i=0;i<num_blocks;i++)
	{
		delete [] normal_temperature_data[i];
		delete [] RI[i];
	}
	delete [] normal_temperature_data;
	delete [] RI;
	
	delete [] areas;
	delete [] Templast;

}

void BlockLevelModel::setRI(double** R)
{
	for (int i=0;i<num_blocks;i++)
	{
		for (int j=0;j<num_blocks;j++)
		{
			RI[i][j]=R[i][j];
		}
	}
}
	

void BlockLevelModel::ReadNormalTemperatureDataFromFile()
{
	dat data;
	data.name=new char[64];

	for (int i=0;i<num_blocks;i++)
	{  
		char szName[100] = {'\0'};

		sprintf(szName,"ZT_steady_%d.steady",i+1);
		ifstream ifle1(szName,ios::in|ios::binary);
		if (!ifle1)
		{
			cerr<<"ifle1: cannot open ZT_steady.txt "<<endl;
			exit(-1);
		}
		for (int j=0;j<num_blocks;j++)
		{
			ifle1>>data.name;
			ifle1>>data.temperature;
			normal_temperature_data[i][j]=data.temperature;
			cout<<data.name<<'\t'<<data.temperature<<endl;

		}
		ifle1.close();
	}

}
void BlockLevelModel::ReadPowerDataFromFile(char* powerFileName)
{   
	ifstream ifle(powerFileName,ios::in|ios::binary);
	if (!ifle)
	{
		cerr<<powerFileName<<" cannot be opened... may be it does not exist"<<endl;
		exit(-1);
	}
	for ( int i=0;i<num_blocks;i++)
	{
		ifle>>power_data[i];
		cout<<power_data[i]<<'\t';
	}
	
	cout<<endl;
	ifle.close();
}

void BlockLevelModel::ReadAreaDataFromFile(char* areaFileName)
{   
	ifstream ifle(areaFileName,ios::in|ios::binary);
	if (!ifle)
	{
		cerr<<areaFileName<<" cannot be opened... may be it does not exist"<<endl;
		exit(-1);
	}
	for ( int i=0;i<num_blocks;i++)
	{
		ifle>>areas[i];
		cout<<areas[i]<<'\t';
	}
	cout<<endl;
	ifle.close();

}

void BlockLevelModel::OutputComputeResultsToFile(char* tempFileName, char* powerFileName)
{   
	double  sum=0;
	double s[7]={1,3,3,18,1,3,3};
	ofstream ofile1(tempFileName,ios::out|ios::binary);
	if (!ofile1)
	{
		cerr<<"temp.txt"<<endl;
		exit(-1);
	}
	for ( int i=0;i<num_blocks;i++)
	{
		char szName[100] = {'\0'};
        sprintf(szName,"temp_%d",i+1);
		ofile1<<szName<<"="<<'\t'<<Temp[i]<<'\t'<<'\t';
		sum=sum+Temp[i]*s[i];

	}
	sum=sum/32;
	ofile1<<"average"<<"="<<'\t'<<sum<<'\t'<<'\t';
	ofile1.close();

	ofstream ofile2(powerFileName,ios::out|ios::binary);
	if (!ofile2)
	{
		cerr<<powerFileName<<" cannot be opened..."<<endl;
		exit(-1);
	}
	for ( int i=0;i<num_blocks;i++)
	{
		char szName1[100];
		char szName2[100] ;
		sprintf(szName1,"Pleakage_%d",i+1);
		sprintf(szName2,"Power_data_%d",i+1);
		Templast[i]=AbsoluteTemperatureBase+AmbientTemperature;
		ofile2<<szName1<<"="<<power_leakage[i]<<'\t'<<szName2<<"="<<power_data[i];
		ofile2<<endl;
	}
	ofile2.close();
	

}
void BlockLevelModel::ExtractBlockLevelParameterMatrix()
{
	double **delta;
	double tmpP;
	delta=new double*[num_blocks];
	for(int i=0;i<num_blocks;i++)
	{
		delta[i]=new double[num_blocks];
	}
	
	for (int i=0;i<num_blocks;i++)
	{
		for(int j=0;j<num_blocks;j++)
		{
			delta[i][j]=normal_temperature_data[i][j]-base_temperature;
			tmpP=power_data[i];
			RI[i][j]=delta[i][j]/tmpP;
			cout<<RI[i][j]<<'\t';
		}
		cout<<endl;
	}
}

void BlockLevelModel::ComputeLeakagePower(double *T,double Valf)
{
	double Is=29.56;
	double *A;
	double *B;
	double alf=466.4029;
	double beta=-1224.74083;
	double gama=6.28153;
	double kesi=6.9094;
	double Vdd=1*Valf;
	A=new double[num_blocks];
	B=new double[num_blocks];
	power_leakage=new double[num_blocks];
	for(int i=0;i<num_blocks;i++)
	{
        A[i] =1.1432*pow(10.00,-9)*areas[i]*Is;
        B[i]=1.0126*pow(10.00,-11)*areas[i]*Is;
	}
	for( int i=0;i<num_blocks;i++)
	{
        double xx=A[i]*T[i]*T[i]*exp( (alf*Vdd+beta)/T[i] );
		double yy=B[i]*exp(gama*Vdd+kesi)*Vdd;

		power_leakage[i]=xx+yy;// 
	   
	}
}

void BlockLevelModel::ComputeTemperatureWithSuchPower(char* powerFileName)
{
	ReadPowerDataFromFile(powerFileName);

    double *leftTemp;
	
	double *Pnewq;
	double **Pnew;
	
	double *lastPLeakDy;
    leftTemp=new double[num_blocks];
    Temp=new double[num_blocks];
	Pnewq=new double[num_blocks];
	power_leak_dy=new double[num_blocks];
	lastPLeakDy=new double[num_blocks];
    Pnew=new double*[num_blocks];
	for(int i=0;i<num_blocks;i++)
	{
		Pnew[i]=new double[num_blocks];
	}
	for(int i=0;i<num_blocks;i++)
	{
		lastPLeakDy[i]=0;
        power_leak_dy[i]=power_data[i];
	}
	int count = 0;
	while (MaxCloseJudge(power_leak_dy,lastPLeakDy)>0.0001)
	{  
		count++;
		cout<<MaxCloseJudge(power_leak_dy,lastPLeakDy)<<endl;
		for(int i=0;i<num_blocks;i++)                 // intialize Pnew
		{
			for(int j=0;j<num_blocks;j++)
			{
				Pnew[i][j]=0;
			}
		}
	    for( int i=0;i<num_blocks;i++)          //assign Pnew
		{
		     Pnew[i][i]=power_leak_dy[i];
         }
	    for(int i=0;i<num_blocks;i++)
		{
		    for(int j=0;j<num_blocks;j++)
			{
				if (j!=i)
				{
					Pnew[i][j]=power_leak_dy[j]*RI[i][j]/RI[i][i];
				}
			}
		}
	    for (int i=0;i<num_blocks;i++)
		{  
			Pnewq[i]=0;
			for (int j=0;j<num_blocks;j++)
			{
				Pnewq[i]=Pnewq[i]+Pnew[i][j];
			}
			leftTemp[i]=RI[i][i]*Pnewq[i];
			leftTemp[i]=leftTemp[i]+Templast[i]-273.15;   //+45
			Temp[i]=leftTemp[i]+273.15;
		}
	    ComputeLeakagePower(Temp,1.0);
		for(int i=0;i<num_blocks;i++)
		{
	        lastPLeakDy[i]=power_leak_dy[i];
		}
		cout<<endl;
		for(int i=0;i<num_blocks;i++)
		{
		
		 power_leak_dy[i]=power_data[i]+power_leakage[i];
		 cout<<"lastPleage= "<<lastPLeakDy[i]<<"\t new= "<<power_leak_dy[i]<<
			 "\tpowerdata="<<power_data[i]<<"\tPleakage"<<power_leakage[i]<<endl;
		}
		cout<<endl;
		for (int i=0;i<num_blocks;i++)
		{
			                                                                                                                               //保存每一次计算的温度；
			cout<<"Temperature= "<<Temp[i]<<'\t'<<" Pleakage= "<<power_leakage[i]<<'\t';
		}
		cout<<endl;
		cout<<count<<endl;
	
	  }
	for (int i=0;i<num_blocks;i++)
	{
      Templast[i]=Temp[i];  
	}
	delete [] power_leakage;
}
double BlockLevelModel::MaxCloseJudge(double *newPLeakDy,double *lastPLeakDy)
{   
	double tempMax=0;
    for(int i=0;i<num_blocks;i++)
	{
		if (tempMax<fabs(newPLeakDy[i]-lastPLeakDy[i]))
		{
			tempMax = fabs(newPLeakDy[i]-lastPLeakDy[i]);
		}	
	}
	return tempMax;

	}