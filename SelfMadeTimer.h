#pragma once
#include <ctime>
#include <vector>

class SelfMadeTimer
{
public:
	SelfMadeTimer(void);
	/*
	  *	Constructor with a project name
	  */
	SelfMadeTimer(std::string pName);
	virtual ~SelfMadeTimer(void);

	/*
	  *	Start the timer for a task, setted as "default_task" as default
	  */
	void startTimer(std::string taskName="default_task");

	/*
	  *	Pause the timer, save this time point---useful when you want to test several task
	  */
	void pauseTimer();

	/*
	  *	Clear the timer's all time points
	  */
	void resetTimer();

	/*
	  *	Call me if you want to know time duration between any given stop point (and its prescendent)
	  */
	double printEclipsedTimeAtIndex(int index, std::ostream& logStream);

	/*
	  *	Call me if you want to know time duration for the task just passed by
	  */
	double printLastEclipsedTime(std::string taskName);

	/*
	  *	Call me if you want to know time duration for the each task and overall/average statistics
	  */
	double printOverallEclipsedTime();
	double printAverageElipsedTime();

private:
	clock_t startPoint;		
	std::vector<clock_t> periods;					// all time stop you saved
	std::vector<std::string> taskNames;			// task name, matching every time stop
	std::string projectName;							// tell me what is this timer for!
	char* timeStatisticFileName;				// I will redirect all print and cout into this file, named based on projectName
};

