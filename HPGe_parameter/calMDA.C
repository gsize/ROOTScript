
void ReadAndCalculate(std::vector<double> &MDA_list, TF1 *fun_eff)
{	
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("calMDA.C","");
	dir.ReplaceAll("/./","/");
	TString pathFile;
	pathFile = dir + "energy_rate.txt";
	ifstream in;
	in.open(pathFile);
	if(!(in.is_open()))
	{
		printf("open file failed!");
		return;
	}

	double liveTime=0.;
	double bg_rate = 0.; 
	double background =0.;
	string str_tmp;

	//	background = bg_rate * liveTime;
	while(getline(in,str_tmp)) {
		double energy,branch,livetime,backgroud, mda;
		if(str_tmp[0] == '#') continue;
		sscanf(str_tmp.c_str(),"%lf%lf%lf%lf",&energy, &branch,&liveTime,&background);
		mda = MDA(background,fun_eff->Eval(energy/1000.),branch/100.0,liveTime);
		MDA_list.push_back(mda);
		//printf("%8.3lf kev,MDA(Bq):%8.3lf\n",energy,mda);
	}
	in.close();
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

	TF1 *fun_eff=  new TF1("Efficiency",eff_fun,0.10,1.5,6);
	//fun_eff->SetParameters(0.109574,-7.255860, 1.839348,  -0.462271, 0.060579, -0.003032);
	fun_eff->SetParameters(-0.349521, -6.056041, 0.605647, -0.068800, 0.003151, -0.000059);

	plotEff(fun_eff);

	vector<double> MDA_list;

	ReadAndCalculate(MDA_list,fun_eff);
	TString outFile;
	outFile = dir + "outdata_MDA1.txt";
	ofstream out;
	out.open(outFile.Data());
	for(int ii=0; ii<MDA_list.size(); ii++)
		out<< MDA_list[ii] <<endl;
	out.close();

}
