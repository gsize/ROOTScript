#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

vector<double> energy;
vector<double> intensity;

void ReadData()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("plotGraph.C","");
	dir.ReplaceAll("/./","/");
	//ifstream fin0(Form("%sgamma.txt",dir.Data()));
	ifstream fin0(Form("%soutput_data_tmp.txt",dir.Data()));


	int i =0;
	string str_tmp;

	while(getline(fin0,str_tmp))
	{
		TString str(str_tmp);

		{
			double d1;
			double d2;
			double d3;
			sscanf(str_tmp.c_str(),"%lf%lf%lf",&d1,&d2,&d3);
			energy.push_back(d1);
			intensity.push_back(d2);
			i++;
		}
	}
	fin0.close();

}

void PlotSpectra()
{
	double maxE=0.,minE=0.;
	maxE=TMath::MaxElement(energy.size(),&energy[0]);
	minE=TMath::MinElement(energy.size(),&energy[0]);
	TCanvas* c1 = new TCanvas("C2", "C2");
	/*
	TGraph *gr = new TGraph(energy.size(),&energy[0],&intensity[0]);
	//gr->Scale(1./gr->Integral());//归一化
	gr->GetXaxis()->SetTitle("Channel");
	gPad->SetLogy(1);
	//gr->GetXaxis()->SetRangeUser(0.2,4);
	gr->GetXaxis()->SetLimits(0.03*maxE,maxE);
	gr->GetXaxis()->CenterTitle(1);
	gr->GetYaxis()->SetTitle("counts");
	gr->GetYaxis()->CenterTitle(1);
	gStyle->SetOptStat(kFALSE);
	gr->Draw();
	*/
	
	TH1D *gr =new TH1D("th1"," ",8192,0.,1.05 * maxE);
     for(int i=0;i<energy.size();i++)
     {
         int binFlag=0;
         binFlag = gr->FindBin(energy[i]);
         double BC= 0.;
         BC = gr->GetBinContent(binFlag);
         gr->SetBinContent(binFlag,BC + intensity[i]);
         //cout<<energy[i]<<"\t"<<strength[i]<<endl;
     }
     gr->Scale(1./gr->Integral());//归一化
     //gr->Draw("apl");
     //gr->GetXaxis()->SetLimits(0.5,2.);
     gStyle->SetOptStat(kFALSE);
     gr->Draw();

}

void plotGraph()
{

	ReadData();
	PlotSpectra();
}

