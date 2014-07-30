#include "FitterPdf.h"

#define AddPdf(name, ...) {#name, {&FitterPdf::Init ## name, &FitterPdf::Check ## name } }

const map<string, vector<function<int(FitterPdf*)> > > FitterPdf::dispatch = {
	AddPdf(Gaussian),
	AddPdf(Polynomial),
	AddPdf(Exponential)
};

#undef AddPdf


FitterPdf::FitterPdf():nArgs(0){}
FitterPdf::FitterPdf(const FitterPdf& other):type(other.type),args(other.args),nArgs(args.NumArgs()){ InitPointers(); }
FitterPdf::FitterPdf(const string& _args):nArgs(0)
{
	InitPointers();
	SetArgs(_args);
}
FitterPdf::~FitterPdf(){ Clean(); }

void FitterPdf::SetArgs(const string& _args)
{
	Clean();
	args.SetStr(_args);
	if(!args.NumArgs()) return;
	type = args.Get<string>(0);
	args.Delete(0);
	nArgs = args.NumArgs();	
}

FitterPdf& FitterPdf::operator =(const FitterPdf& other)
{
	Clean();
	type = other.type;
	args = other.args;
	nArgs = args.NumArgs();
	return *this;
}

void FitterPdf::InitPointers()
{
	InitNULL(pPdf, pYld, pFitVar);
}

void FitterPdf::Clean()
{
	Delete(pPdf);
	Delete(vec_pParam);
}

ostream& FitterPdf::Print(ostream& os) const
{
	os << type;
	for(int i=0; i<args.NumArgs(); ++i) os << ' ' << args[i];
	return os;
}

int FitterPdf::Check()
{
	if(!dispatch.count(type))
	{
		cerr << "Unknown pdf type: " << *this << endl;
		return 1;
	}
	
	args.Reset();
	int e = dispatch.at(type).at(1)(this);
	
	switch(e)
	{
		case 0: return 0;
		case 1: cerr << "Wrong number of arguments: "; break;
		case 2: cerr << "Error parsing arguments: ";   break;
		case 3: cerr << "Invalid arguments: ";         break;
		default: return e;
	}
		
	return e;	
}
int FitterPdf::Init(const string& name, RooRealVar& fitVar, int nPdfs, int nEvents)
{
	Clean();
	
	basename = name;
	pFitVar = &fitVar;
	fitUnit = fitVar.getUnit();
	fitRange.Set(fitVar.getMin(), fitVar.getMax());
	//pYld = new RooRealVar(Name("yield"), Name(), nEvents*1./nPdfs, 0., nEvents);
	pYld = NewVar("yield", "", nEvents*1./nPdfs, 0., nEvents);
	
	if(Check()) return 1;
	
	args.Reset();
	return dispatch.at(type).at(0)(this);
	
	return 1;
}

int FitterPdf::CheckGaussian()
{
	double a=0., b=0., c=0.;
	
	if(nArgs > 3 || nArgs < 2) return 1;
	if(args.Scan(a, b, c) > 3 - nArgs) return 2;
	if(nArgs==3 && c < b) return 3; 
		
	return 0;
}

int FitterPdf::InitGaussian()
{	
	double mean, sigma, sigma_max;
	
	if(args.Scan(mean, sigma, sigma_max) > 3 - nArgs){ cerr << "Error parsing arguments: " << *this << endl; return 1; }
	if(nArgs == 2) sigma_max = 3*sigma;
	
	cout << type << " " << basename << ": " << mean << ", " << sigma << ", " << sigma_max << endl;
	
	RooRealVar* pMean = NewVar("mean", fitUnit, mean, mean-sigma, mean+sigma);
	RooRealVar* pSigma = NewVar("sigma", fitUnit, sigma, 0., sigma_max);
	
	NewPdf<RooGaussian>(*pFitVar, *pMean, *pSigma);
	
	return 0;
}

int FitterPdf::CheckPolynomial()
{
	int degree = 0;
	double a=0., b=0., c=0.;

	if(!nArgs || (nArgs-1)%3 != 0) return 1;
	if(args.Scan(degree)) return 2;
	if(degree <= 0) return 3;
	if(1 + 3*degree != nArgs) return 1;
	
	while(!args.Scan(b, a, c))
		if(a>b || b>c) return 3; 
	
	if(args.NumArgsLeft()) return 2;
		
	return 0;
}

int FitterPdf::InitPolynomial()
{
	auto int2str = [](int n){ ostringstream oss; oss << n; return oss.str(); };
	int degree = args.Get<int>();
	string fitVarName = pFitVar->GetName();
	RooArgList coefficients;
	
	RooRealVar* pShift = NewVar("shift", fitUnit, fitRange.Center());
	RooFormulaVar* pShiftedFitVar = new RooFormulaVar((basename+"_shiftedFitVar").c_str(), (fitVarName + " - " + pShift->GetName()).c_str(), RooArgSet(*pFitVar, *pShift));
	
	vec_pParam.push_back(pShiftedFitVar);
	
	for(int n=1; n<=degree; ++n)
	{
		double an = args.Get<double>();
		double an_0 = args.Get<double>();
		double an_1 = args.Get<double>();
		
		RooRealVar* pAn = NewVar("a" + int2str(n), "("+fitUnit+")^{-"+int2str(n)+"}", an, an_0, an_1);
		
		coefficients.add(*pAn);
	}
	
	NewPdf<RooPolynomial>(*pShiftedFitVar, coefficients);
	
	return 0;
}


int FitterPdf::CheckExponential()
{
	double init=0, min=0., max=0.;
	
	if(nArgs != 3) return 1;
	if(args.Scan(init, min, max)) return 2;
	if(min>init || init>max) return 3;
	return 0;
}

int FitterPdf::InitExponential()
{
	double lambda = args.Get<double>();
	double lambda_0 = args.Get<double>();
	double lambda_1 = args.Get<double>();
	RooRealVar* pLambda = NewVar("lambda", "("+fitUnit+")^{-1}", lambda, lambda_0, lambda_1);
	
	NewPdf<RooExponential>(*pFitVar, *pLambda);
	
	return 0;
}
































