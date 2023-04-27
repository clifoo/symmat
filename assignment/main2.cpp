#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <random>
#include <regex>
#include <cctype>
#include <iterator>
#include <algorithm>
#include <iomanip>

using namespace std;

int countLinesInFile(char *filename)
{
	std::ifstream inFile(filename);
	int count = std::count(std::istreambuf_iterator<char>(inFile),
						   std::istreambuf_iterator<char>(), '\n');
	// 	cout << "countLinesInFile() " << count << endl;
	return count;
}

int countWords(std::string str)
{
	std::regex re("\\S+"); // or `[^ \t\n]+` to exactly match the question
	return std::distance(
		std::sregex_iterator(str.begin(), str.end(), re),
		std::sregex_iterator());
}

int countColumnsInFile(char *filename)
{

	std::ifstream inFile(filename);
	if (inFile.good())
	{
		string sLine;
		getline(inFile, sLine);
		int words = countWords(sLine);
		// 		cout << "countColumnsInFile() " << words << endl;
		return words;
	}
	else
	{
		exit(-1);
	}
}

double Pearson(double X[], double Y[], int rows)
{

	double SumX = 0.0, SumY = 0.0;
	for (int i = 0; i < rows; ++i)
	{
		SumX += X[i];
		SumY += Y[i];
	}
	double MeanX = SumX / rows;
	double MeanY = SumY / rows;
	SumX = 0.0;
	SumY = 0.0;
	for (int i = 0; i < rows; ++i)
	{
		SumX += (X[i] - MeanX) * (X[i] - MeanX);
		SumY += (Y[i] - MeanY) * (Y[i] - MeanY);
	}
	double SigmaX = sqrt(SumX);
	double SigmaY = sqrt(SumY);

	// calculate covariance:
	double Cov = 0.0;
	for (int i = 0; i < rows; ++i)
	{
		Cov += (X[i] - MeanX) * (Y[i] - MeanY);
	}
	// return correlation coefficient
	return Cov / SigmaX / SigmaY;
}
void printArray(string prefix, double A[], int cells)
{

	cout << prefix << "\t";
	for (int i = 0; i < cells; i++)
	{
		cout << A[i] << "\t";
	}
	cout << "\n";
}

