/*

Read a table with two columns from a tab-delimited text file


Input: A tab-delimited text file (e.g., from Excel) with a two columns of floating point numbers

Output: number of rows in the table and a copy of the table.


Learning objective: 
1. Parsing a tab-delimited file



*/



#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv) {

	if (argc==1)  {
		cout << "Use as:  " << argv[0] << " <Tab-delimited text file>\n";
		cout << "Example: " << argv[0] << " Table.txt\n";
		return 0;
	}
	
// Opening the file:
	ifstream InFile(argv[1]);
	if (!InFile.is_open())  {
		cout << "Cannot open file \"" << argv[1] << "\"\n";
		return 1;
	}
	
// I'll use arrays to store the data but if I want to reserve just the right amount of memory 
// for the arrays I need to read the file twice -- once to find how many lines it contains
// and after reserving memory for the arrays read it again and actually store the data.

	int rows=0;  // This will be the number of rows in the file
// One way to find the number of lines is simply to count the number of '\n' in the file:
	char Z;
	while (InFile.get(Z))  {
		if (Z=='\n')  ++rows;
	}
	
	cout << "Number of rows in the file: " << rows << "\n";
	
// Reserve memory for arrays:
	double X[rows],Y[rows];
	
// Return to the start of the file in order to read it again:
	InFile.clear();
	InFile.seekg(0,InFile.beg);
// InFile.seekg() sets the position in the stream from which next read will take place.
// InFile.seekg(0,InFile.beg) returns to the beginning of the file. See https://cplusplus.com/reference/istream/istream/seekg/
// InFile.clear() is also needed because I read to the end of file and that is still stored in the stream status.
// Without it the next read would fail because the computer would think I am still at the end of the file.
// The clear has to come before seekg !!!
// Closing the file and opening it again would have the same effect but this is slightly faster.


// Now I can read the actual numbers:
	string Str;
	for (int i=0;i<rows;++i)  {
		getline(InFile,Str,'\t');
// The '\t' specifies a delimiter character. That is, instead of reading until next '\n' (which is the default),
// this getline will read only until '\t' (tab).
		X[i]=stof(Str);  // convert Str to a double and store in X[i]
// Now read the rest of the line:
		getline(InFile,Str);
// and extract the value Y[i]:
		Y[i]=stof(Str);
	}
	
	
// A standard convention is that even the last line should end with '\n'. But just in case,
// I am going to check if there is any text after the last '\n' (which would have been ignored
// because I did not count that line):
	if (getline(InFile,Str))  {   // if reading another line does not fail	
		cerr << "WARNING: there is text after the last end-of-line: \"" << Str << "\"\n";
// cerr is standard error output. It goes to the console even if standard output (cout) is redirected
// into a file with the > in the command line.
	}
	
	
// Print the data I read:
	cout << "Data read from the file:\nX\tY\n";
	for (int i=0;i<rows;++i)  cout << X[i] << "\t" << Y[i] << "\n";
	
  return 0;

}


