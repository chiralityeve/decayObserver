#ifndef _INTERVAL_H
#define _INTERVAL_H

#include <algorithm>

//using namespace std;

//#define STDFUNCALIAS(func) template <typename... Args> auto func(Args&&... args) -> decltype(std::func(std::forward<Args>(args)...)) { return std::func(std::forward<Args>(args)...); }

class Interval
{
	
	//auto min(Args&&... args) -> decltype(std::min(std::forward<Args>(args)...)) { return std::min(std::forward<Args>(args)...); }
	//STDFUNCALIAS(min)
	//STDFUNCALIAS(max)
public:
	Interval():minimum(0.),maximum(0.){}
	Interval(double a, double b):minimum(std::min(a,b)),maximum(std::max(a,b)){}
	~Interval(){}
	
	void Set(double a, double b) { minimum = std::min(a,b); maximum = std::max(a,b); }
	
	double Length() const { return maximum - minimum; }
	double Center() const { return 0.5*(minimum + maximum); }
	
	bool In(double x) const { return !(x < minimum || x > maximum); }
	
	double minimum;
	double maximum;
};

#endif // _INTERVAL_H