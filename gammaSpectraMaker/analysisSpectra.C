#include <vector>
#include <stdio.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

vector<double> energy;
vector<double> intensity;
vector<double> energy1;
vector<double> intensity1;
TH1D *th0 = 0 ;
TH1D *th1 = 0 ;

TFitResultPtr plotEnergyCal()
{
	TFitResultPtr pt=0; 
	if(energy1.size())
	{
		TGraph * eneCal = new TGraph();
		for(int i=0;i<energy1.size();i++)
		{
			eneCal->SetPoint(i+1,i+1,1000.*energy1[i]);
		}
		pt = eneCal->Fit("pol2","s");
	}
	return pt;
}

void plotFWHM()
{
	TF1 * fwhm = new TF1("fwhm","[0]+[1]*(x+[2]*x*x)",0.,3.);
	fwhm->SetParameters(7.356e-4,8.595e-4,0.48984);

	TCanvas* c3 = new TCanvas("fwhm", "fwhm");

	fwhm->Draw();
}

bool ReadData(TString fileName)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("analysisSpectra.C","");
	dir.ReplaceAll("/./","/");
	//ifstream fin0(Form("%sgamma.txt",dir.Data()));
	ifstream fin0(Form("%s%s",dir.Data(),fileName.Data()));

	if(!(fin0.is_open()))
	{
		cout<< "open in file failed!"<<endl;
		return 0;
	}
	else
	{
		TPRegexp re1("(\\d[\\d.+\\-Ee]+,)([\\d.+\\-Ee]+),");

		int i =0,j=0;
		int flag = 0;
		bool bflag = 0;
		string str_tmp;

		while(getline(fin0,str_tmp))
		{
			TString str(str_tmp);
			if(str.Contains(re1))
			{
				double d1;
				double d2;

				if(bflag == 0)
				{
					flag++ ;
					bflag= 1;
				}
				sscanf(str_tmp.c_str(),"%lf,%lf",&d1,&d2);
				if(flag ==1)
				{
					energy.push_back(d1);
					intensity.push_back(d2);
					j++;
				}
				if(flag ==2)
				{
					energy1.push_back(d1);
					intensity1.push_back(d2);
					i++;
				}
			}else
				bflag =0;

		}
		//		printf("%d\t%d\n",j,i);
		fin0.close();

		return 1;
	}
}

void GetEdge(const double energy, double &edgeL,double &edgeR)
{
	TF1  fwhm("fwhm","[0]+[1]*(x+[2]*x*x)",0.,3.);
	fwhm.SetParameters(7.356e-4,8.595e-4,0.48984);

	double k0=3.5;
	edgeL= energy - k0* fwhm.Eval(energy);
	edgeR= energy + k0* fwhm.Eval(energy);

	return ;
}

double CalMDA(double A0, double an, double b)
{
	double mda =0.;
	mda = 1.645 * TMath::Sqrt(b)/(an/A0);
	return mda;
}


void PlotSpectra(TString opt="")
{
	double maxE=0.,minE=0.;
	double nps = 8.e+8;

	maxE=TMath::MaxElement(energy.size(),&energy[0]);
	minE=TMath::MinElement(energy.size(),&energy[0]);

	th0 =new TH1D("original","original",8192,0.,1.05 * maxE);
	th0->SetAxisRange(minE,maxE);

	for(int i=0;i<intensity.size();i++)
	{
		int binFlag =0;
		binFlag = th0->FindBin(energy[i]);
		double Bc= 0.;
		Bc = th0->GetBinContent(binFlag);
		th0->SetBinContent(binFlag,Bc + intensity[i]);
	}
	th0->GetXaxis()->SetTitle("Energy/MeV");
	th0->GetXaxis()->CenterTitle(1);
	th0->GetYaxis()->SetTitle("counts");
	th0->GetYaxis()->CenterTitle(1);
	th0->SetLineColor(kBlack);
	th0->Scale(nps/th0->Integral());//归一化
	//th0->GetXaxis()->SetRangeUser(Rlow - 0.1,Rhigh +0.1);

	th1 =new TH1D("respond","respond",intensity1.size(),0.,1.05 * maxE);
	th1->SetAxisRange(minE,maxE);
	for(int i=0;i<intensity1.size();i++)
	{
		th1->SetBinContent(i+1, intensity1[i]);
	}

	TAxis *xa;
	xa = th1->GetXaxis();
	xa->Set(energy1.size()-1, &energy1[0]);
	//th1->Scale(nps/th1->Integral());//归一化
	th1->Scale(nps);//归一化
	th1->GetXaxis()->SetTitle("Energy/MeV");
	th1->GetXaxis()->CenterTitle(1);
	th1->GetYaxis()->SetTitle("counts");
	th1->GetYaxis()->CenterTitle(1);
	th1->GetXaxis()->SetRangeUser(0.05,1.05 * maxE);

	if(opt.Contains("draw"))
	{
		TCanvas* c1 = new TCanvas("C1", "C1");
		//	c1->Divide(1,2);

		th0->Draw("pl");
		th0->SetMinimum(1.0e-2);
		th1->Draw("pl same");
		TH1* bg=0;
		TSpectrum s;
		bg = s.Background(th1,6,"nodraw  b ackincreasingwindow b ackorder6 backsmoothingr7  n osmoothing compton");
		int npeak=0;
		npeak = s.Search(th0,6,"nodraw",0.0005);

		bg->Draw("same");
		gStyle->SetOptStat(kFALSE);
		gPad->SetLogy(1);
		c1->cd();
	}
}

