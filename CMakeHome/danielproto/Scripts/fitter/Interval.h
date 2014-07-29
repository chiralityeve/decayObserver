#ifndef _INTERVAL_H
#define _INTERVAL_H

using namespace std;

class Interval
{
public:
	Interval():minimum(0.),maximum(0.){}
	Interval(double a, double b):minimum(min(a,b)),maximum(max(a,b)){}
	~Interval(){}
	
	void Set(double a, double b) { minimum = min(a,b); maximum = max(a,b); }
	
	double Length() const { return maximum - minimum; }
	double Center() const { return 0.5*(minimum + maximum); }
	
	double minimum;
	double maximum;
};

#endif // _INTERVAL_H