//绘制高纯锗探测器的能量、半高宽、效率曲线

#include <vector>

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

int plotLine(TString lineName, int i, double *x,double *y, double *yErrors)
{
	TString yLabel;
	TCanvas* c1 = new TCanvas(lineName, lineName);
	TGraphErrors *gr = new TGraphErrors(i,x,y, 0 ,yErrors);
	TF1 *fun_fit = 0;
	if(lineName.Contains("eff"))
	{
		fun_fit = new TF1(lineName,eff_fun,0.030,1.500.,6);
		fun_fit->SetParameters(-0.349521, -6.056041, 0.605647, -0.068800, 0.003151, -0.000059);
	}else{
		fun_fit = new TF1(lineName,"pol2",0.030,1.500);
	}
	gr->Fit(fun_fit,"R+");

	yLabel = lineName;
	gr->GetXaxis()->SetTitle("Energy/MeV");
	gr->GetXaxis()->CenterTitle();
	gr->GetYaxis()->SetTitle(yLabel);
	gr->GetYaxis()->CenterTitle();
	gr->SetLineColor(kBlack);
	gr->SetTitle(yLabel);
	gr->SetMarkerStyle(21);
	//gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	//fun_eff->Draw();
	gr->Draw("AP");
	return 1;
}

int plotEfficiency()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("plotGraph.C","");
	dir.ReplaceAll("/./","/");
	std::vector<double> xList;
	std::vector<double> yList;
	std::vector<double> yErrorsList;

	TCanvas* c1 = new TCanvas("Efficiency", "Efficiency");
	TMultiGraph *mg =new TMultiGraph();
	TLegend *leg = new TLegend(0.75,0.75,0.9,0.9);

	TString name[6]={"eff_ba133_00_20130325.Txt", "eff_ba133_04_20130415.Txt"
		,"eff_ba133_08_20130502.Txt","eff_ba133_12_20130316.Txt"
			,"eff_ba133_16_20130422.Txt","eff_ba133_24_20130318.Txt" };		
	TString legendName[]={"0cm","4cm","8cm","12cm","16cm","24cm"};
	for(int i=0;i<6;i++)
	{
		if(i==3 || i == 5) continue;
		TString pathName;
		pathName = dir + name[i];
		ReadFile(pathName,xList,yList,yErrorsList);
		TGraphErrors *gr = new TGraphErrors((Int_t)xList.size(),&xList[0],&yList[0],0,&yErrorsList[0]);
		TF1 *fun_fit = 0;
		fun_fit = new TF1(legendName[i],eff_fun,0.030,1.5,6);
		//fun_fit->SetParameters(-0.349521, -6.056041, 0.605647, -0.068800, 0.003151, -0.000059);
		fun_fit->SetParameters(-0.2846, -7.533462, 0.635764, -0.0788, 0.004049, -0.000083);
		gr->Fit(fun_fit,"R+");
		TString outfile;
		outfile ="outfile" +legendName[i];
		outfile+=".txt";
ReadAndCalculate(outfile, fun_fit);
		yErrorsList.clear();
		xList.clear();
		yList.clear();
		gr->SetLineColor(kBlack+i);
		//gr->SetTitle(yLabel);
		gr->SetMarkerStyle(20+i);
		mg->Add(gr);
		leg->AddEntry(gr,legendName[i].Data(),"PL");
	}
	gPad->SetLogy(1);
	//fun_eff->Draw();
	mg->Draw("AP");
	mg->GetXaxis()->SetTitle("Energy/MeV");
	mg->GetXaxis()->CenterTitle();
	mg->GetYaxis()->SetTitle("Efficiency");
	mg->GetYaxis()->CenterTitle();
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	gPad->Modified();
	leg->Draw();
	return 1;
}

