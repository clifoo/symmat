#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <random>
#include <regex>
#include <cctype>
#include <iterator>
#include <algorithm>

using namespace std;

int countLinesInFile(char* filename) {
	std::ifstream inFile(filename); 
	int count = std::count(std::istreambuf_iterator<char>(inFile), 
	std::istreambuf_iterator<char>(), '\n');
	cout << "countLinesInFile() " << count << endl;
	return count;
}

int countWords(std::string str) 
{   
    std::regex  re("\\S+"); // or `[^ \t\n]+` to exactly match the question 
    return std::distance(
        std::sregex_iterator(str.begin(), str.end(), re),
        std::sregex_iterator()
    );

}

int countColumnsInFile(char* filename) {

	std::ifstream inFile(filename); 
	if (inFile.good()) {
		string sLine;
		getline(inFile, sLine);
		int words = countWords(sLine);
		cout << "countColumnsInFile() " << words << endl;
		return words;
	
	} else {
		exit(-1);
	}

}

void printArray(string prefix, double A[], int cells) {

	cout << prefix << "\t";
	for (int i = 0; i < cells; i++) {
		cout << A[i] << "\t";
	}
	cout << "\n";
}

double Pearson(double X[], double Y[], int rows) {

	double SumX=0.0,SumY=0.0;
	for (int i=0;i<rows;++i)  {
		SumX+=X[i];
		SumY+=Y[i];
	}
	double MeanX=SumX/rows;
	double MeanY=SumY/rows;
	SumX=0.0;
	SumY=0.0;
	for (int i=0;i<rows;++i)  {
		SumX+=(X[i]-MeanX)*(X[i]-MeanX);
		SumY+=(Y[i]-MeanY)*(Y[i]-MeanY);
	}
	double SigmaX=sqrt(SumX);
	double SigmaY=sqrt(SumY);
	
	// calculate covariance:
	double Cov=0.0;
	for (int i=0;i<rows;++i)  {
		Cov+=(X[i]-MeanX)*(Y[i]-MeanY);
	}
	// return correlation coefficient
	return Cov/SigmaX/SigmaY;
}

