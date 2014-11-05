#include <vector>
#include <iostream>
#include <stdio.h>

#include "TPaveText.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TString.h"
#include "TF1.h"

Double_t fun_unfold_gause(Double_t energy);
Double_t fun_FWHM( Double_t *energy,Double_t *par);
double eff_fun(double *x,double *par);
double eff_fun1(double *x,double *par);
Double_t correct(Double_t *x, Double_t *par);

class DataAnalysis {
	public:
		DataAnalysis();
		virtual ~DataAnalysis();

		int ReadFile(const std::vector<TString> fileList);
		void ReadMacfile(TString &macFile);
		void PlotFWHM();
		TGraphErrors* PlotEffExperiment(TString fname);
		TGraphErrors *PlotEffMC(int i);
		TGraphErrors* DataAnalysis::PlotEffMCNP();
		void PlotAllEfficiency();
		void PlotSpectra(int i);
		void PlotAllSpectra();
		void SetDir(TString d){dataDir = d;};
		void ReadEffFile(TString filename,std::vector<double> &xList,std::vector<double> &yList,std::vector<double> &yErrorsList);

	private:
		double GetRateOfPeakComputom(TH1D *hist);
		//double eff_fun(double *x,double *par);
		double GetNetArea( TH1D *h,int bini,int winbin=10);
		double GetArea( TH1D *h, double e);
		void AnalyzeSpectra(TH1D *h,std::vector<double> &peakAddr,std::vector<double> &peakArea);
		TF1* getFitFunction(TString funname);

	private:
		std::vector<TString> fileList;
		TObjArray *sourceList;
		TObjArray *HPGeList;
		TString dataDir;
		Bool_t flagEffFit;
};

DataAnalysis::DataAnalysis()
{
	sourceList = new TObjArray();
	HPGeList = new TObjArray();
	flagEffFit =0;
}
DataAnalysis::~DataAnalysis()
{
	//Cleanup
	delete sourceList;
	delete HPGeList;
}
TF1* DataAnalysis::getFitFunction(TString funname)
{
	TF1* fun_eff = 0;
	if(flagEffFit)
	{
		fun_eff= new TF1(funname.Data(),eff_fun,0.040,1.500,6);
		//fun_eff->SetParameters(-0.552,-5.687, 0.434, -0.0404, 0.0013, -0.00003);
		fun_fit->SetParameters(-0.349521, -6.056041, 0.605647, -0.068800, 0.003151, -0.000059);
	}
	else
	{
		fun_eff= new TF1(funname.Data(),eff_fun1,0.040,1.500,5);
		fun_eff->SetParameters(-5.869, -0.9255, -0.22389, -0.27728,-0.09987);
	}
	return fun_eff;
}

double eff_fun(double *x,double *par)
{
	double eff=0;
	for(int i=1;i<7;i++)
	{
		eff += par[i-1]*TMath::Power(x[0],2-i);
	}
	return (TMath::Exp(eff));
}
double eff_fun1(double *x, double *par)
{
	double eff=0.;
	for(int i=0;i<5;i++)
	{
		eff += par[i] * TMath::Power(TMath::Log(x[0]),i);
	}
	return (TMath::Exp(eff));
}

Double_t fun_unfold_gause(Double_t energy)
{
	Double_t par[3];
	Double_t ene_tmp=energy*1000.0 ;

	par[0]=0.54;par[1]=0.022;par[2]=0.00158;
	return 0.001*(ene_tmp + fun_FWHM(&ene_tmp,par) * gRandom->Gaus()/2.355);
}

Double_t fun_FWHM( Double_t *energy,Double_t *par)
{
	Double_t x0=energy[0];
	return (par[0]+par[1]*TMath::Sqrt(x0 +par[2]* x0 * x0));
}

Double_t correct(Double_t *x, Double_t *par) {
	Double_t x0 = TMath::Tan(TMath::Pi()*x[0]/180);
	Double_t result = 0.5*(1. - 1/TMath::Sqrt(1+x0*x0));
	return result;
}

