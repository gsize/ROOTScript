/*
 * =====================================================================================
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/27/2011 10:47:16 AM
 * =====================================================================================
 */

#include "RooCFunction1Binding.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooDecay.h"
#include "RooGaussModel.h"
#include "RooCFunction1Binding.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
using namespace RooFit;
double exp_fun(double, double, double);

// define new class
class MyExpF : public RooAbsPdf {
public:
  MyExpF(const char *name, const char *title,
	      RooAbsReal& _x,
	      RooAbsReal& _tau,
	      RooAbsReal& _t0);
  MyExpF(const MyExpF& other, const char* name=0) ; //why????
  virtual TObject* clone(const char* newname) const { return new MyExpF(*this,newname); } //why????
  inline virtual ~MyExpF() { }

protected:

  RooRealProxy x ;
  RooRealProxy tau ;
  RooRealProxy t0 ;
  
  Double_t evaluate() const ;

private:

 ClassDef(MyExpF,1) // Your description goes here...

};

 MyExpF::MyExpF(const char *name, const char *title, 
                        RooAbsReal& _x,
                        RooAbsReal& _tau,
                        RooAbsReal& _t0) :
   RooAbsPdf(name,title), 
   x("x","x",this,_x),
   tau("tau","",this,_tau),
   t0("t0","",this,_t0)
 { 
 } 

  MyExpF::MyExpF(const MyExpF& other, const char* name) :  
   RooAbsPdf(other,name), 
   x("x",this,other.x),
   tau("tau",this,other.tau),
   t0("t0",this,other.t0)
 { 
 }  //зачем их тут две???

 Double_t MyExpF::evaluate() const 
 { 
   // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 
   return exp_fun(x,tau,t0) ; 
 } 

// end define new class


void interp_prot()
{
	//introduce a decay
	RooRealVar t("t","time",-2,10);
	RooRealVar tau("tau","decay const.",1.2);
	RooRealVar t0("t0","",0.);
	RooRealVar c_t("c_t","",0.25);
	RooGaussModel g_t("g_t","",t,t0,c_t) ;
	
	RooDecay dec1("dec1","",t,tau,g_t,RooDecay::SingleSided);

	RooDataHist * data = dec1.generateBinned(t,50000) ;

	RooPlot* fr1 = t.frame() ;
	data->plotOn(fr1) ;
	dec1.plotOn(fr1);


//make binding
t.setBins(1000,"cache");
	MyExpF mydecay("mydecay","",t,tau,t0);
//	RooCFunction1Ref<double,double> exp_f(exp_fun);
	RooFFTConvPdf exg("exg","exp (X) gauss",t,mydecay,g_t) ;
	mydecay.plotOn(fr1, LineColor(3));
	exg.plotOn(fr1, LineColor(6));
	fr1->Draw() ;

	return;
}



double exp_fun(double x, double tau, double t0)
{
	if(x>t0){return exp(-(x-t0)/tau);}
	else{return 0.;}
}

