//Convolution of two function
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

TF1 *f1, *f2, *f_12;

double Func_1(double *, double *);
double Func_2(double *, double *);

double Func_12( double *, double * );
double Conv_12( double *, double * );

void Convol_test()
{
  TCanvas *c1 = new TCanvas("c1", "", 750, 750);
  c1->Divide(2, 2);

  f1 = new TF1("f1", Func_1, -10, 10, 1);
  f2 = new TF1("f2", Func_2, -10, 10, 1);

  f1->SetNpx(1000);
  f2->SetNpx(1000);

  f1->SetParameter(0, 5);
  f2->SetParameter(0, 3);
 
  c1->cd(1);
  f1->Draw();

  c1->cd(2);
  f2->Draw();
 

  f_12 = new TF1("f_12", Func_12, -10, 10, 1);
  f_12->SetNpx(1000);
  f_12->SetParameter(0, -4);

  c1->cd(3);
  f_12->DrawClone();
 
 
  //    TF1 for convolution function
  TF1 *f_Conv = new TF1("f_Conv", Conv_12, -15, 15, 0 );
 
  f_Conv->SetNpx(1000);
 
  c1->cd(4);
  f_Conv->Draw();
 
}

double Func_1(double *x, double *par)
{
  double result;
  if( TMath::Abs( x[0] ) < 5 )
    {
      result = par[0];
    }
  else result = 0;
 
  return result;
}

double Func_2(double *x, double *par)
{
  double result;
  if( TMath::Abs(x[0]) < 3 )
    {
      result = par[0];
    }
  else result = 0;
 
  return result;
}

double Func_12( double *x, double *par )
{
  return (f1->Eval(x[0])) * (f2->Eval( par[0] - x[0] ));
}

double Conv_12( double *x, double *par )
{
  f_12->SetParameter(0, x[0]);
  //return f_12->Integral(-6.2, 6.2);
  ROOT::Math::WrappedTF1 wf1(*f_12);
  ROOT::Math::IntegratorOneDim ig(ROOT::Math::IntegrationOneDim::kADAPTIVE);
  ig.SetFunction(wf1,1);
  return ig.Integral(-6.2,6.2);
}