void DataAnalysis::PlotFWHM()
{
	std::vector<double> en;
	std::vector<double> chn;
	std::vector<double> fwhm;

	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("dataAnalysis.C","");
	dir.ReplaceAll("/./","/");

	ifstream in;
	in.open(Form("%scalb_energy.Ent",dir.Data()));
	while (1) {
		double v_en,v_chn,v_fwhm;
		in >>  v_en >> v_chn >> v_fwhm;
		if (!in.good()) break;
		en.push_back(v_en);
		chn.push_back(v_chn);
		fwhm.push_back(v_fwhm);
	}//while
	in.close();

	TCanvas* c2 = new TCanvas("Plot FWHM", "FWHM");
	c2->Divide(1,2);
	TGraph *energy_cal=new TGraph(en.size(),&en[0],&chn[0]);
	energy_cal->SetTitle("Energy calibration");
	energy_cal->Fit("pol2");
	energy_cal->GetXaxis()->CenterTitle(true);
	energy_cal->GetYaxis()->CenterTitle(true);
	energy_cal->GetHistogram()->GetXaxis()->SetTitle("Energy/keV");
	energy_cal->GetHistogram()->GetYaxis()->SetTitle("Channal");
	TF1 *fun_energy_calb= energy_cal->GetFunction("pol2");
	c2->cd(1);
	energy_cal->Draw("AC*");

	for(int i=0;i<en.size();i++)
	{
		fwhm[i] /= fun_energy_calb->Derivative(en[i]);
	}
	TGraph *FWHM_cal=new TGraph(en.size(),&en[0],&fwhm[0]);
	FWHM_cal->SetTitle("FWHM calibration");
	FWHM_cal->GetXaxis()->CenterTitle(true);
	FWHM_cal->GetYaxis()->CenterTitle(true);
	FWHM_cal->GetHistogram()->GetXaxis()->SetTitle("Energy/keV");
	FWHM_cal->GetHistogram()->GetYaxis()->SetTitle("FWHM/keV");
	TF1 *fun_FWHM_calb= new TF1("fun_FWHM",fun_FWHM,0.,2.,3);
	fun_FWHM_calb->SetParameters(0.54027,0.0219425,0.00158056);
	FWHM_cal->Fit("fun_FWHM");
	c2->cd(2);
	FWHM_cal->Draw("A*");
}

double DataAnalysis::GetRateOfPeakComputom(TH1D *hist)
{
	Int_t binmin=0,binmax=0;

	binmin=hist->FindFixBin(1.040);
	binmax=hist->FindFixBin(1.096);
	printf("d_bin=%d - %d = %d\n",binmax,binmin,binmax-binmin);
	Double_t sum=hist->Integral(binmin,binmax);
	return sum/(binmax-binmin);
}

void DataAnalysis::ReadMacfile(TString &macFile)
{

}


double DataAnalysis::GetNetArea( TH1D *h,int bin,int winbin)
{
	double grossArea=0.;
	double netArea=0.;
	TH1* bg = h->ShowBackground(20,"compton");
	//TH1* bg = h->ShowBackground(50,"BackSoothing5");
	grossArea = h->Integral(bin-winbin,bin+winbin);
	netArea = bg->Integral(bin-winbin,bin+winbin);
	return (grossArea-netArea);
}
double DataAnalysis::GetArea( TH1D *h, double energy)
{
	int bin = h->FindBin(energy);
	return (GetNetArea(h, bin,10));
}

void DataAnalysis::AnalyzeSpectra(TH1D *h,std::vector<double> &peakAddr,std::vector<double> &peakArea)
{
	TSpectrum *sp = new TSpectrum();
	Int_t nfound = sp->Search(h,2,"nodraw");
	printf("Found %d peaks to fit\n",nfound);

	for(int i=0; i < nfound;i++)
	{
		peakAddr.push_back((double)( (sp->GetPositionX())[i]));
	}
	std::sort(peakAddr.begin(),peakAddr.end());
	for(int i=0; i < nfound;i++)
	{
		double area = 0;
		area = GetArea(h,peakAddr[i]);
		peakArea.push_back(area);
	}

	delete sp;
}

