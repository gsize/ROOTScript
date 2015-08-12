#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

vector<double> energy;
vector<double> intensity;
vector<double> strength;
vector<int> ensdf;
vector<double> active;

int GetGammaRay(int endfcode, std::vector<double> &energy, std::vector<double> &intensity );

void ReadData()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");
	ifstream fin1(Form("%sa_data.prn",dir.Data()));

	int i =0;
	string str_tmp;

	while(getline(fin1,str_tmp))
	{
		TPRegexp re2("\\d( +\\d[0-9.+\\-Ee]+)");
		TString str(str_tmp);
		if(str.Contains(re2))
		{
			int  d1;
			double d2;
			sscanf(str_tmp.c_str(),"%d%lf",&d1,&d2);

			active.push_back(d2);
			if(1!=GetGammaRay(d1*10,d2,energy,intensity))
				printf("don't have nuclear %d\n",d1*10);
			i++;
		}
	}

	printf("i=%d,%zu\n",i,active.size());

	fin1.close();
}

int  GetGammaRay(int endfcode,double ac, std::vector<double> &energy, std::vector<double> &intensity )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");

	TString dbname(Form("sqlite://%snucleidata.sqlite",dir.Data()));
	TSQLiteServer * f = new TSQLiteServer(dbname.Data());

	TSQLiteRow *row;
	TSQLiteResult *res;
	int nfield;
	//int endfcode = elemNR2ENDFCode(el);
	TString sql(Form("select * from rayInfor where ENDFCode=%d and rayType = \"G\" ",endfcode));
	res = (TSQLiteResult*) f->Query(sql.Data());
	if(res==0)
	{
		f->Close();
		return 0;
	}
	else
	{
		nfield = res->GetFieldCount();
		while((row = (TSQLiteRow *)( res->Next())))
		{
			TString str(row->GetField(3));
			energy.push_back(str.Atof());
			str= row->GetField(2);
			intensity.push_back(str.Atof());
			strength.push_back(ac*str.Atof());
			ensdf.push_back(endfcode);
			delete row;
		}
		delete res;

		f->Close();
		return 1;
	}
}

void Print2File()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");
	ofstream fout(Form("%soutput_data.txt",dir.Data()));

	for(int i=0;i<energy.size();i++)
	{
		fout<<ensdf[i]<<"\t"<<energy[i]<<"\t"<<strength[i]<<endl;
	}
	fout.close();
}

void PlotSpectra()
{
	double maxE=0.;
	maxE=TMath::MaxElement(energy.size(),&energy[0]);
	TCanvas* c1 = new TCanvas("C1", "");
	TH1D *gr =new TH1D("th1"," ",8192,0.,1.05 * maxE);
	for(int i=0;i<energy.size();i++)
	{
		int binFlag=0;
		binFlag = gr->FindBin(energy[i]);
		double BC= 0.;
		BC = gr->GetBinContent(binFlag);
		gr->SetBinContent(binFlag,BC + strength[i]);
	}
	//	gr->Scale(1./gr->Integral());//归一化
	//gr->Draw("apl");
	gStyle->SetOptStat(kFALSE);
	gr->Draw();
}

void gammaSpectraMaker()
{

	ReadData();
	Print2File();
	PlotSpectra();
}

