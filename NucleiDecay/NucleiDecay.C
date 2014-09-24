// 衰变核素相关class的使用

void ReadInputFile(std::vector<int> &mass, std::vector<string> &element, std::vector<double> &time, std::vector<double> &Activity ,bool flag= 0)
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
	while(getline(in,str_tmp)) {
		if(str_tmp[0] == '#')
			continue;	
		double timeSTR;
		double activitySTR;
		int massSTR;
		char elementSTR[5];
		if( flag== 0)
			sscanf(str_tmp.c_str(),"%d%s%lg%lg",&massSTR,elementSTR,&timeSTR, &activitySTR);
		else
		{
			char day0[15],day1[15];
			char time0[10],time1[10];
			sscanf(str_tmp.c_str(),"%d%s%s%s%s%s%lg",&massSTR,elementSTR,day0,time0,day1,time1, &activitySTR);
			
			TString daytime(day0);
			daytime += " ";
			daytime += time0;
			TDatime t0(daytime.Data());
			daytime.Clear();

			daytime= day1 ;
			daytime += " ";
			daytime += time1;
			TDatime t1(daytime.Data());
			timeSTR = (double)(t1.Convert())-t0.Convert();
			cout<< timeSTR<<endl;

		}
		//cout << massSTR<<elementSTR <<timeSTR << activitySTR <<endl;
		mass.push_back(massSTR);
		string tmpSTR(elementSTR);
		element.push_back(tmpSTR);
		time.push_back(timeSTR);
		Activity.push_back(activitySTR);
	}
	in.close();
}

void CalculateActivity( const TString &element_name, Int_t element_mass,double time, Double_t Activity0 = 1.0)
{
	TGeoElementTable *table = gGeoManager->GetElementTable();

	// Radioactive material
	TObjArray *vect = new TObjArray();
	TGeoBatemanSol *sol;
	TGeoElement *elem = table->FindElement(element_name.Data());
	TGeoElementRN *elemNR = table->GetElementRN(element_mass,elem->Z());

	elemNR->FillPopulation(vect, 0.000001);
	sol = elemNR->Ratio();

	sol->Normalize(Activity0);
	//sol->SetRange(0,time);
	double active=sol->Concentration(time);
	cout<<elemNR->GetName()<<": "<<Activity0<< "  evolution after " <<time<<" s,the active: "<<active<<endl;
	elemNR->ResetRatio();

	delete vect;
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

	std::vector<int> MassList;
	std::vector<string> ElementList;
	std::vector<double> TimeList;
	std::vector<double> ActivityList; 

	ReadInputFile(MassList, ElementList, TimeList, ActivityList,flag);

	for(int i=0; i<MassList.size();i++)
	{
		TString element_name(ElementList[i]);
		CalculateActivity( element_name, MassList[i],TimeList[i], ActivityList[i]);
	}
}
