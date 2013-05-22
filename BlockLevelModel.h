#pragma once

	class BlockLevelModel
	{
		private:
			int num_blocks;
			double ** normal_temperature_data;
			double base_temperature;
			double *power_data;
			double **RI;
			double *power_leakage;
			double *power_leak_dy;
			double *areas;
			double *Temp;
			double  *Templast;

		public:
			BlockLevelModel(int numBlocks);
			void ReadNormalTemperatureDataFromFile();
			void ReadPowerDataFromFile(char* powerFileName="InputPower.txt");
			void ReadAreaDataFromFile(char* areaFileName="Area.txt");
			void ExtractBlockLevelParameterMatrix(); 
			void ComputeTemperatureWithSuchPower(char* powerFileName="RandomPower.txt");
			void OutputComputeResultsToFile(char* tempFileName="SteadyTemperature.txt", char* powerFileName="SteadyPower.txt");
			void ComputeLeakagePower(double *T,double Valf);
			double MaxCloseJudge(double *newPLeakDy,double *lastPLeakDy);
			virtual ~BlockLevelModel();

	};

	struct dat{
			char*name;
			double temperature;
	};