TGraphErrors* DataAnalysis::PlotEffMC(int index )
{
	TH1D *hSource =(TH1D* )sourceList->At(index);
	TH1D *hHPGe =(TH1D* )HPGeList->At(index);

	std::vector<double> speakAddr;
	std::vector<double> speakArea;
	AnalyzeSpectra(hSource,speakAddr,speakArea);

	std::vector<double> peakAddr;
	std::vector<double> peakArea;
	AnalyzeSpectra(hHPGe,peakAddr,peakArea);
	TString effName;
	effName = "eff_" +fileList[index];
	printf("%s\n",fileList[index].Data());
	TF1 *fun_eff=0;
	fun_eff = getFitFunction(effName);
	fun_eff->SetLineColor(2+index);
	//TF1 *fun_correct=  new TF1("fun_correct",correct,0,90,0);
	TGraphErrors *g_eff = new TGraphErrors(peakAddr.size());
	TF1 fun_correct("fun_correct",correct,0,90,0);
	double ang=30.;
	if(index==0||index==1)
		ang=90.;
	for(int i=0; i<speakAddr.size();i++)
	{
		int k=0;
		double shield = 0.0015;
		while(k<peakAddr.size())
		{
			if(TMath::Abs(peakAddr[k] - speakAddr[i]) < shield )
			{
				g_eff->SetPoint(i,peakAddr[k],fun_correct.Eval(ang) *peakArea[k]/speakArea[i]);
				g_eff->SetPointError(i,0,fun_correct.Eval(ang) *TMath::Sqrt(1./peakArea[k]+1./speakArea[i])*(peakArea[k]/speakArea[i]));
				double dif=( (g_eff->GetY())[i] - fun_eff->Eval(peakAddr[k]) )/fun_eff->Eval(peakAddr[k]) ;
				printf("%d\t%8.4lf\t%8.3lf\t%8.3lf\t%8.5lf\t%8.2lf\n",i,peakAddr[k],speakArea[i],peakArea[k],(g_eff->GetY())[i], 100*dif);
				break;
			}
			k++;
		}

	}
	g_eff->Fit(fun_eff,"R+");
	g_eff->SetMarkerStyle(20 + index);
	g_eff->SetLineColor(2+index);
	g_eff->SetTitle(fileList[index].Data());
	g_eff->GetXaxis()->SetTitle("Energy/MeV");
	g_eff->GetXaxis()->CenterTitle();
	g_eff->GetYaxis()->SetTitle("Efficiency");
	g_eff->GetYaxis()->CenterTitle();
	//	delete fun_correct;
	return g_eff;
}

void  DataAnalysis::PlotSpectra(int i)
{
	int entries = sourceList->GetEntries();
	if(entries < 1 || i >= entries) return;

	TH1D *hSource =(TH1D* )sourceList->At(i);
	TH1D *hHPGe =(TH1D* )HPGeList->At(i);


	TCanvas* c1 = new TCanvas(fileList[i].Data(), fileList[i].Data());
	c1->Divide(1,2);
	c1->cd(1);
	hSource->SetTitle("gamma Source");
	hSource->GetXaxis()->SetTitle("Energy/MeV");
	hSource->GetXaxis()->CenterTitle();
	hSource->GetYaxis()->SetTitle("Count");
	hSource->GetYaxis()->CenterTitle();
	hSource->Draw("HIST");
	hSource->ShowBackground(20,"compton same");
	//gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);

	c1->cd(2);
	hHPGe->SetTitle("Respond");
	hHPGe->GetXaxis()->SetTitle("Energy/MeV");
	hHPGe->GetXaxis()->CenterTitle();
	hHPGe->GetYaxis()->SetTitle("Count");
	hHPGe->GetYaxis()->CenterTitle();
	hHPGe->Draw("HIST");
	hHPGe->ShowBackground(20,"compton same");
	//gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	c1->cd();
	return ;
}

void  DataAnalysis::PlotAllSpectra()
{
	for(int i=0;i<sourceList->GetEntries();i++)
	{
		PlotSpectra(i);
	}
}

