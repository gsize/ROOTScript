# include "TCanvas.h"
# include "TROOT.h"
# include "TGraphErrors.h"
# include "TSty le .h"
# include "TMultiGraph.h"
# include "TF1.h"
# include "TLegend.h"
# include "TPaveStats .h"
# include "TArrow.h"
# include "TLatex.h"
# include "TPaveText.h"
# include "TText .h"
# include "TPavesText.h"

void MultigraphWithFit() {

/*
 *Constucts a multigraph with Y errors, fits data points, prints fitting parameters,
 *adds text and arrows on canvas and prints it on a .pdf file
 */
//Author: Stamatopoulos N. Athanasios
   
gROOT->Reset();
TCanvas *mycanvas = new TCanvas("c","c",600, 400);
mycanvas->SetFillColor(5);
mycanvas->SetFrameFillColor(10);
TMultiGraph * mg = new TMultiGraph("energy calibration","Energy Calibration");

// The values on the X,Y axes

const int n_120=4;
double x_120[n_120]={311.5, 467.5, 777.5, 933.5};
double y_120[n_120]={984.77, 1477.15, 2461.92, 2954.30};

const int n_150=3;
double x_150[n_150]={276.5, 415.5, 694.5};
double y_150[n_150]={981.09, 1471.63, 2452.71};

const int n_170=4;
double x_170[n_170]={271.5, 411.5, 690.5, 825.5};
double y_170[n_170]={979.89, 1469.84, 2449.73, 2939.68};

gROOT->SetStyle("Plain");
// See: http://root.cern.ch/root/html/TStyle.html#TStyle:SetOptFit
gStyle->SetOptFit(1111);

TGraph *gr1 = new TGraph(n_120, x_120, y_120);
gr1->SetName("gr1");
gr1->SetTitle("120^{#circ}");
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(4);
gr1->SetDrawOption("AP");
gr1->SetLineColor(4);
gr1->SetLineWidth(1);
gr1->SetFillStyle(0);

TGraph *gr2 = new TGraph(n_150, x_150, y_150);
gr2->SetName("gr2");
gr2->SetTitle("150^{#circ}");
gr2->SetMarkerStyle(20);
gr2->SetMarkerColor(2);
gr2->SetDrawOption("P");
gr2->SetLineColor(2);
gr2->SetLineWidth(1);
gr2->SetFillStyle(0);

TGraph *gr3 = new TGraph(n_170, x_170, y_170);
gr3->SetName("gr3");
gr3->SetTitle("170^{#circ}");
gr3->SetMarkerStyle(20);
gr3->SetMarkerColor(6);
gr3->SetDrawOption("P");
gr3->SetLineColor(6);
gr3->SetLineWidth(1);
gr3->SetFillStyle(0);

/*gr1->Fit("expo");
gr1->GetFunction("expo")->SetLineColor(gr1->GetLineColor());
gr1->GetFunction("expo")->SetLineWidth(2);
//gr1->Print("all");
gr2->Fit("expo");
gr2->GetFunction("expo")->SetLineColor(gr2->GetLineColor());
gr2->GetFunction("expo")->SetLineWidth(2);*/

mg->Add(gr1);
mg->Add(gr2);
mg->Add(gr3);
mg->Draw("AP");
mg->SetTitle("Gain Curves for Fe-55 X-rays and Po-210 alpha particles;Vmesh[V];Gain");
mycanvas->BuildLegend(0.15, 0.7, 0.4, 0.9,"Ar-CO2-->70%-30%");


gr1->Fit("pol1");
gr1->GetFunction("pol1")->SetLineColor(gr1->GetLineColor());
gr1->GetFunction("pol1")->SetLineWidth(2);
gr2->Fit("pol1");
gr2->GetFunction("pol1")->SetLineColor(gr2->GetLineColor());
gr2->GetFunction("pol1")->SetLineWidth(2);
gr3->Fit("pol1");
gr3->GetFunction("pol1")->SetLineColor(gr3->GetLineColor());
gr3->GetFunction("pol1")->SetLineWidth(2);

mycanvas->Modified(); mycanvas->Update(); // make sure it's really (re)drawn
TPaveStats *st = ((TPaveStats*)(gr1->GetListOfFunctions()->FindObject("stats")));
  if (st) {
      st->SetTextColor(gr1->GetLineColor());
      st->SetX1NDC(0.64); st->SetX2NDC(0.99);
      st->SetY1NDC(0.4); st->SetY2NDC(0.6);
   }
   st = ((TPaveStats*)(gr2->GetListOfFunctions()->FindObject("stats")));
   if (st) {
      st->SetTextColor(gr2->GetLineColor());
      st->SetX1NDC(0.64); st->SetX2NDC(0.99);
      st->SetY1NDC(0.15); st->SetY2NDC(0.35);
   }
st = ((TPaveStats*)(gr3->GetListOfFunctions()->FindObject("stats")));
   if (st) {
      st->SetTextColor(gr3->GetLineColor());
      st->SetX1NDC(0.64); st->SetX2NDC(0.99);
      st->SetY1NDC(0.0); st->SetY2NDC(0.25);
   }
}