void Print2File(TString output="output")
{
	if(th1->GetNbinsX())
	{
		TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
		dir.ReplaceAll("analysisSpectra.C","");
		dir.ReplaceAll("/./","/");
		ofstream fout(Form("%s%s-out.Spe",dir.Data(),output.Data()));

		fout<<"$SPEC_ID:"<<endl;
		fout<<"OutSpectra"<<endl;
		fout<<"$DATE_MEA:"<<endl;
		fout<<"03/08/2016 10:17:24"<<endl;
		fout<<"$MEAS_TIM:"<<endl;
		fout<<"516 517"<<endl;
		fout<<"$DATA:"<<endl;
		fout<<"0 "<<th1->GetNbinsX()<<endl;
		for(int i=0;i<th1->GetNbinsX();i++)
		{
			fout<<"  "<<th1->GetBinContent(i+1)<<endl;
		}
		fout<<"$PRESET:"<<endl;
		fout<<"None\n0\n0"<<endl;

		TFitResultPtr pt= plotEnergyCal();

		fout<<"$ENER_FIT:"<<endl;
		fout<<pt->Value(0)<<" "<<pt->Value(1)<<endl;
		//		fout<<"$SHAPE_CAL:"<<endl;
		//	fwhm.SetParameters(7.356e-4,8.595e-4,0.48984);
		//		fout<<"0.158900 0.135897"<<endl;
		fout.close();
	}else
		printf("No Data!\n");
}