void ReadFile(TString filename,std::vector<double> &xList,std::vector<double> &yList,std::vector<double> &yErrorsList)
{
	ifstream in;
	in.open(filename.Data());
	if(!(in.is_open()))
	{
		printf("open file failed! %s\n",filename.Data());
		return;
	}

	int i=0; 
	string str_tmp;
	while(getline(in,str_tmp)) {
		i++;
		double x,y,yFit,yError,yDelta; 
		if(i>5){
			sscanf(str_tmp.c_str(),"%lf%lf%lf%lf",&x, &y, &yFit, &yError);
			xList.push_back(x*0.001);
			yList.push_back(y);
			yDelta =TMath::Abs( y * yError *0.01);
			//cout<<yError<<"\t"<<yDelta<<endl;
			yErrorsList.push_back(yDelta);
		}
	}
	in.close();

}

void ReadAndCalculate(const TString &outfile, TF1 *fun_eff)
{	
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("plotGraph.C","");
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
	TString str_tmp;

std::vector<double> eff_list;
std::vector<double> energy_list;
std::vector<double> branch_list;
std::vector<double> MDA_list;

//	background = bg_rate * liveTime;
	while(1) {
		if(in.eof()) break;
		str_tmp.ReadLine(in);
		if(str_tmp.Contains("BackgroundRate") && str_tmp.Contains("LiveTime"))
		{
			sscanf(str_tmp.Data(),"%*[^=]=%lf%*[^=]=%lf",&bg_rate,&liveTime);
			cout <<"backgroundrate:"<<bg_rate<<"liveTime:"<<liveTime<<endl;
		}else
		{
			double energy,branch,livetime,backgroud, mda;
			if(str_tmp[0] == '#' || str_tmp.IsWhitespace()) continue;
			sscanf(str_tmp.Data(),"%lf%lf%lf%lf",&energy, &branch);
			double effData = fun_eff->Eval(energy/1.E+6);
			eff_list.push_back(effData * 1.E+2);
			energy_list.push_back(energy/1.E+3);
			branch_list.push_back(branch);
			mda = MDA(bg_rate * liveTime,effData,branch/100.0,liveTime);
			MDA_list.push_back(mda);
		//	printf("%8.3lf kev,eff:%8.3lfMDA(Bq):%8.3lf\n",energy/1.E+3,effData,mda);
		}
	}
	in.close();
	 
	TString outFile;
	outFile = dir + outfile;
	//ofstream out;
	//out.open(outFile.Data());
	FILE *out;
	out = fopen(outFile.Data(),"w");
	for(int ii=0; ii<MDA_list.size(); ii++)
		fprintf(out,"%8.3lf%8.3lf%8.3lf%15.3lf\n",energy_list[ii],branch_list[ii],eff_list[ii],MDA_list[ii] );
	//	out<< energy_list[ii]<<"  "<< branch_list[ii]<<"  " <<eff_list[ii]<< "  "<<MDA_list[ii] <<endl;
	//out.close();
	fclose(out);
}

void plotGraph()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("plotGraph.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;
	pathName = dir + "energy_80mm.Txt";
	std::vector<double> xList;
	std::vector<double> yList;
	std::vector<double> yErrorsList;

	ReadFile(pathName,xList,yList,yErrorsList);
	TString lineName("Channal");
	plotLine(lineName,(Int_t)xList.size(),&xList[0],&yList[0],&yErrorsList[0]);

	yErrorsList.clear();
	xList.clear();
	yList.clear();
	pathName = dir + "eff_80mm.Txt";
	//pathName = dir + "eff_ba133_16_20130422.Txt";
	ReadFile(pathName,xList,yList,yErrorsList);
	lineName = "eff" ;
	plotLine(lineName,(Int_t)xList.size(),&xList[0],&yList[0],&yErrorsList[0]);

	yErrorsList.clear();
	xList.clear();
	yList.clear();
	pathName = dir + "FWHM_80mm.Txt";
	ReadFile(pathName,xList,yList,yErrorsList);
	lineName = "FWHM" ;
	plotLine(lineName,(Int_t)xList.size(),&xList[0],&yList[0],&yErrorsList[0]);

	yErrorsList.clear();
	xList.clear();
	yList.clear();

	plotEfficiency();
	return ;
}
