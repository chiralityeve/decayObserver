#include <iostream>
#include "Sayer.h"

using namespace std;

int main(int argc, char** argv)
{
	Sayer sayer("Test message");
	
	sayer.Say();
	
	return 0;
}