#ifndef _ARGPARSER_H
#define _ARGPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>


using namespace std;



class ArgParser
{
public:
	ArgParser():args(),nArgs(0),index(0),parseError(false){ }
	ArgParser(const string& line):args(),nArgs(0),index(0),parseError(false){ InitArgs(line); }
	~ArgParser(){}

	//template<class T> ArgParser& operator >>(T& var){ parseError = index<nArgs? !(istringstream(args[index]) >> var) : true; if(!parseError) ++index; return *this; }
	//ArgParser& operator >>(string& var){ if(index < nArgs) var = args[index++]; else parseError = true; return *this; }
	template<class T> ArgParser& operator >>(T& var);
	//ArgParser& operator >>(string& var){ if(index < nArgs) var = args[index++]; else parseError = true; return *this; }
	operator bool() const { return !parseError; }
	
	template<class T> inline T Get(int n) const;
	
	/*class ArgParseProxy
	{
		ArgParseProxy(ArgParser ap, int i):ap(ap),i{i}{}
		ArgParser& ap;
		int i;
	public:
		template<class T> inline operator T();
		friend inline ostream& operator<<(ostream& os, const ArgParseProxy& app);
		friend class ArgParser;
	};
	
	ArgParseProxy operator[](int n) const { return ArgParseProxy(*this, n); }*/
	string operator[](int n) const { return n<nArgs?args[n]:string(); }
	
	template<class T1, class... Tn> int operator ()(T1& v1, Tn&... vn)
	{
		int nVals = 0;
		int nFail = 0;
		Scan(nVals, nFail, v1, vn...);
		return nFail;
	}
	
	void ClearError(){ parseError = false; }
	void Reset(){ index = 0; ClearError(); }
	void SetStr(const string& line){ Reset(); InitArgs(line); }
	int NumArgs() const { return nArgs; }
	int NumArgsLeft() const { return nArgs - index; }
	
private:
	vector<string> args;
	int nArgs;
	int index;
	bool parseError;
	
	template<class T1> void Scan(int& n, int& m, T1& v1)
	{
		if(m || !(*this >> v1)) ++m;
		++n;
	}
	template<class T1, class... Tn> void Scan(int& n, int& m, T1& v1, Tn&... vn)
	{
		if(m || !(*this >> v1)) ++m;
		Scan(n, m, vn...);
		++n;
	}
	
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

template<class T> inline T ArgParser::Get(int n) const { T v; if(0<=n && n<nArgs) istringstream(args[n]) >> v; return v; }
template<> inline string ArgParser::Get<string>(int n) const { return n<nArgs?args[n]:string(); }
//template<class T> inline ArgParser::ArgParseProxy::operator T(){ return ap.get<T>(i); }
//inline ostream& operator<<(ostream& os, const ArgParser::ArgParseProxy& app){ return os << app.ap.get<string>(app.i); }

template<typename T>
static void ParseArguments(int& n, int& m, ArgParser& parser, T& arg)
{
	if(parser >> arg) ++n;
	++m;
}

template<typename T, typename... Types>
static void ParseArguments(int& n, int& m, ArgParser& parser, T& arg, Types&... args)
{
	if(parser >> arg) ++n;
	++m;
	ParseArguments(n, m, parser, args...);
}

/*
class ArgParser
{
public:
	ArgParser(const string& line):iss(line + " "),ss(),parseError(false){}
	~ArgParser(){}
	
	template<class T> ArgParser& operator >>(T& var){ parseError = !(GetNextArg() >> var); return *this; }
	ArgParser& operator >>(string& var){ var = GetNextArg().str(); return *this; }
	operator bool() const { return !parseError && (bool) iss; }
	
	
private:
	istringstream iss;
	stringstream ss;
	bool parseError;
	
	stringstream& GetNextArg()
	{
		const string ws = "\r\t ";
		const string dl = "\"'";
		char c;
		char inquote = 0;
		bool escaped = false;
		
		ss.str("");
		ss.clear();
		while(iss.get(c))
		{
			if(c == '\\')
			{
				if(escaped) ss << c;
				escaped = !escaped;
				continue;
			}
			
			if(!escaped && dl.find(c) != string::npos)
			{
				if(inquote && inquote!=c)
				{
					ss << c;
					continue;
				}
				if(inquote == c) inquote = 0;
				else inquote = c;
				continue;
			}
			
			escaped = false;
			
			if(inquote || ws.find(c) == string::npos)
			{
				ss << c;
				if(iss.peek() != char_traits<char>::eof())
					continue;
			}
			
			if(ss.str().empty()) continue;
			return ss;
		}
	
		return ss;
	}

	template<typename T>
	static void ParseArguments(int& n, int& m, ArgParser& parser, T& arg)
	{
		if(parser >> arg) ++n;
		++m;
	}

	template<typename T, typename... Types>
	static void ParseArguments(int& n, int& m, ArgParser& parser, T& arg, Types&... args)
	{
		if(parser >> arg) ++n;
		++m;
		ParseArguments(n, m, parser, args...);
	}
	
	template<typename T> friend int ParseArguments(const string& line, T& arg);
	template<typename T, typename... Types> friend int ParseArguments(const string& line, T& arg, Types&... args);
};*/
	
template<typename T>
int ParseArguments(const string& line, T& arg)
{
	int n = 0;
	int m = 0;
	ArgParser parser(line);
	ParseArguments(n, m, parser, arg);
	return m - n;
}

template<typename T, typename... Types>
int ParseArguments(const string& line, T& arg, Types&... args)
{
	int n = 0;
	int m = 0;
	ArgParser parser(line);
	ParseArguments(n, m, parser, arg, args...);
	return m - n;
}

template<typename T>
vector<T> ParseArguments(const string& line)
{
	ArgParser parser(line);
	vector<T> vec;
	T e;
	
	while(parser >> e) vec.push_back(e);
	return vec;
}


#endif // _ARGPARSER_H







































