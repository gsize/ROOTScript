

Double_t fun(Double_t *x, Double_t *par) {
   
   Double_t result = x[0]*x[0]/(4.* x[1]*x[1]);
   return result;
}

TCanvas *plotG()
{
TCanvas *c = new TCanvas();
gStyle->SetOptStat(kTRUE); 
gStyle->SetPalette(1);

Double_t x_min = 0.5;
Double_t x_max = 6.;
Double_t y_min = 30.;
Double_t y_max = 300.;

TF2 *f2 = new TF2("f2",fun,x_min,x_max,y_min,y_max,0);
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
gPad->SetLogz(1);

f2->Draw("surf1");

TPaveLabel *title = new TPaveLabel(0.74, 0.86, 0.96, 0.98,
                           "P=#frac{r^{2}}{4L^{2}}");
//   title->SetFillColor(32);
   title->Draw();

return c;
}