int main (int argc, char **argv) {

	std::cout << std::setprecision(6);
	if (argc < 2)  {
		cout << "Use as:  " << argv[0] << " <InputFile>\n";
		cout << "Example: " << argv[0] << " Table.txt\n";
		return 0;
	}
	
	ifstream InFile(argv[1]);
	if (!InFile.is_open())  {
		cout << "Cannot open file \"" << argv[1] << "\"\n";
		return 1;
	}
	char* filename = argv[1];

	int numberOfMicrobiomes = countLinesInFile(argv[1]);
	int numberofBacteria = countColumnsInFile(filename);

	double input[numberOfMicrobiomes][numberofBacteria];

	std::string microbiomeName[numberOfMicrobiomes];
	cout << "numberofBacteria = " << numberofBacteria << "\n";
	std::string bacteriaName[numberofBacteria];
	memset(bacteriaName, 0, numberofBacteria);
	std::ifstream inFile(filename); 
	if (inFile.good()) {
	
		std::string sLine;
		int rowNum = 0;
		while ( std::getline(inFile, sLine) ) {
			std::regex r("\\s+");
			std::smatch match;
			
			std::sregex_token_iterator iter(sLine.begin(),
				sLine.end(),
				r,
				-1);
			std::sregex_token_iterator end;
			int columnNum = 0;
			for ( ; iter != end; ++iter) {
				if (rowNum == 0) {
					if (columnNum == 0) {
						bacteriaName[columnNum] = "blank";
					} else {
						std::string test = iter->str();
						bacteriaName[columnNum] = test;
					}
				} else {
					if (columnNum == 0) {
						microbiomeName[rowNum] = iter->str();
						// this doesn't contain any useful data
					} else {
						std::string value = iter->str();
						double temp = ::atof(value.c_str());
						input[rowNum][columnNum] = temp;
					}
				}
				columnNum++;
			}
			++rowNum;
		}
	} else {
		exit(-1);
	}

	double output[numberOfMicrobiomes][numberOfMicrobiomes];
	memset(&output, 0, numberOfMicrobiomes * numberOfMicrobiomes);
	int deleteThis = 0;
	for (int microbiomeVertical = 1; microbiomeVertical < numberOfMicrobiomes; microbiomeVertical++) {
	
		for (int microbiomeHorizontal = 1; microbiomeHorizontal < numberOfMicrobiomes; microbiomeHorizontal++) {
		
			//--------------------------------------------------------------------------------
			// Regular credit	
			// calculate the correlation coefficient and insert it into the nxn output array

			if (microbiomeHorizontal > microbiomeVertical) {
				double pearsonCoeff = Pearson(input[microbiomeVertical], input[microbiomeHorizontal], numberOfMicrobiomes);
				output[microbiomeVertical][microbiomeHorizontal] = pearsonCoeff;
				if (pearsonCoeff < -99999999 ) {
					printArray("horizontal: ", input[microbiomeVertical], numberOfMicrobiomes);
					printArray("vertical: ", input[microbiomeHorizontal], numberOfMicrobiomes);
					cout << "wrong pearsonCoeff: " << pearsonCoeff  << "\t(" << microbiomeVertical << "," << microbiomeHorizontal << ")\n";
					exit (-1);
				}
//				cout << microbiomeHorizontal << "\n"; 
				int MaxPerm = 1000000;
				int NExtreme = 0;
				for (int i = 0; i < MaxPerm; i++) {
					// Shuffle one of the arrays
					double *X = input[microbiomeVertical];
					double *Y = input[microbiomeHorizontal];
					double *Yshuffled = Y;

					mt19937 RNG(time(0));

					for (int i=microbiomeHorizontal-1;i>0;--i)  {
						int j=RNG()%(i+1);
						double Temp=Yshuffled[i];
						Yshuffled[i]=Yshuffled[j];
						Yshuffled[j]=Temp;
					}
					
					// calculate the correlation coefficient
					double pearsonCoeffShuffled = Pearson(X, Y, numberOfMicrobiomes);
					
					if (abs(pearsonCoeffShuffled) > abs(pearsonCoeff)) {
						++NExtreme;
// 						cout << ".";
					}
				}
				if (MaxPerm == 0) {
					cout <<"Why is this zero?\n";
					exit(-1);
				}
				if (NExtreme == 0) {
// 					cout << microbiomeName[microbiomeVertical] << ": " << NExtreme << " (threshold was " << pearsonCoeff << ")\n";
				}
				double ratio = (double) NExtreme/MaxPerm;
				if (abs(ratio) < 0.0000000000001) {
					ratio = 0;
				} else {
					output[microbiomeHorizontal][microbiomeVertical] = ratio;
				}
			} else {
				// do not duplicate what we've already done
			}
		}
	}
	
	// Print the final table with headings
	cout << "\t";
	for (int bacteriaVertical = 1; bacteriaVertical < numberOfMicrobiomes; bacteriaVertical++) {
			std::string name(microbiomeName[bacteriaVertical]);
			printf("%7s", name.c_str());
	}
	cout <<"\n";
	for (int microbiomeVertical = 1; microbiomeVertical < numberOfMicrobiomes; microbiomeVertical++) {
		cout << microbiomeName[microbiomeVertical] << "\t";
		for (int microbiomeHorizontal = 1; microbiomeHorizontal < numberOfMicrobiomes-1; microbiomeHorizontal++) {
			double value = output[microbiomeVertical][microbiomeHorizontal];
			if (microbiomeVertical == microbiomeHorizontal) {
				printf("%4s\t", "*");
			} else if (value == 0) {
				printf("%4d\t", 0);
			} else {
				printf("%-.4f\t", value);
			}
		}
		cout << "\n";
	}
	
}