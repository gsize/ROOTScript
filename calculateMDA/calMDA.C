

#include "TCanvas.h"
#include "TF1.h"
#include "TString.h"
#include "TMath.h"

vector<double> energy;
vector<double> intensity;
vector<double> MDA_list;

int  GetGammaRay(int endfcode )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("calMDA.C","");
	dir.ReplaceAll("/./","/");

	TString dbname(Form("sqlite://%s/../../NuclearDatabase/nucleidata.sqlite",dir.Data()));
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
	//		ensdf.push_back(endfcode);
			delete row;
		}
		delete res;

		f->Close();
		return 1;
	}
}

void Calculate(int endfcode,double liveTime,double bg_rate, TF1 *fun_eff)
{
	double mda;
	double background =0.;

	background = bg_rate * liveTime;
	GetGammaRay( endfcode );
	for(int i=0;i<energy.size();i++)
	{
		mda = MDA(background,fun_eff->Eval(energy[i]),intensity[i],liveTime);
		MDA_list.push_back(mda);
		printf("%lf\t %lf\n",energy[i],intensity[i]);
	}
}

double eff_fun(double *x, double *par)
{
	double eff=0.;
	for(int i=1;i<7;i++)
	{
		eff += par[i-1] * TMath::Power(x[0],2-i);
	}
	return (TMath::Exp(eff));
}

double MDA(double bg, double eff, double re,double t)
{
	double k=4.65;
	//double k=1.645;

	return (k*TMath::Sqrt(bg)/eff/re/t);
}
void plotEff(TF1 *fun_eff)
{
	TCanvas* c1 = new TCanvas("EfficiencyPlot", "  ");

	fun_eff->GetXaxis()->SetTitle("Energy/MeV");
	fun_eff->GetXaxis()->CenterTitle();
	fun_eff->GetYaxis()->SetTitle("Efficiency");
	fun_eff->GetYaxis()->CenterTitle();
	fun_eff->SetLineColor(kBlack);
	//gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	fun_eff->Draw();
}

void calMDA()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("calMDA.C","");
	dir.ReplaceAll("/./","/");

	bool effFlg = 0;
	TF1 *fun_eff=  new TF1("Efficiency",eff_fun,0.10,1.5,6);
	//fun_eff->SetParameters(0.109574,-7.255860, 1.839348,  -0.462271, 0.060579, -0.003032);
	fun_eff->SetParameters(-0.349521, -6.056041, 0.605647, -0.068800, 0.003151, -0.000059);
	if(effFlg==1)
		plotEff(fun_eff);
int endfcode = 531330;
double liveTime=200;
double bg_rate=12; 

	Calculate( endfcode,liveTime,bg_rate, fun_eff);
	TString outFile;
	outFile = dir + "outdata_MDA1.txt";
	ofstream out;
	out.open(outFile.Data());
	for(int ii=0; ii<MDA_list.size(); ii++)
		out<< MDA_list[ii] <<endl;
	out.close();

}