void Analysis(TH1 *h1,TString opt="onepeak no draw print")
//void FitAwmi(TH1 *h1)
{
	Int_t nfound=0;

	TH1 *h = (TH1 *)h1->Clone("h");
	h->SetAxisRange(0.5,1.2);
	TH1 *sub = (TH1 *)h->Clone("sub");
	h->SetTitle("Fitting using AWMI algorithm");
	if(opt.Contains("Stiefel"))
		h->SetTitle("Fitting using Stiefel algorithm");

	TSpectrum *s = new TSpectrum();
	//searching for candidate peaks positions
	TH1 *bg = s->Background(h,6," b ackincreasingwindow b ackorder6 backsmoothingr7  n osmoothing compton");

	sub->Add(bg,-1);
	sub->SetLineColor(kBlack);

	nfound = s->Search(sub,2,"goff",0.01);
	printf("Find %d peaks\n",nfound);

	//filling in the
	//initial estimates of the input parameters
	Int_t first = sub->GetXaxis()->GetFirst();
	Int_t last  = sub->GetXaxis()->GetLast();
	Int_t size= last-first+1;
	Int_t xmin = 0;
	Int_t xmax = size;
	Float_t* source = new Float_t[size];

	Bool_t *FixPos =new Bool_t[nfound];
	Bool_t *FixAmp = new Bool_t[nfound];
	for( Int_t i = 0; i< nfound ; i++){
		FixPos[i] = kFALSE;
		FixAmp[i] = kFALSE;
	}

	Float_t *PosX = new Float_t[nfound];
	Float_t *PosY = new  Float_t[nfound];
	Float_t *PosYp = s->GetPositionY();
	Float_t *PosXp = s->GetPositionX();
	for(int i=0;i<nfound;i++)
	{
		PosX[i] =(Float_t) (sub->FindFixBin(PosXp[i]) - first) ;	
		PosY[i] = PosYp[i];	
	}

	TSpectrumFit *pfit;
	TObjArray *dList  = new TObjArray();

	if(opt.Contains("onepeak"))
	{
		pfit=new TSpectrumFit(1);
		for( Int_t i=0; i<nfound; i++ )
		{
			pfit->SetFitParameters(xmin, xmax-1, 10000, 0.1, pfit->kFitOptimChiCounts,
					pfit->kFitAlphaHalving, pfit->kFitPower2,
					pfit->kFitTaylorOrderFirst);
			pfit->SetPeakParameters(2, kFALSE, PosX+i, (Bool_t *) FixPos+i, PosY+i, (Bool_t *) FixAmp+i);
			for ( Int_t j = 0; j < size; j++) source[j]=(Float_t)( sub->GetBinContent(j + first));
			if(opt.Contains("Stiefel"))
				pfit->FitStiefel(source);
			else
				pfit->FitAwmi(source);

			Double_t* CalcPositions = pfit->GetPositions();
			Double_t* CalcAmp= pfit->GetAmplitudes();
			Double_t* CalcAreas = pfit->GetAreas();
			Double_t* CalcAreasErr = pfit->GetAreasErrors();

			TH1 *d = (TH1 *)h->Clone("d");
			dList->Add(d);
			for (Int_t j = 0; j < size; j++) d->SetBinContent(j + first,source[j]);
			d->Add(bg,1.);
			d->SetLineColor(kGreen);

			Int_t	bin = first +  Int_t(CalcPositions[0] + 0.5);
			Double_t markerX = d->GetBinCenter(bin);
			Double_t markerY = d->GetBinContent(bin);
			if(opt.Contains("print"))
				printf("%9.3e\t%9.3e\t%9.3e\t%9.3e\t%9.3e\n",markerX,markerY,CalcAmp[0],CalcAreas[0],CalcAreasErr[0] );

			TPolyMarker * pm = (TPolyMarker*)d->GetListOfFunctions()->FindObject("TPolyMarker");
			if (pm) {
				d->GetListOfFunctions()->Remove(pm);
				delete pm;
			}
			pm = new TPolyMarker(1, &markerX,  &markerY);
			d->GetListOfFunctions()->Add(pm);
			pm->SetMarkerStyle(23);
			pm->SetMarkerColor(kBlack);
			pm->SetMarkerSize(1);
		}
	}else
	{
		pfit=new TSpectrumFit(nfound);
		pfit->SetFitParameters(xmin, xmax-1, 1000, 0.1, pfit->kFitOptimChiCounts,
				pfit->kFitAlphaHalving, pfit->kFitPower2,
				pfit->kFitTaylorOrderFirst);
		pfit->SetPeakParameters(2, kFALSE, PosX, (Bool_t *) FixPos, PosY, (Bool_t *) FixAmp);
		for ( Int_t j = 0; j < size; j++) source[j]=(Float_t)( sub->GetBinContent(j + first));
		if(opt.Contains("Stiefel"))
			pfit->FitStiefel(source);
		else
			pfit->FitAwmi(source);

		TH1 *d = (TH1 *)h->Clone("d");
		dList->Add(d);
		for (Int_t j = 0; j < size; j++) d->SetBinContent(j + first,source[j]);
		d->Add(bg,1.);

		d->SetLineColor(kGreen);

		Double_t* CalcPositions = pfit->GetPositions();
		Double_t* CalcAmp= pfit->GetAmplitudes();
		Double_t* CalcAreas = pfit->GetAreas();
		Double_t* CalcAreasErr = pfit->GetAreasErrors();
		for(int i=0;i<nfound;i++)
		{
			Int_t	bin = first +  Int_t(CalcPositions[i] + 0.5);
			PosX[i] = d->GetBinCenter(bin);
			PosY[i] = d->GetBinContent(bin);
			if(opt.Contains("print"))
				printf("%9.3e\t%9.3e\t%9.3e\t%9.3e\t%9.3e\n",PosX[i],PosY[i],CalcAmp[i],CalcAreas[i],CalcAreasErr[i] );

		}
		TPolyMarker * pm = (TPolyMarker*)d->GetListOfFunctions()->FindObject("TPolyMarker");
		if (pm) {
			d->GetListOfFunctions()->Remove(pm);
			delete pm;
		}
		pm = new TPolyMarker(nfound, PosX,  PosY);
		d->GetListOfFunctions()->Add(pm);
		pm->SetMarkerStyle(23);
		pm->SetMarkerColor(kBlack);
		pm->SetMarkerSize(1);
	}
	delete [] source;

	if(!opt.Contains("nodraw"))
	{
		TCanvas *Fit1 = gROOT->GetListOfCanvases()->FindObject("Fit1");
		if (!Fit1) delete Fit1;

		Fit1 = new TCanvas("Fit1","Fit1",10,10,1000,700);
		h->Draw("L");
		sub->Draw("same L");
		bg->Draw("same L");
		gPad->SetLogy(1);
		for(int i=0;i<dList->GetEntries();i++)
		{
			(TH1 *)dList->At(i)->Draw("SAME L");
		}
	}

}

void analysisSpectra()
{
	//TString fileName("tuhpge34-1.csv");
	//TString fileName("th180.csv");
	TString fileName("u180.csv");
	if(ReadData(fileName))
	{
		PlotSpectra("draw");
	//	plotFWHM();
		//Print2File(fileName);
		//plotEnergyCal();
		Analysis(th1,"one peak no draw Stie fel print");
	}
}

