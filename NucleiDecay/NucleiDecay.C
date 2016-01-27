// 衰变核素相关class的使用

int elemNR2ENDFCode(TString elementName);

void ReadInputFile( std::vector<string> &element, std::vector<double> &time, std::vector<double> &Activity )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("NucleiDecay.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;

		pathName = dir + "input0.txt";

	ifstream in;
	in.open(pathName.Data());
	if(!(in.is_open()))
	{
		printf("open file failed!");
		return;
	}

	string str_tmp;
	TPRegexp re1("([A-Za-z]{1,2}-?\\d{1,3}[Mm]?)([ \t]+\\d[0-9.+\\-Ee]+){2,2}");
	TPRegexp re2("([A-Za-z]{1,2}-?\\d{1,3}[Mm]?)([ \t]+\\d[0-9.+\\-Ee]+)([ \t]+(\\d{4,4}(-\\d{1,2}){2,2} +\\d{1,2}(:\\d{1,2}){2,2})){2,2}");
	TPRegexp re("^#");
	while(getline(in,str_tmp)) {
		TString str(str_tmp);
		if(str.Contains(re))
			continue;	
		double timeSTR;
		double activitySTR;
		char elementSTR[9];
		if(str.Contains(re1) || str.Contains(re2))
		{
		if(str.Contains(re1))
		{
			sscanf(str_tmp.c_str(),"%s%lg%lg",elementSTR, &activitySTR,&timeSTR);
		}
		if(str.Contains(re2))
		{
			char day0[15],day1[15];
			char time0[10],time1[10];
			sscanf(str_tmp.c_str(),"%s%lg%s%s%s%s",elementSTR, &activitySTR,day0,time0,day1,time1);

			TDatime t0(Form("%s %s",day0,time0));
			TDatime t1(Form("%s %s",day1,time1));
			timeSTR = (double)(t1.Convert())-t0.Convert();
		}
		string tmpSTR(elementSTR);
		element.push_back(tmpSTR);
		time.push_back(timeSTR);
		Activity.push_back(activitySTR);
		}
	}
	in.close();
}

int elemNR2ENDFCode(TString elementName)
{
	int ENDFCode;

	TPRegexp re("^[A-Z][a-z]?-\\d+m?");
	TPRegexp re1("^[A-Z][a-z]?-\\d+");
	TPRegexp re2("^[A-Z][a-z]?-\\d+m$");

	TGeoElementTable *table = gGeoManager->GetElementTable();
	if(elementName.Contains(re))
	{
		int a=0,z=0,iso=0;
		char str[4];

		sscanf(elementName.Data(),"%[A-Za-z]-%d",str,&a);
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

void GetGammaRay(const TString el, std::vector<double> &energy, std::vector<double> &intensity )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("NucleiDecay.C","");
	dir.ReplaceAll("/./","/");

	TString dbname(Form("sqlite://%s/../gammaSpectraMaker/nucleidata.sqlite",dir.Data()));
	TSQLiteServer * f = new TSQLiteServer(dbname.Data());

	TSQLiteRow *row;
	TSQLiteResult *res;
	int nfield;
	int endfcode = elemNR2ENDFCode(el);
	TString sql(Form("select * from rayInfor where ENDFCode=%d and rayType = \"G\" ",endfcode));
	res = (TSQLiteResult*) f->Query(sql.Data());
	nfield = res->GetFieldCount();

	while((row = (TSQLiteRow *)( res->Next())))
	{
		TString str(row->GetField(3));
		energy.push_back(str.Atof());
		str= row->GetField(2);
		intensity.push_back(str.Atof());
		//	printf("%12s%12s%12s\n",row->GetField(0),row->GetField(3),row->GetField(2));
		delete row;
	}
	delete res;

	f->Close();
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
//Calculate the  MDA value of detector.
//bg is the count of background
//eff is the efficiency
//re is branching ratio
//t is the live time
double MDA(double bg,double eff,double re,double t)
{
	//double k=1.645;
	double k= 4.65;

	return (k*TMath::Sqrt(bg)/eff/re/t);
}

double CalculateActivity( const TString &element_name,double time, Double_t Activity0 = 1.0)
{
	int ENDFCode;
	ENDFCode = elemNR2ENDFCode(element_name);
	TGeoElementTable *table = gGeoManager->GetElementTable();
	TGeoElementRN *elemNR = table->GetElementRN(ENDFCode );

	// Radioactive material
	TObjArray *vect = new TObjArray();
	TGeoBatemanSol *sol;

	elemNR->FillPopulation(vect, 0.000001,Activity0);

	sol = elemNR->Ratio();
	//sol->SetRange(0,time);
//	printf("%s specificActivity = %.3E Bq/g\n",elemNR->GetName(),elemNR->GetSpecificActivity());
	//elemNR->Print();
	double active=sol->Concentration(time);

	elemNR->ResetRatio();
	delete vect;

	return active;
}

void DrawPopulation(TObjArray *vect, TCanvas *can, Double_t tmin=0., 
		Double_t tmax=0., Bool_t logx=kFALSE)
{
	Int_t n = vect->GetEntriesFast();
	//TGeoElementRN *elem;
	//TGeoBatemanSol *sol;
	can->SetLogy();

	if (logx) can->SetLogx();
	for (Int_t i=0; i<n; i++) {
		TGeoElement *el = (TGeoElement*)vect->At(i);
		if (!el->IsRadioNuclide()) continue;
		TGeoElementRN *elem = (TGeoElementRN *)el;
		TGeoBatemanSol *sol = elem->Ratio();
		if (sol) {
			sol->SetLineColor(1+(i%9));
			sol->SetLineWidth(2);
			if (tmax>0.) sol->SetRange(tmin,tmax);
			if (i==0) {
				sol->Draw();
				TF1 *func = (TF1*)can->FindObject(Form("conc%s",sol->GetElement()->GetName()));
				if (func) {
					func->SetTitle(Form("Concentration of %s derived elements;time[year];Ni/N0",sol->GetElement()->GetName()));
					func->GetXaxis()->SetTimeDisplay(1);
					func->GetXaxis()->SetTimeOffset(0);
					func->GetXaxis()->SetTimeFormat("%y%F2000-01-01 00:00:01");
				}
			}
			else      sol->Draw("SAME");
			TString nameRN;
			double t;
			//t=0.5*(tmin+tmax);
			if(i%2==0)
				t=0.6*(tmin+tmax);
			else
				t=0.3*(tmin+tmax);
			nameRN.Form("%s", elem->GetName());
			TLatex *tex = new TLatex(t,sol->Concentration(t),nameRN);
			tex->SetTextSize(0.0388601);
			tex->SetTextColor(sol->GetLineColor());
			tex->Draw();
		}
	}
}

void NucleiDecay()
{
	TGeoManager *geom = new TGeoManager("","");

	std::vector<string> ElementList;
	std::vector<double> TimeList;
	std::vector<double> ActivityList;
	std::vector<double> energy;
	std::vector<double> branch; 

	ReadInputFile(ElementList, TimeList, ActivityList);

	for(int i=0; i<ElementList.size();i++)
	{
		TString element_name(ElementList[i]);
		double a1=0.;
		a1 = CalculateActivity( element_name,TimeList[i], ActivityList[i]);
		printf("%s after %.3E s ,activity = %.3E\n",element_name.Data(),TimeList[i] ,a1);
	}
	/*	
		TString em("Co-60");
		GetGammaRay(em,energy,branch);
		for(int i=0;i<energy.size();i++)
		cout<<energy[i]<<"\t"<<branch[i]<<endl;
		*/
	delete geom;
}
