/*

Pearson product moment correlation coefficient and permutation test to assess statistical significance


Input: 

1.	A tab-delimited text file (e.g., from Excel) with two columns of floating point numbers
	that represent the pair measurements without any headers or labels
2.	Maximum number of permutations used (default 10,000,000)


Output: two-tailed p-value.


Learning objective: 
1. Practice use of random number generators
2. Practice math and arithmetic expressions
3. Fisher-Yates algorithm (https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
4. Heap's algorithm (for extra credit)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NOTE: 
The simulations may take some time. To make your program faster, add -O (capital letter 'O', not zero)
to the command line when compiling the program. For example, instead of
g++ Program.cpp -o Program
type 
g++ Program.cpp -O -o Program
This asks the compiler to perform certain optimizations to make the program faster.
Even if you write efficient code, compiler can do things that are rather complex that you
would not want to do yourself (see https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html 
if you want to get an idea what it does -- I will NOT test you from it).
My code usually runs 2-3 times faster with compiler optimization.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



*/



#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <random>

using namespace std;




/*************************************************************************************/
/* Pearson calculates Pearson product moment correlation coefficient between X and Y */
/* while using the provided means and standard deviations                            */
/*************************************************************************************/

double Pearson(double *X, double *Y, int rows, double MeanX, double MeanY, double SigmaX, double SigmaY)
{
// calculate covariance:
	double Cov=0.0;
	for (int i=0;i<rows;++i)  {
		Cov+=(X[i]-MeanX)*(Y[i]-MeanY);
	}
// return correlation coefficient
	return Cov/SigmaX/SigmaY;
}
	

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

int main(int argc, char **argv) {

	if (argc<2)  {
		cout << "Use as:  " << argv[0] << " <InputFile> [<Max permutations>]\n";
		cout << "Example: " << argv[0] << " Table.txt 1000000\n";
		return 0;
	}
	
// Reading the data is the same as in 083-ReadTable2.cpp that you can find on eLC

	ifstream InFile(argv[1]);
	if (!InFile.is_open())  {
		cout << "Cannot open file \"" << argv[1] << "\"\n";
		return 1;
	}
	
	int rows=0; 
	
	char Z;
	while (InFile.get(Z))  {
		if (Z=='\n')  ++rows;
	}
	
	cout << "Number of data points: " << rows << "\n";
	
	double X[rows],Y[rows];
	
	InFile.clear();
	InFile.seekg(0,InFile.beg);
	string Str;
	for (int i=0;i<rows;++i)  {
		getline(InFile,Str,'\t');
		X[i]=stof(Str);
		getline(InFile,Str);
		Y[i]=stof(Str);
	}
	
	if (getline(InFile,Str))  {   // if reading another line does not fail	
		cerr << "WARNING: there is text after the last end-of-line: \"" << Str << "\"\n";
	}
	
// The numbers are now in arrays X and Y, array size in rows.

// Setting up the max number of permutations
	long MaxPerm;
	if (argc<3)  {
		MaxPerm=10000000;
	}
	else  {
		MaxPerm=atol(argv[2]);
	}

// Calculate and print the correlation coefficient for actual data. 
// But first I am going to calculate the mean X, mean Y, and their standard deviations
// because those are the same for all permutations:
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
// Now I can pass these values to the function that calculates the correlation coefficient
	double r0=Pearson(X,Y,rows,MeanX,MeanY,SigmaX,SigmaY);
	cout << "r = " << r0 << "\n";
	
	
// Determine whether rows! (! means factorial, it is the number of all possible permutations) is larger than MaxPerm
// You only need to do this if you want the extra credit -- otherwise skip the next four lines
	long long Fact=1;
	for (int i=2;i<=rows && Fact<=MaxPerm; ++i)  {
		Fact*=i;
	};
	
	if (r0<0.0) r0=-r0;  // Absolute value of r (needed for two-tailed probability)
	long NExtreme=0;  // Number of permutations yielding values of r equaly or more extreme than the observed value
	mt19937 RNG(time(0));  // setting up a random number generator (Mersenne twister)
//	mt19937 RNG(21);  // For debugging it may be beneficial to use a constant seed so that the RNG produces the
	                  // same number sequence of "random" numbers every time but do not use it in the final version
	
	
// Now the main loop -- separate versions depending on whether number of all possible permutations is 
// greater than MaxPerm or not
	
	if (Fact>MaxPerm)  {  
// this version is using random sampling. The one using all possible permutation follows the 'else' below.
// if you do not want the extra credit, skip the 'if' above and just use the for loop that follows
		for (long c=0;c<MaxPerm;++c)  {  // test MaxPerm permutations
// there are more possible permutation than MaxPerm; I will use random sampling from the distribution
// Randomly shuffle values in array Y (Fisher-Yates algorithm):
			for (int i=rows-1;i>0;--i)  {
				int j=RNG()%(i+1);
				double Temp=Y[i];
				Y[i]=Y[j];
				Y[j]=Temp;
// Note that it is possible for j to be the same as i. This is correct and important for unbiased sampling
// (all possible permutations have equal probability to be selected)
			}
// Get a new correlation coefficient with the shuffled array:
			double r=Pearson(X,Y,rows,MeanX,MeanY,SigmaX,SigmaY);
//printf ("%4li %6.3f %6.3f: %5.2f %5.2f\n",NExtreme,r0,r,Y[0],Y[rows-1]); // I used this to check how the function is working
// and increment NExtreme if r is equally or more extreme than r0:
			if (r>=r0 || r<=-r0)  ++NExtreme;
		}  // end loop for c<MaxPerm
// Print results:
		cout << "p-value: " << (double)NExtreme/MaxPerm << "\n";
		cout << "(evaluated from a sample of " << MaxPerm << " random permutation)\n";
// Because the null distribution is symmetrical, one-tailed p-value is half of 2-tail p-value
	}
	
/*****************************+++++++++++++++++++++++++++++++++++++++++++++++++*********/
/* The part below is for extra credit -- if you do not want extra credit, you are done */
/******************************+++++++++++++++++++++++++++++++++++++++++++++++++********/

	else  {  // the version testing all possible permutations (Heap's algorithm)
// long PermCount=1;  // used for testing -- if the code is correct, PermCount must be equal to rows! in the end
		++NExtreme;  // The actual value of r satisfies the condition (r>=r0 || r<=-r0), so I count it in NExtreme.
// Now the Heap's algorithm (https://en.wikipedia.org/wiki/Heap%27s_algorithm)
		int k[rows];
		for (int i=0;i<rows;++i)  k[i]=0;
		int i=1;
		while (i<rows)  {
			if (k[i]<i)  {
				int j=k[i]*(i%2);
				double Temp=Y[i];
				Y[i]=Y[j];
				Y[j]=Temp;
// I just produced a new permutation, so I am immediately going to test if it yields a more extreme r:
				double r=Pearson(X,Y,rows,MeanX,MeanY,SigmaX,SigmaY);
				if (r>=r0 || r<=-r0)  ++NExtreme;
//++PermCount;  // only used for testing -- see above
				++k[i];
				i=1;
			}
			else  {
				k[i]=0;
				++i;
			}
		}
		cout << "p-value: " << (double)NExtreme/Fact << "\n";
		cout << "(evaluated from all possible " << Fact << " permutation)\n";
//cout << "check if " << PermCount << " = " << Fact << "\n";
	}
	
	
  return 0;

}


