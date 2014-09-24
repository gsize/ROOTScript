
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
	double k=1.645;

	return (k*TMath::Sqrt(bg)/eff/re/t);
}

void jisuan()
{
	//double par[6]={0.109574,-7.255860, 1.839348,  -0.462271, 0.060579, -0.003032};
	TF1 *fun_eff=  new TF1("tf1jsuan",eff_fun,0.10,1.5,6);
	fun_eff->SetParameters(0.109574,-7.255860, 1.839348,  -0.462271, 0.060579, -0.003032);
	//f1->Draw();
	int plot_flag=0;  //calculate the MDA value of detector
	if(plot_flag){
		int num_peak=5;
		double energy[]={155.04,290.669,316.51,129.43,290.669};
		double background[]={6174.,1694.,1459.,4657.,682039.};
		double branch_rate[]={0.1561,0.004,0.8271,0.265,0.004};
		double m_time[]={460.,460.,460.,460.,460.};

		for(int i=0;i<num_peak;i++)
			printf("%8.3lf kev,MDA(Bq):%8.3lf\n",energy[i],MDA(background[i],fun_eff->Eval(energy[i]/1000.),branch_rate[i],m_time[i]));
	}

	//Calculation of Re-188
	TString date_time[]={"2013-09-24 15:23:58", "2013-09-24 15:31:11", "2013-09-24 15:35:26"
		,"2013-09-24 15:38:49", "2013-09-24 15:41:10", "2013-09-24 15:44:31"
			,"2013-09-24 15:47:06", "2013-09-24 15:43:56", "2013-09-24 15:53:34"
	};
	Double_t active[] = {1.42e+5, 1.35e+5, 1.40e+5
		,2.40e+5,2.59e+5,2.54e+5
			,4.04e+5,3.91e+5,3.99e+5
	};
	Double_t meansure_time[]={116.08,115.02,100.,100.,100.,100.,100.,100.,100.};
	int num_sample = 9;
    int unit_Ci = 0;

	if(unit_Ci){
		for(int i=0;i<num_sample;i++)
			active[i]/= 3.7e+10;
	}
	TGeoManager *geom = new TGeoManager("calculation","Calculation the elementNR active");
	TGeoElementTable *table = gGeoManager->GetElementTable();
	TGeoElementRN *c14 = table->GetElementRN(14,6);
	TDatime T1;
	TDatime T0;
	T1.Set("2013-09-24 15:23:00");
	double *active_0 = new double [num_sample];
	double *time = new double [num_sample];
	for(int i=0;i<num_sample;i++){
		T0.Set(date_time[i].Data());
		time[i]=(Double_t)(T1.Convert())-T0.Convert()-0.5*meansure_time[i];
		//cout<<"time length(s):"<<time[i]<<endl;
	}
	// Radioactive material
	TGeoElement *elem = table->FindElement("Re");
	TGeoElementRN *elemNR = table->GetElementRN(188,elem->Z());
	TObjArray *vect = new TObjArray();
	TGeoBatemanSol *sol;
	for(int j=0;j<num_sample;j++){
		elemNR->FillPopulation(vect, 0.000001);
		sol = elemNR->Ratio();
		sol->Normalize(active[j]);
		//sol->SetRange(0,time);
		active_0[j]=sol->Concentration(time[j]);
		cout<<elemNR->GetName()<<" evolution after " <<time[j]<<" seconds,the active: "<<active_0[j]<<endl;
		elemNR->ResetRatio();
		//vect->Clear();
	}

	for(int k=0;k<3;k++){
		cout<<"sample_"<<k<<" Mean: "<<TMath::Mean(3,active_0+k*3)<<" RMS: "<<TMath::RMS(3,active_0+k*3);
		if(unit_Ci)
			cout<<" Ci"<<endl;
		else
			cout<<" Bq"<<endl;
	}
	//Calculation W-188
	int flag_W188 =0;
	if(flag_W188)
	{
		TDatime T1;
		TDatime T0;
		double active_W188=3.7e+10;
		T0.Set("2013-09-24 15:23:00");
		T1.Set("2013-10-02 10:23:00");
		double time_W188=0.;

		time_W188 = (Double_t)(T1.Convert())-T0.Convert();
		TGeoElement *elem_W = table->FindElement("W");
		TGeoElementRN *elemNR_W = table->GetElementRN(188,elem_W->Z());
		TObjArray *vect_W = new TObjArray();
		elemNR_W->FillPopulation(vect_W, 0.00001);
		TGeoBatemanSol *sol_W = elemNR_W->Ratio();

		sol_W->Normalize(active_W188);
		double active_1=sol_W->Concentration(time_W188);
		cout<<elemNR_W->GetName()<<" evolution after " <<time_W188/3600./24.<<" days,the active: "<<active_1<<endl;
	}
}
