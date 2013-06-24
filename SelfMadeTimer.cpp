#include "SelfMadeTimer.h"
#include <iostream>
#include <fstream>


SelfMadeTimer::SelfMadeTimer(void)
{
}

SelfMadeTimer::SelfMadeTimer(std::string pName):projectName(pName)
{
	timeStatisticFileName=new char[pName.length()+64];
	sprintf(timeStatisticFileName,"%s_running_time.log",pName.c_str());
}

SelfMadeTimer::~SelfMadeTimer(void)
{
}


void SelfMadeTimer::startTimer(std::string taskName)
{
	startPoint=clock();
	taskNames.push_back(taskName);
}

void SelfMadeTimer::pauseTimer()
{
	clock_t period=clock()-startPoint;
	periods.push_back(period);
}

void SelfMadeTimer::resetTimer()
{
	periods.clear();
	taskNames.clear();
}

double SelfMadeTimer::printLastEclipsedTime(std::string taskName)
{
	clock_t lastPeriod=periods.back();
	double seconds=(double)lastPeriod/CLOCKS_PER_SEC;
	std::cout<<taskName.c_str()<<" takes "<<lastPeriod<<" clicks, "<<seconds<<" seconds"<<std::endl;
	return seconds;
}

double SelfMadeTimer::printEclipsedTimeAtIndex(int index, std::ostream& logStream)
{
	clock_t period=periods.at(index);
	std::string taskName=taskNames.at(index);
	double seconds=(double)period/CLOCKS_PER_SEC;
	std::cout<<taskName.c_str()<<" takes "<<period<<" clicks, "<<seconds<<" seconds"<<std::endl;
	if (logStream!=std::cout)
	{
		logStream<<taskName.c_str()<<" takes "<<period<<" clicks, "<<seconds<<" seconds"<<std::endl;
	}
	return seconds;
}

double SelfMadeTimer::printOverallEclipsedTime()
{
	std::ofstream logStream(timeStatisticFileName);
	if (!logStream.good())
	{
		std::cerr<<"IO error while open log file"<<std::endl;
		return -1;
	}

	std::cout<<"Summary of "<<projectName.c_str()<<"'s consumed time in order:\n";
	logStream<<"Summary of "<<projectName.c_str()<<"'s consumed time in order:\n";
	double seconds=0.0;
	if (periods.size()!=taskNames.size())
	{
		std::cout<<"You may forget to name some tasks... Here are all tasks regardless of name"<<std::endl;
		logStream<<"You may forget to name some tasks... Here are all tasks regardless of name"<<std::endl;
	}
	for (std::vector<clock_t>::iterator pos=periods.begin();pos!=periods.end();pos++)
	{
		seconds+=printEclipsedTimeAtIndex(pos-periods.begin(),logStream);
	}
	std::cout<<"Overall consumed time is "<<seconds<<" seconds"<<std::endl;
	std::cout<<"Averaged consumed time is "<<seconds/periods.size()<<" seconds"<<std::endl;
	std::cout<<"End of "<<projectName.c_str()<<"\n\n"<<std::endl;
	logStream<<"Overall consumed time is "<<seconds<<" seconds"<<std::endl;
	logStream<<"Averaged consumed time is "<<seconds/periods.size()<<" seconds"<<std::endl;
	logStream<<"End of "<<projectName.c_str()<<"\n\n"<<std::endl;
	logStream.close();
	return seconds;
}

double SelfMadeTimer::printAverageElipsedTime()
{
	std::cout<<"Average consumed time of "<<projectName.c_str()<<"\n";
	double seconds=0.0;
	if (periods.size()!=taskNames.size())
	{
		std::cout<<"You may forget to name some tasks..."<<std::endl;
	}
	for (std::vector<clock_t>::iterator pos=periods.begin();pos!=periods.end();pos++)
	{
		seconds+=printEclipsedTimeAtIndex(pos-periods.begin(),std::cout);
	}
	std::cout<<"Averaged consumed time is "<<seconds/periods.size()<<" seconds"<<std::endl;
	std::cout<<"End of "<<projectName.c_str()<<"\n\n"<<std::endl;
	return seconds/periods.size();
}