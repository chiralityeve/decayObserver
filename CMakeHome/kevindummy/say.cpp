#include <iostream>
#include "Sayer.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc < 2) 
		cout << "Say what?" << endl;
	
	for(int i=1; i<argc; ++i)
		Sayer(argv[i]).Say();
	
	return 0;
}