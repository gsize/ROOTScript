#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

vector<double> energy;
vector<double> intensity;
vector<double> strength;
vector<int> ensdf;
vector<double> active;
//vector<string> nuclearList;

int GetGammaRay(int endfcode, double ac, std::vector<double> &energy, std::vector<double> &intensity );
int GetGammaRayTXT(int endfcode, double ac, std::vector<double> &energy, std::vector<double> &intensity );

int elemNR2ENDFCode(TString elementName)
{
	int ENDFCode=0;

	TPRegexp re("^[A-Za-z]{1,2}-?\\d+[Mm]?");
	TPRegexp re1("^[A-Za-z]{1,2}-?\\d+");
	TPRegexp re2("^[A-Za-z]{1,2}-?\\d+[Mm]$");

	TGeoElementTable *table = gGeoManager->GetElementTable();
	if(elementName.Contains("-"))
		elementName.ReplaceAll("-","");
	if(elementName.Contains(re))
	{
		int a=0,z=0,iso=0;
		char str[4];

		sscanf(elementName.Data(),"%[A-Za-z]%d",str,&a);
		TGeoElement *elem = table->FindElement(str);
		z = elem->Z();

		if(elementName.Contains(re2))
			iso=1;
		else
			iso=0;

		ENDFCode = 10000*z+10*a+iso;
	}
	else
		ENDFCode=0;

	return ENDFCode;
}

void ReadData(TString inputfile)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");
	ifstream fin1(Form("%s%s",dir.Data(),inputfile.Data()));

	int i =0;
	string str_tmp;
    //TPRegexp re2("\\d( +\\d[0-9.+\\-Ee]+)");
	TPRegexp re2("([A-Za-z]{1,2}-?\\d+[Mm]?)([ \t]+\\d[0-9.+\\-Ee]+)");

	while(getline(fin1,str_tmp))
	{
		TString str(str_tmp);
		if(str.Contains(re2))
		{
			int  d1;
			double d2;
            char nuclear[6];
			//sscanf(str_tmp.c_str(),"%d%lf",&d1,&d2);
			sscanf(str.Data(),"%s%lf",nuclear,&d2);
			TString nuclearName(nuclear);
			
			d1 = elemNR2ENDFCode(nuclearName);
			active.push_back(d2);
			if(0==GetGammaRay(d1,d2,energy,intensity)) 
			//if(0==GetGammaRayTXT(d1,d2,energy,intensity)) 
				printf("%d:don't have nuclear %s: %d\n",i,nuclear,d1);
			i++;
		}
	}

	printf("i=%d nuclear,%d\n",i,active.size());

	fin1.close();
}

int GetGammaRayTXT(int endfcode, double ac, std::vector<double> &energy, std::vector<double> &intensity )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");

	ifstream in;
	in.open(Form("%sicrp-07.rad",dir.Data()));
	if(!(in.is_open()))
	{
		cout<< "open in file failed!"<<endl;
		return 0;
	}

	string str_tmp;
	int i=1;
	TPMERegexp re_start1("^[a-zA-Z]+-\\d+");
	int d1=0;
	int rayNum=0;
	int hasgammaray=0;
	while(getline(in,str_tmp)) 
	{
		TString str(str_tmp);
		if(str.Contains((TPRegexp)re_start1))
		{
			char s1[10],s2[10];
			sscanf(str.Data(),"%s %s %d",s1,s2,&rayNum);
			TString strt(s1);
			d1 = elemNR2ENDFCode(strt);
        	if(d1 == endfcode)
			{
				for(int i=0;i<rayNum;i++)
				{
					double b,e;
					int t;
					char s3[5];
					getline(in,str_tmp);
					TString str1(str_tmp);
					sscanf(str1.Data(),"%d %lf %lf %s",&t,&b,&e,s3);
					if(s3[0]=='G')
					{
						energy.push_back(e);
						intensity.push_back(b);
						strength.push_back(ac*b);
						ensdf.push_back(endfcode);
					}
				}
				break;
			}
		}
	}
	hasgammaray=energy.size();
	in.close();
return hasgammaray;
}

int  GetGammaRay(int endfcode, double ac, std::vector<double> &energy, std::vector<double> &intensity )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");

	TString dbname(Form("sqlite://%sdata/nucleidata.sqlite",dir.Data()));
	TSQLiteServer *f = new TSQLiteServer(dbname.Data());

	TSQLiteRow *row;
	TSQLiteResult *res;
	int nfield;
	int hasgammaray=0;
	//int endfcode = elemNR2ENDFCode(el);
	TString sql(Form("select * from rayInfor where ENDFCode=%d and rayType = \"G\" ",endfcode));
	res = (TSQLiteResult*) f->Query(sql.Data());
	if(res==0)
	{
		f->Close();
		hasgammaray=0;
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
	}
	f->Close();
	hasgammaray=energy.size();
	return hasgammaray;
}

void Print2File(TString input)
{
	if(energy.size()>0)
	{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("gammaSpectraMaker.C","");
	dir.ReplaceAll("/./","/");
	input.ReplaceAll(".","-");
	ofstream fout(Form("%s%s-output.txt",dir.Data(),input.Data()));
	
	TGeoElementTable *table = gGeoManager->GetElementTable();
	TGeoElementRN* nu;
	fout<<"nuclear"<<"\t"<<"HalfLife(s)"<<"\t"<<"energy(MeV)"<<"\t"<<"intensity"<<"\t"<<"strength"<<endl;
	for(int i=0;i<energy.size();i++)
	{
		nu = table->GetElementRN(ensdf[i]);
		fout<<nu->GetName()<<"\t"<<nu->HalfLife()<<"s\t";
		fout<<energy[i]<<"\t"<<intensity[i]<<"\t"<<strength[i]<<endl;
	}
	fout.close();
	}else
		printf("No Data!\n");
}

void PlotSpectra()
{
	if(energy.size()>0)
	{
	double maxE=0.;
	maxE=TMath::MaxElement(energy.size(),&energy[0]);
	TCanvas* c1 = new TCanvas("C1", " ");
	TH1D *gr =new TH1D("th1"," ",8192,0.,1.05 * maxE);
	for(int i=0;i<energy.size();i++)
	{
		int binFlag=0;
		binFlag = gr->FindBin(energy[i]);
		double BC= 0.;
		BC = gr->GetBinContent(binFlag);
		gr->SetBinContent(binFlag,BC + strength[i]);
	}
		gr->Scale(1./gr->Integral());//归一化
	//gr->Draw("apl");
	gStyle->SetOptStat(kFALSE);
	gr->GetXaxis()->SetTitle("Energy/MeV");
	gr->GetXaxis()->CenterTitle();
	gr->GetYaxis()->SetTitle("strength");
	gr->GetYaxis()->CenterTitle();
	gr->Draw();
	}else
		printf("No Data!\n");
}

void gammaSpectraMaker(TString input="inputData.txt")
{
	TGeoManager *geom = new TGeoManager("","");
	ReadData(input);
	Print2File(input);
	PlotSpectra();
}

