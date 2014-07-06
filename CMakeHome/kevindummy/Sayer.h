#ifndef _SAYER_H
#define _SAYER_H

#include <iostream>
#include <string>

using namespace std;

class Sayer
{
public:
	Sayer(const string& message):message(message){}
	~Sayer(){}
	
	void Say(){ cout << message << endl; }
	
private:
	string message;
};

#endif //_SAYER_H