TGraphErrors* DataAnalysis::PlotEffMCNP()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("dataAnalysis.C","");
	dir.ReplaceAll("/./","/");
	ifstream inf;
	inf.open(Form("%sdata/eff_80mm_point_MCNP.txt",dir.Data()));
	if(!(inf.is_open()))
	{
		printf("open eff file failed!\n");
		return 0;
	}

	int i=0; 
	std::vector<double> xList;
	std::vector<double> yList;
	string str_tmp;
	while(getline(inf,str_tmp)) {
		i++;
		double x,yExp,yMC; 
		if(i>0){
			sscanf(str_tmp.c_str(),"%lf%lf%lf",&x, &yExp, &yMC);
			xList.push_back(x*0.001);
			yList.push_back(yMC);
		}
	}
	inf.close();

	TGraphErrors *gr = new TGraphErrors((Int_t)xList.size(),&xList[0],&yList[0]);
	TF1 *fun_fit = 0;// new TF1("eff_funMC",eff_fun,0.039,1.5,6);
	TString effName("eff_funMC");
	printf("%s\n",effName.Data());
	fun_eff = getFitFunction(effName);
	gr->Fit(fun_fit,"R+");
	gr->SetTitle("eff MCNP");
	gr->GetXaxis()->SetTitle("Energy/MeV");
	gr->GetXaxis()->CenterTitle();
	gr->GetYaxis()->SetTitle("Eff");
	gr->GetYaxis()->CenterTitle();
	return gr;
}
TGraphErrors* DataAnalysis::PlotEffExperiment(TString fname)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("dataAnalysis.C","");
	dir.ReplaceAll("/./","/");

	std::vector<double> xList;
	std::vector<double> yList;
	std::vector<double> yErrorsList;
	//	TString fname("eff_80mm.Txt");
	TString pathName;
	pathName = dir + "data/";
	pathName  += fname;
	ReadEffFile(pathName,xList,yList,yErrorsList);

	TGraphErrors *gr = new TGraphErrors((Int_t)xList.size(),&xList[0],&yList[0],0,&yErrorsList[0]);
	TF1 *fun_fit = 0;// new TF1("eff_funExp",eff_fun,0.039,1.5,6);
	TString effName("eff_funExp");
	printf("%s\n",effName.Data());
	fun_fit = getFitFunction(effName);
	fun_fit->SetLineColor(1);
	gr->Fit(fun_fit,"R+");
	gr->SetMarkerStyle(5);
	//	gr->SetLineColor(5);
	//for(int i=0;i<xList.size();i++)
	//	printf("%d\t%8.4lf\t%8.4lf\t%8.4lf\n",i,xList[i],yList[i],100*(yList[i] -fun_fit->Eval(xList[i]))/ fun_fit->Eval(xList[i]));

	return gr;
}
void DataAnalysis::PlotAllEfficiency()
{
	TMultiGraph *mg = new TMultiGraph();
	TLegend *leg = new TLegend(0.1,0.8,0.3,0.9);

	for(int i=0;i<sourceList->GetEntries();i++)
	{
		TGraphErrors *gr = 0;
		gr = PlotEffMC( i);
		mg->Add(gr);
		leg->AddEntry(gr,fileList[i].Data(),"lep");
	}
	if(1)
	{
		TString fname[6]={"eff_ba133_00_20130325.Txt", "eff_ba133_04_20130415.Txt"
			,"eff_ba133_08_20130502.Txt","eff_ba133_12_20130316.Txt"
				,"eff_ba133_16_20130422.Txt","eff_ba133_20_20130318.Txt" };		
		TString legendName[]={"0cm","4cm","8cm","12cm","16cm","20cm"};
		for(int i=0;i<6;i++)
		{
		//TString fname("eff_80mm.Txt");
		TGraphErrors *gr = 0;
		gr= PlotEffExperiment(fname[i]);
		mg->Add(gr);
		leg->AddEntry(gr,legendName[i].Data(),"lep");
		//		gr = PlotEffMCNP();
		//		mg->Add(gr);
		}
	}
	TCanvas* c4 = new TCanvas("effGr", "  ");
	mg->Draw("AP");
	mg->GetXaxis()->SetTitle("Energy/MeV");
	mg->GetXaxis()->CenterTitle();
	mg->GetYaxis()->SetTitle("Efficiency");
	mg->GetYaxis()->CenterTitle();
	leg->Draw();
	//gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
}

