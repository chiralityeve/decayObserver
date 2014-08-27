#include <iostream>
#include <fstream>

#include <string>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TStyle.h>
#include <math.h>
#include <TMath.h>


using std::cout;
using std::endl;


double getEffError(double num_all, double num_subset) {

    return (1.0/num_all)*TMath::Sqrt(num_subset*(1.0-num_subset/num_all));

}


//Calculate estimates for non-resonant decay in the signal window
class Value_w_Err {
    public:
        Value_w_Err(double value, double lowerror, double uperror): _value(value), _lowerror(fabs(lowerror)), _uperror(fabs(uperror)), _symmetric(false) { };
        Value_w_Err(double value, double error): _value(value), _lowerror(fabs(error)), _uperror(fabs(error)), _symmetric(true) { };

        //Conversions and Operators
        Value_w_Err(double value): _value(value), _lowerror(0), _uperror(0), _symmetric(true) { };
        Value_w_Err(int value): _value(value), _lowerror(0), _uperror(0), _symmetric(true) { };

        //erstmal nur für symmetrische
        Value_w_Err operator=(Value_w_Err const& value_w_err) {                                         //Equal
            double new_value = value_w_err.GetVal();
            double new_err = value_w_err.GetError();
            
            Value_w_Err new_value_w_err(new_value, new_err);
            return new_value_w_err;
        }


        Value_w_Err operator+(Value_w_Err const& value_w_err) {                                         //Plus
            double new_value = _value + value_w_err.GetVal();
            double new_err = sqrt( GetError2() + value_w_err.GetError2() );

            Value_w_Err new_value_w_err(new_value, new_err);
            return new_value_w_err;
        }
            
        Value_w_Err operator-(Value_w_Err const& value_w_err) {                                         //Minus
            double new_value = _value - value_w_err.GetVal();
            double new_err = sqrt( GetError2() + value_w_err.GetError2() );

            Value_w_Err new_value_w_err(new_value, new_err);
            return new_value_w_err;
        }

        Value_w_Err operator*(Value_w_Err const& value_w_err) {                                         //Multiplikation
            double new_value = _value * value_w_err.GetVal();
            double new_rel_err = sqrt( GetRelError2() + value_w_err.GetRelError2() );
            double new_err = new_rel_err * new_value;

            Value_w_Err new_value_w_err(new_value, new_err);
            return new_value_w_err;
        }

        Value_w_Err operator/(Value_w_Err const& value_w_err) {                                         //Division
            double new_value = _value / value_w_err.GetVal();
            double new_rel_err = sqrt( GetRelError2() + value_w_err.GetRelError2() );
            double new_err = new_rel_err * new_value;

            Value_w_Err new_value_w_err(new_value, new_err);
            return new_value_w_err;
        }
            
            
        
        double _value;
        double _lowerror;
        double _uperror;
        bool _symmetric;

        double GetVal() const { return _value; }

        double GetUpError() const { return _uperror; }
        double GetLowError() const { return _lowerror; }
        double GetError() const { 
            if(_symmetric) return _uperror;
            else {std::cerr << "Asymmetric Error!"; return 0; } }
        
        double GetUpError2() const { return _uperror * _uperror; }
        double GetLowError2() const { return _lowerror * _lowerror; }
        double GetError2() const { 
            if(_symmetric) return GetUpError2();
            else {std::cerr << "Asymmetric Error!"; return 0; } }

        double GetRelUpError() const { return _uperror / _value ;}
        double GetRelLowError() const { return _lowerror / _value ;}
        double GetRelError() const { 
            if(_symmetric) return GetRelUpError();
            else {std::cerr << "Asymmetric Error!"; return 0; } }

        double GetRelUpError2() const {return GetRelUpError() * GetRelUpError() ;}
        double GetRelLowError2() const { return GetRelLowError() * GetRelLowError() ;}
        double GetRelError2() const {
            if(_symmetric) return GetRelUpError2();
            else { std::cerr << "Asymmetric Error"; return 0;} }

        void Print() const {
            cout << _value << " +/- " << GetError();
        }

};






int main() {

    //---------------------------------
    //Input ---------------------------
    //---------------------------------

    double efftot_nonres = 0.002815;
    double efftot_nonres_error = 0.000014;

    double efftot_res = 0.004258;
    double efftot_res_error = 1;

    //Test
    Value_w_Err efftot_nonres_w_error(10, -4);
    Value_w_Err efftot_res_w_error(2, 1);

    Value_w_Err addeff = efftot_nonres_w_error / efftot_res_w_error;

    addeff.Print();


    //-----------------------------------
    //Output ----------------------------
    //-----------------------------------
    /*
    cout << "------ Total Efficiency --------" << endl << endl;
    cout << "With Common-Cut:" << total_eff_common << " +/- " << total_error_common << endl;
    cout << "With Punzi-Cut:" << total_eff_punzi  << " +/- " << total_error_punzi << endl << endl;
    */
   }

