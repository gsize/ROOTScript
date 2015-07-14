// 衰变核素相关class的使用

void ReadInputFile( std::vector<string> &element, std::vector<double> &time, std::vector<double> &Activity ,bool flag= 0)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("NucleiDecay.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;
	if(flag== 0)
		pathName = dir + "input0.txt";
	else 
		pathName = dir + "input1.txt";

	ifstream in;
	in.open(pathName.Data());
	if(!(in.is_open()))
	{
		printf("open file failed!");
		return;
	}

	string str_tmp;
	TPRegexp re("^#");
	while(getline(in,str_tmp)) {
		TString str(str_tmp);
		if(str.Contains(re))
			continue;	
		double timeSTR;
		double activitySTR;
		char elementSTR[9];
		if( flag== 0)
			sscanf(str_tmp.c_str(),"%s%lg%lg",elementSTR,&timeSTR, &activitySTR);
		else
		{
			char day0[15],day1[15];
			char time0[10],time1[10];
			sscanf(str_tmp.c_str(),"%s%s%s%s%s%lg",elementSTR,day0,time0,day1,time1, &activitySTR);

			TDatime t0(Form("%s %s",day0,time0));
			TDatime t1(Form("%s %s",day1,time1));
			timeSTR = (double)(t1.Convert())-t0.Convert();
		}
		string tmpSTR(elementSTR);
		element.push_back(tmpSTR);
		time.push_back(timeSTR);
		Activity.push_back(activitySTR);
	}
	in.close();
}

void CalculateActivity( const TString &element_name,double time, Double_t Activity0 = 1.0)
{
	int ENDFCode;
	ENDFCode = elemNR2ENDFCode(element_name);
	TGeoElementTable *table = gGeoManager->GetElementTable();
	TGeoElementRN *elemNR = table->GetElementRN(ENDFCode );

	// Radioactive material
	TObjArray *vect = new TObjArray();
	TGeoBatemanSol *sol;

	elemNR->FillPopulation(vect, 0.000001);
	sol = elemNR->Ratio();

	sol->Normalize(Activity0);
	//sol->SetRange(0,time);
	double active=sol->Concentration(time);
	cout<<elemNR->GetName()<<": "<<Activity0<< "  evolution after " <<time<<" s,the active: "<<active<<"specificActivity:"<<elemNR->GetSpecificActivity()<<endl;
	elemNR->ResetRatio();

	delete vect;
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
				/*  TF1 *func = (TF1*)can->FindObject(
					Form("conc%s",sol->GetElement()->GetName()));
					if (func) {
					if (!strcmp(can->GetName(),"c1")) func->SetTitle(
					"Concentration of C14 derived elements;time[s];Ni/N0(C14)");
					else func->SetTitle(
					"Concentration of elements derived from mixture Ca53+Sr78;\
					time[s];Ni/N0(Ca53)");
					} */  
			}   
			else      sol->Draw("SAME");
			TString nameRN;
			double t;
			t=0.5*(tmin+tmax);
			nameRN.Form("%s", elem->GetName());
			TLatex *tex = new TLatex(t,sol->Concentration(t),nameRN);
			tex->SetTextSize(0.0388601);
			tex->SetTextColor(sol->GetLineColor());
			tex->Draw();
		}
	}
}

void NucleiDecay(bool flag = 0)
{
	TGeoManager *geom = new TGeoManager("","");

	std::vector<string> ElementList;
	std::vector<double> TimeList;
	std::vector<double> ActivityList; 

	ReadInputFile(ElementList, TimeList, ActivityList,flag);

	for(int i=0; i<ElementList.size();i++)
	{
		TString element_name(ElementList[i]);
		CalculateActivity( element_name,TimeList[i], ActivityList[i]);
	}
}