int  DataAnalysis::ReadFile(const std::vector<TString> fList)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("dataAnalysis.C","");
	dir.ReplaceAll("/./","/");

	for(int i=0;i<fList.size();i++) 
	{
		TString fname;
		fileList.push_back(fList[i]);
		fname = fileList[i];

		TFile *f2= TFile::Open(Form("%sdata/%s/%s.root",dir.Data(),dataDir.Data(),fname.Data()));
		if(!(f2->IsOpen())){
			cout<<"file: "<<fname<<" isn't opened!"<<endl;
			return 0;
		}
		TDirectory* dire = (TDirectory*)f2->Get("histo");
		TH1D *hSource = (TH1D*)dire->Get("source"); 
		hSource->SetDirectory(0);
		TH1D * hHPGe = (TH1D*)dire->Get("HPGe"); 
		hHPGe->SetDirectory(0);
		sourceList->Add(hSource);
		HPGeList->Add(hHPGe);
		cout<<"Read file "<<fname<<" success!"<<endl;
		f2->Close();
	}
	return 1;
}

void DataAnalysis::ReadEffFile(TString filename,std::vector<double> &xList,std::vector<double> &yList,std::vector<double> &yErrorsList)
{
	ifstream inf;
	inf.open(filename.Data());
	if(!(inf.is_open()))
	{
		printf("open file failed! %s\n",filename.Data());
		return;
	}

	int i=0; 
	string str_tmp;
	while(getline(inf,str_tmp)) {
		i++;
		double x,y,yFit,yError,yDelta; 
		if(i>5){
			sscanf(str_tmp.c_str(),"%lf%lf%lf%lf",&x, &y, &yFit, &yError);
			xList.push_back(x*0.001);
			yList.push_back(y);
			yDelta =TMath::Abs( y * yError *0.01);
			yErrorsList.push_back(yDelta);
		}
	}
	inf.close();
}

void dataAnalysis()
{
	DataAnalysis *da = new DataAnalysis();
	std::vector<TString> fileList;
	/*	TString fileName[]={
		"pointd90mmdl7cover3mm",
		"plane1mmd90mmdl7cover3mm",
		"plane2mmd90mmdl7cover3mm",
		"plane3mmd90mmdl7cover3mm",
		"plane5mmd90mmdl7cover3mm",
		"plane10mmd90mmdl7cover3mm",
		"plane20mmd90mmdl7cover3mm"
		};
		*/
	/*	TString fileName[]={
		"pointd90mmdl07cover3mm",
	//		"pointd90mmdl08cover3mm",
	//		"pointd90mmdl09cover3mm",
	//		"pointd90mmdl10cover3mm",
	//		"pointd90mmdl11cover3mm",
	"pointd90mmdl12cover3mm",
	"pointd90mmdl13cover3mm",
	"pointd90mmdl14cover3mm",
	"pointd90mmdl15cover3mm",
	//		"pointd90mmdl16cover3mm",
	//		"pointd90mmdl17cover3mm",
	//		"pointd90mmdl18cover3mm",
	//		"pointd90mmdl20cover3mm"

	};
	*/	/*TString fileName[]={
		  "pointd87mmdl15cover3mm",
		  "pointd88mmdl15cover3mm",
		  "pointd89mmdl15cover3mm",
		  "pointd90mmdl15cover3mm",
		  "pointd91mmdl15cover3mm",
		  "pointd92mmdl15cover3mm",
		  "pointd93mmdl15cover3mm",
		  };*/
	/*TString fileName[]={
	  "pointd90mmdl15cover3mm"
	  };*/
	TString fileName[]={
		"point10mm",
		"point50mm",
		"point90mm",
		"point130mm",
		"point170mm",
		"point210mm",
		"point250mm"
	};
	int num =7;
	for(int i=0; i<num;i++)
		fileList.push_back(fileName[i]);
	//	TString dir("pointdl15cover3mm");
	//	TString dir("pointd90mmdl15cover3mmmylar");
	//	TString dir("pointd90dl_new");
	TString dir("point");
	da->SetDir(dir);
	/*
	   "output_point_80mm",
	   "output_point_85mm",
	   "output_point_90mm",
	   "output_point_95mm",
	   "output_point_100mm"
	   */
	/*	  "output_plane_circle_5mm",
		  "output_plane_circle_10mm",
		  "output_plane_circle_15mm",
		  "output_plane_circle_20mm",
		  "output_plane_circle_30mm",
		  "output_plane_circle_50mm",
		  */
	da->ReadFile(fileList);
	da->PlotAllEfficiency();
	if(0)
		da->PlotAllSpectra();
	if(0)
		da->PlotEffExperiment();

	if(0)
		da->PlotFWHM();
}

