#include <TH3F.h>
#include <TH1F.h>
#include <TPolyMarker3D.h>
#include <TStyle.h>
#include <TCanvas.h>

//TPolyMarker3D *pm3d;

void read_data(TString FileName,TPolyMarker3D *pm3d)
{
	Double_t px, py, pz;
	
	 TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("Draw_point.C","");
   dir.ReplaceAll("/./","/");
    TString PathName;
    PathName = dir + FileName;
	
	TFile *pf= new TFile(PathName);
    TTree *tr = (TTree*)pf->Get("points");

    tr->SetBranchAddress("px",&px);
    tr->SetBranchAddress("py",&py);
    tr->SetBranchAddress("pz",&pz);
    Int_t nentries= (Int_t) tr->GetEntries();
   // printf("nentries:%d\n",nentries);
    for(Int_t i=0;i<nentries;i++){
    	tr->GetEntry(i);
    	pm3d->SetPoint(i,px,py,pz);
    }
    
    pf->Close();
}

void draw_point3D(TPolyMarker3D *pm3d)
{
	gStyle->SetOptStat(0);
	
	TCanvas *c1 = new TCanvas("demo1", "PolyMarker3D Window", 500, 0, 500, 500);
	c1->cd();

	TH3F *frame3d = new TH3F("ball","ball",26,-26,26,26,-26,26,26,-26,26);

	frame3d->Draw();
	
	pm3d->SetMarkerSize(1);
    pm3d->SetMarkerColor(4);
    pm3d->SetMarkerStyle(8);
	pm3d->Draw();
}

void draw_hist(TPolyMarker3D *pm3d)
{
	gStyle->SetOptStat(0);
	
	TCanvas *c2 = new TCanvas("hist1D", "hist1D", 500, 0, 500, 500);
	c2-> Divide(1,2);
	
	TH1F *th1= new TH1F("x","x",50,-26,26);
	TH1F *th2= new TH1F("y","y",50,-26,26);
	TH1F *th3= new TH1F("z","z",50,-26,26);
	TH2F *th4= new TH2F("xy","xy",50,-26,26,50,-26,26);
	TH3F *th5= new TH3F("xyz","xyz",50,-26,26,50,-26,26,50,-26,26);

	Int_t num_point = pm3d->Size();
	printf("%d\n",num_point);
	Float_t x, y, z;

	for(Int_t i=0;i<num_point;i++)
	{
		pm3d->GetPoint(i,x,y,z);
		th4->Fill(x,y);
		th5->Fill(x,y,z);
		th1->Fill(x);
		th2->Fill(y);
		th3->Fill(z);
	}
	TPad *tp;
	tp = (TPad *) c2->cd(1);
	tp -> Divide(3,1);
	tp->cd(1);
	th1->Draw();
	tp->cd(2);
	th2->Draw();
	tp->cd(3);
	th3->Draw();
	
	tp = (TPad *) c2->cd(2);
	tp -> Divide(2,1);
	tp->cd(1);
	th4->Draw("lego");
	tp->cd(2);
	th5->Draw("CONT3");
}

void Draw_point(TString filename="points.root")
{
	gROOT->Reset();
   
   TPolyMarker3D *pm3d1 = new TPolyMarker3D();

	read_data(filename,pm3d1);
	draw_point3D(pm3d1);
	draw_hist(pm3d1);
}