int main(int argc, char **argv)
{

	std::cout << std::setprecision(6);
	if (argc < 2)
	{
		cout << "Use as:  " << argv[0] << " <InputFile>\n";
		cout << "Example: " << argv[0] << " Table.txt\n";
		return 0;
	}

	ifstream InFile(argv[1]);
	if (!InFile.is_open())
	{
		cout << "Cannot open file \"" << argv[1] << "\"\n";
		return 1;
	}
	char *filename = argv[1];

	cout << filename << endl;

	// Should be 12
	int numberOfMicrobiomes = countLinesInFile(argv[1]) - 1;
	cout << "numberOfMicrobiomes = " << numberOfMicrobiomes << endl;

	// Should be 57
	int numberOfBacteria = countColumnsInFile(filename);
	cout << "numberOfBacteria = " << numberOfBacteria << endl;

	double input[numberOfMicrobiomes][numberOfBacteria];
	memset(input, 0, numberOfMicrobiomes * numberOfBacteria);
	std::string microbiomeName[numberOfMicrobiomes];
	std::string bacteriaName[numberOfBacteria];
	memset(bacteriaName, 0, numberOfBacteria);
	std::ifstream inFile(filename);

	if (!inFile.good())
	{
		exit(-1);
	}

	//// Fill Input Data
	std::string sLine;
	int rowNum = 0;
	while (std::getline(inFile, sLine))
	{
		std::regex r("\\s+");
		std::smatch match;

		std::sregex_token_iterator iter(sLine.begin(),
										sLine.end(),
										r,
										-1);
		std::sregex_token_iterator end;
		int columnNum = 0;
		for (; iter != end; ++iter)
		{
			if (rowNum == 0)
			{
				if (columnNum == 0)
				{
					// 					bacteriaName[columnNum] = "blank";
				}
				else
				{
					std::string test = iter->str();
					bacteriaName[columnNum - 1] = test;
				}
			}
			else
			{
				if (columnNum == 0)
				{
					microbiomeName[rowNum - 1] = iter->str();
					// this doesn't contain any useful data
				}
				else
				{
					std::string value = iter->str();
					double temp = ::atof(value.c_str());
					input[rowNum - 1][columnNum - 1] = temp;
				}
			}
			columnNum++;
		}
		++rowNum;
	}

	//// (temporary) print the input arrays to make sure the arrays are correct
	for (int j = 0; j < numberOfBacteria; j++)
	{
		// 		cout << bacteriaName[j] << "\t";
	}
	cout << endl;
	for (int i = 0; i < numberOfMicrobiomes; i++)
	{
		//		cout << microbiomeName[i] << "\t";

		for (int j = 0; j < numberOfBacteria; j++)
		{
			// 			printf("%.9f\t", input[i][j]);
		}
		// 		cout << endl;
	}

	////
	double output[numberOfMicrobiomes][numberOfMicrobiomes];
	memset(output, 0, numberOfMicrobiomes * numberOfMicrobiomes);

	for (int microbiomeVertical = 0; microbiomeVertical < numberOfMicrobiomes; microbiomeVertical++)
	{

		for (int microbiomeHorizontal = 0; microbiomeHorizontal < numberOfMicrobiomes; microbiomeHorizontal++)
		{
			if (microbiomeHorizontal > microbiomeVertical)
			{
				//// Regular credit
				// 				printf("[regular credit] Using cell %d,%d\n",  microbiomeVertical, microbiomeHorizontal);
				double pearsonCoeff = Pearson(input[microbiomeVertical], input[microbiomeHorizontal], numberOfBacteria);
				output[microbiomeVertical][microbiomeHorizontal] = pearsonCoeff;
			}
		}
	}

	// We must do these in separate loops otherwise the first part's calculation will be
	// affected by the shuffling in the 2nd part
	for (int microbiomeVertical = 0; microbiomeVertical < numberOfMicrobiomes; microbiomeVertical++)
	{

		for (int microbiomeHorizontal = 0; microbiomeHorizontal < numberOfMicrobiomes; microbiomeHorizontal++)
		{
			if (microbiomeHorizontal < microbiomeVertical)
			{
				printf("[extra credit] Using cell %d,%d\n", microbiomeVertical, microbiomeHorizontal);
				cout << endl;
				double pearsonCoeff = output[microbiomeHorizontal][microbiomeVertical];
				// 				cout << "pearsonCoeff at " << microbiomeHorizontal << "," <<microbiomeVertical << "= "  << pearsonCoeff << endl;
				//// Extra credit
				int MaxPermFull = 10000000;
				int MaxPerm = 10000000;
				int NExtreme = 0;
				double *X = input[microbiomeVertical];
				double *Y = input[microbiomeHorizontal];
				for (int i = 0; i < MaxPerm; i++)
				{

					//// Shuffle one of the arrays
					double *Yshuffled = Y;
					mt19937 RNG(time(0));
					for (int i = numberOfBacteria - 1; i > 0; --i)
					{
						int j = RNG() % (i + 1);
						// Swap 2 consecutive entries
						double Temp = Yshuffled[i];
						Yshuffled[i] = Yshuffled[j];
						Yshuffled[j] = Temp;
					}
					//					printArray("[shuffled] ", Yshuffled, numberOfBacteria);
					//// calculate the correlation coefficient (again)
					double pearsonCoeffShuffled = Pearson(X, Yshuffled, numberOfBacteria);
					if (abs(pearsonCoeffShuffled) > abs(pearsonCoeff))
					{
						++NExtreme;
						// 						cout << ".";
					}
				}
				double ratio = (double)NExtreme / MaxPerm;
				// 				if (abs(ratio) < 0.000000001) {
				// 					ratio = 0;
				// // 					output[microbiomeHorizontal][microbiomeVertical] = 0;
				// 				}
				// 				} else {
				// sanity check
				if (output[microbiomeVertical][microbiomeHorizontal] > 0.0000000001)
				{
					cout << "[error] ovewriting written data at " << microbiomeVertical << "," << microbiomeHorizontal << " = " << output[microbiomeVertical][microbiomeHorizontal];
					// 					exit(-1);
				}
				output[microbiomeVertical][microbiomeHorizontal] = ratio;
				// 				}
			}
			else
			{
				// nothing to do, the results have been generated already.
			}
		}
	}

	//// Print Output
	cout << "\n";
	cout << "\t";
	for (int bacteriaVertical = 0; bacteriaVertical < numberOfMicrobiomes; bacteriaVertical++)
	{
		std::string name(microbiomeName[bacteriaVertical]);
		printf("%s\t", name.c_str());
	}
	cout << "\n";
	for (int microbiomeVertical = 0; microbiomeVertical < numberOfMicrobiomes; microbiomeVertical++)
	{
		cout << microbiomeName[microbiomeVertical] << "\t";
		for (int microbiomeHorizontal = 0; microbiomeHorizontal < numberOfMicrobiomes; microbiomeHorizontal++)
		{
			double value = output[microbiomeVertical][microbiomeHorizontal];
			if (microbiomeVertical == microbiomeHorizontal)
			{
				printf("%4s\t", "*");
			}
			else if (value == 0)
			{
				printf("%4d\t", 0);
			}
			else if (microbiomeHorizontal < microbiomeVertical)
			{
				printf("%-.4f\t", value);
			}
			else
			{
				printf("%-.3f\t", value);
			}
		}
		cout << "\n";
	}
}