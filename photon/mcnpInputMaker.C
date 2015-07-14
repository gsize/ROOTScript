#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

vector<double> energy;
vector<double> intensity;
//vector<double> strength;
double *strength;
vector<int> ensdf;
vector<int> ensdf1;
vector<double> active;
/*
   class Photon
   {
   private:
   double energy;
   double intensity;
   int ensdf;
   double strength;

   public:

   photon()
   {
   energy = 0.;
   intensity = 0.;
   ensdf = 0;
   strength =0.;
   };

//void ReadData();
void SetE(double E){energy =E;};
void SetI(double I){intensity=I;};
void SetE(int en){ensdf =en;};
void SetE(double S){strength =S;};

double GetE(){return energy   ;};
duoble GetI(){return intensity;};
int    GetE(){return ensdf    ;};
double GetE(){return strength ;};
};
*/
void ReadData()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("mcnpInputMaker.C","");
	dir.ReplaceAll("/./","/");
	ifstream fin0(Form("%sphoton_data.prn",dir.Data()));
	ifstream fin1(Form("%sa_data.prn",dir.Data()));

	int i =0;
	string str_tmp;
	TPRegexp re1("\\d( +\\d[0-9.+\\-Ee]+){2,2}");
	TPRegexp re2("\\d( +\\d[0-9.+\\-Ee]+)");
	while(getline(fin0,str_tmp))
	{
		TString str(str_tmp);
		if(str.Contains(re1))
		{
		int  d1;
		double d2;
		double d3;
		sscanf(str_tmp.c_str(),"%d%lf%lf",&d1,&d2,&d3);
		ensdf.push_back(d1);
		energy.push_back(d2/1.E+6);
		intensity.push_back(d3);

		i++;
		}
	}
	printf("i=%d,%d\n",i,ensdf.size());
	i=0;

	while(getline(fin1,str_tmp))
	{
		TString str(str_tmp);
		if(str.Contains(re2))
		{
		int  d1;
		double d2;
		sscanf(str_tmp.c_str(),"%d%lf",&d1,&d2);

		ensdf1.push_back(d1);
		active.push_back(d2);
		i++;
		}
	}

	printf("i=%d,%d\n",i,ensdf1.size());

	fin0.close();
	fin1.close();
}

void PlotSpectra()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("mcnpInputMaker.C","");
	dir.ReplaceAll("/./","/");
	ofstream fout(Form("%soutput_data.txt",dir.Data()));

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
		fout<<energy[i]<<"\t"<<strength[i]<<endl;
	}
	fout.close();
	gr->Scale(1./gr->Integral());//归一化
	//gr->Draw("apl");
	gStyle->SetOptStat(kFALSE);
	gr->Draw();
}

void Calculation()
{
	strength = new double[ensdf.size()];
	for(int i=0;i<ensdf.size();i++)
	{
		for(int j=0;j<ensdf1.size();j++)
		{
			if(ensdf[i] == ensdf1[j])
			{
				double I=0.;
				I=active[j]*intensity[i]/100.;
				//strength.push_back(I);
				strength[i]=I;
			}
		}
	}

	PlotSpectra();
}

void mcnpInputMaker()
{

	ReadData();
	Calculation();
}

