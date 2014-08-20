#ifndef _BRANCHPROXY_H
#define _BRANCHPROXY_H

#include <iostream>
#include <functional>
#include <string>
#include <unordered_map>

#include "TTree.h"
#include "TBranch.h"

template<class T>
class BranchProxy
{
	using string = std::string;
	template<class K, class M> using unordered_map = std::unordered_map<K, M>;
	enum BranchType:int{ BT_Char_t, BT_UChar_t, BT_Short_t, BT_UShort_t, BT_Int_t, BT_UInt_t, BT_Float_t, BT_Double_t, BT_Long64_t, BT_ULong64_t, BT_Bool_t };
public:
	BranchProxy(){}
	BranchProxy(TTree* pTree, const string& name){ Connect(pTree, name); }
	
	int Connect(TTree* pTree, const string& name);
	
	operator T(){ return getVal(); }
	const T& operator =(const T& rval){ setVal(rval); return rval; }	
	
private:
	union { Char_t c; UChar_t uc; Short_t s; UShort_t us; Int_t i; UInt_t ui; Float_t f; Double_t d; Long64_t l; ULong64_t ul; Bool_t b; } val;
	
	std::function<T(void)> getVal;
	std::function<void(T)> setVal;
	
	static BranchType GetBranchType(TBranch* pBranch)
	{
		const unordered_map<char, BranchType> branchTypeMap = {{'C',BT_Char_t},{'B',BT_Char_t},{'b',BT_UChar_t},{'S',BT_Short_t},{'s',BT_UShort_t},{'I',BT_Int_t},{'i',BT_UInt_t},{'F',BT_Float_t},{'D',BT_Double_t},{'L',BT_Long64_t},{'l',BT_ULong64_t},{'O',BT_Bool_t}};
		string str=pBranch->GetTitle(); 
		size_t p=str.find_last_of('/'); 
		return p==string::npos? BT_Float_t : branchTypeMap.at(str[p+1]); 
	}
};

template<class T>
int BranchProxy<T>::Connect(TTree* pTree, const string& name)
{	
	TBranch* pBranch = pTree->GetBranch(name.c_str());
	BranchType bt = BranchProxy<T>::GetBranchType(pBranch);
	
	#define AddCase(type, var) case BT_  ## type: getVal=[this](){ return (T) val.var; }; setVal=[this](T rval){ val.var = (type) rval; }; break;
	switch(bt)
	{
		AddCase(   Char_t,  c)
		AddCase(  UChar_t, uc)
		AddCase(  Short_t,  s)
		AddCase( UShort_t, us)
		AddCase(    Int_t,  i)
		AddCase(   UInt_t, ui)
		AddCase(  Float_t,  f)
		AddCase( Double_t,  d)
		AddCase( Long64_t,  l)
		AddCase(ULong64_t, ul)
		AddCase(   Bool_t,  b)
		
	default: 	
		std::cerr << "Error: BranchProxy failed to deduce the branch type" << std::endl;
		return 1;
	}
	#undef AddCase
	
	pTree->SetBranchStatus(name.c_str(), 1);
	pTree->SetBranchAddress(name.c_str(), (void*) &val);
	
	return 0;
}

#endif // _BRANCHPROXY_H







































