//
//计算点放射源到探测器的几何因子
//

Double_t fun(Double_t *x, Double_t *par) {
   
   Double_t result = 0.5*(1. - x[1]/TMath::Sqrt(x[1]*x[1]+x[0]*x[0]));
   return result;
}

Double_t fun1(Double_t *x, Double_t *par) {
   
   Double_t result = (x[0]*x[0]/(4.*x[1] *x[1]));
   return result;
} 

TCanvas *calculation_geo_par()
{
TCanvas *c = new TCanvas();
gStyle->SetOptStat(kTRUE); 
gStyle->SetPalette(1);
  c->Divide(1,2);
  
Double_t x_min = 0.5;
Double_t x_max = 6.;
Double_t y_min = 10.;
Double_t y_max = 200.;

TF2 *f2 = new TF2("f2",fun,x_min,x_max,y_min,y_max,0);
TF2 *f21 = new TF2("f21",fun1,x_min,x_max,y_min,y_max,0);

f2->GetXaxis()->SetTitle("r/cm");
f2->GetXaxis()->SetTitleOffset(1.5);
f2->GetYaxis()->SetTitle("L/cm");
f2->GetYaxis()->SetTitleOffset(1.5);
f2->GetZaxis()->SetTitle("P");
f2->GetZaxis()->SetTitleOffset(1.5);
f2->GetZaxis()->CenterTitle();
f2->GetXaxis()->CenterTitle();
f2->GetYaxis()->CenterTitle();
f2->SetTitle(" ");

c->cd(1);
gPad->SetLogz(1);
f2->Draw("surf1");

TPaveLabel *title = new TPaveLabel(0.74, 0.86, 0.96, 0.98,
                           "P=0.5*(1-h#/#sqrt{L^{2}+r^{2}})");
//   title->SetFillColor(32);
   title->Draw();
c->cd(2);
gPad->SetLogz(1);
f21->Draw("surf1");

cout<<"L=2.5m r=0.5cm f="<<f2->Eval(0.5,250) <<endl;
cout<<"L=2.5m r=0.5cm f1="<<f21->Eval(0.5,250) <<endl;
cout<<"L=3.6m r=0.11cm f="<<f2->Eval(0.11,360) <<endl;
cout<<"L=3.6m r=0.11cm f1="<<f21->Eval(0.11,360) <<endl;
return c;
}
