#ifndef _ARGPARSER_H
#define _ARGPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <tuple>
#include <vector>

//using namespace std;

class ArgParser
{
	using string = std::string;
	using istringstream = std::istringstream;
	using ostringstream = std::ostringstream;
	template<class T> using vector = std::vector<T>;
	template<class T> using char_traits = std::char_traits<T>;
	template<class T, class S> using map = std::map<T, S>;
	template<class... Ts> using tuple = std::tuple<Ts...>;
public:
	ArgParser():args(),nArgs(0),index(0),parseError(false){ }
	ArgParser(const string& line):args(),nArgs(0),index(0),parseError(false){ InitArgs(line); }
	~ArgParser(){}

	operator bool() const { return !parseError; }
	
	template<class T> inline T Get();
	template<class T1, class T2, class... Ts> inline tuple<T1, T2, Ts...> Get(); 
	template<class T, int N> inline vector<T> Get(); 
	
	template<class T> inline T Get(int n) const;
	template<class T1, class T2, class... Ts> inline tuple<T1, T2, Ts...> Get(int n) const; 
	template<class T, int N> inline vector<T> Get(int n) const; 
	
	template<class T> inline int Scan(vector<T>& vec, int n);
	//template<class... Ts> inline int Scan(tuple<Ts...>& t);
	template<class... Ts> inline int Scan(Ts&... vs);
	
	string operator[](int n) const { return n<nArgs?args[n]:string(); }
	
	void ClearError(){ parseError = false; }
	void Reset(){ index = 0; ClearError(); }
	void SetStr(const string& line){ Reset(); InitArgs(line); }
	int NumArgs() const { return nArgs; }
	int NumArgsLeft() const { return nArgs - index; }
	void Delete(int i, int n=1)
	{
		if(i >= nArgs) return;
		int m = std::min(i + n, nArgs);
		args.erase(args.begin() + i, args.begin() + m);
		if(i < index)
			index -= std::min(m, index - i);
		nArgs = args.size();
	}
	
private:
	vector<string> args;
	int nArgs;
	int index;
	bool parseError;
	int failCount; // tuple scan helper
	
	template<class T> ArgParser& operator >>(T& var);
	
	void InitArgs(const string& line)
	{
		const string ws = "\r\t ";
		const string dl = "\"'";
		istringstream iss(line + ' ');
		ostringstream os;
		char c;
		char inquote = 0;
		bool escaped = false;
		
		args.clear();
		
		while(iss.get(c))
		{
			if(c == '\\')
			{
				if(escaped) os << c;
				escaped = !escaped;
				continue;
			}
			
			if(!escaped && dl.find(c) != string::npos)
			{
				if(inquote && inquote!=c)
				{
					os << c;
					continue;
				}
				if(inquote == c) inquote = 0;
				else inquote = c;
				continue;
			}
			
			escaped = false;
			
			if(inquote || ws.find(c) == string::npos)
			{
				os << c;
				if(iss.peek() != char_traits<char>::eof())
					continue;
			}
			
			if(os.str().empty()) continue;
			args.emplace_back(os.str());
			os.str(string());
		}
		
		nArgs = args.size();
	}

};

template<class T> inline ArgParser& ArgParser::operator >>(T& var){ parseError = index<nArgs? !(istringstream(args[index]) >> var) : true; if(!parseError) ++index; return *this; }
template<> inline ArgParser& ArgParser::operator >>(string& var){ if(index < nArgs) var = args[index++]; else parseError = true; return *this; }

template<class T> 
inline T ArgParser::Get()
{ 
	std::cout << "the" << std::endl;
	T var; 
	if(!(*this >> var)) ++failCount;
	return var; 
}

template<class T1, class T2, class... Ts> 
inline ArgParser::tuple<T1, T2, Ts...> ArgParser::Get()
{
	std::cout << "wat" << std::endl;
	return {Get<T1>(), Get<T2>(), Get<Ts>()...}; 
}

template<class T, int N> 
inline ArgParser::vector<T> ArgParser::Get()
{
	std::cout << "hell" << std::endl;
	vector<T> vec;
	for(int i=0; i<N; ++i) 
		vec.emplace_back(Get<T>());
	return vec;
} 

template<class T> 
inline T ArgParser::Get(int n) const 
{ 
	std::cout << ">" << n << "<" << std::endl;
	T v; 
	if(0<=n && n<nArgs) 
		istringstream(args[n]) >> v; 
	return v; 
}

template<> 
inline ArgParser::string ArgParser::Get<ArgParser::string>(int n) const 
{ 
	return n<nArgs?args[n]:string(); 
}

template<class T1, class T2, class... Ts> 
inline ArgParser::tuple<T1, T2, Ts...> ArgParser::Get(int n) const
{
	return { Get<T1>(n++), Get<T2>(n++), Get<Ts>(n++)... };
}
 
template<class T, int N> 
inline ArgParser::vector<T> ArgParser::Get(int n) const
{
	vector<T> vec;
	for(int i=0; i<N; ++i)
		vec.emplace_back(Get<T>(n+i));
	return vec;
}

template<class T> 
inline int ArgParser::Scan(vector<T>& vec, int n)
{
	failCount = 0;
	for(int i=0; i<n; ++i)
		vec.emplace_back(Get<T>());
	
	return failCount;
}

/*template<class... Ts> 
inline int ArgParser::Scan(tuple<Ts...>& t)
{
	std::cout << "ffs" << std::endl;
	failCount = 0;
	//t = tuple<Ts...>( Get<Ts...>() );
	//t = static_cast<tuple<Ts...> >(Get<Ts...>());
	return failCount;
}*/

template<class... Ts> 
inline int ArgParser::Scan(Ts&... vs)
{
	vector<bool> s = {(*this >> vs)...};
			
	return count(s.begin(), s.end(), false);
}

#endif // _ARGPARSER_H







































