TNtupleD* readData(TString filename)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("plotGraph.C","");
	dir.ReplaceAll("/./","/");

	TString fname;
	fname= dir +filename;
	fname += ".txt";
	TString listTag[8]={
		"PhotonEnergy",
		"ScatteringCoherent",
		"ScatteringIncoher",
		"PhotoElectricAbsorPtion",
		"PairProductionInNuclearField",
		"PairProductionInElectronField",
		"TotalAttenuationWithCoherentScatt",
		"TotalAttenuationWithoutCoherentScatt"
	};
	TString tag;
	for(int i=0;i<8;i++)
	{
		tag+=listTag[i];
		if(i!=7)
			tag+=":";
	}

	TNtupleD *ntuple= new TNtupleD(filename.Data(),Form("%s Photon cross section",filename.Data()),tag.Data());

	ifstream in;
	in.open(fname.Data());
	if(!(in.is_open()))
	{
		printf("open file failed! %s\n",filename.Data());
		return 0;
	}

	TPMERegexp re(".{7,7}( [0-9]+(\\.[0-9]+)(E|e)(\\+|\\-)[0-9]{2,2}){8,8}");
	//TPRegexp re(".{7,7}( [0-9]+(\\.[0-9]+)(E|e)(\\+|\\-)[0-9]{2,2}){8,8}");
//	TPMERegexp re("([0-9]+(\\.[0-9]+)(E|e)(\\+|\\-)[0-9]{2,2} ){7,7}([0-9]+(\\.[0-9]+)(E|e)(\\+|\\-)[0-9]{2,2})");
	//re.Print();
	string str_tmp;

	double d1,d2,d3,d4,d5,d6,d7,d8;
	while(getline(in,str_tmp)) {
		TString str(str_tmp);
		if(re.Match(str))
		{
			str.Replace(0,7," ");
			sscanf(str.Data(),"%lf%lf%lf%lf%lf%lf%lf%lf",&d1,&d2,&d3,&d4,&d5,&d6,&d7,&d8);
			ntuple->Fill(d1,d2,d3,d4,d5,d6,d7,d8);
		}
	}
	in.close();
	return ntuple;
}

void drawMaterial(TString mat)
{
	TNtupleD *ntuple;
	ntuple = readData(mat);
	TString listTag[8]={
		"PhotonEnergy",
		"ScatteringCoherent",
		"ScatteringIncoher",
		"PhotoElectricAbsorPtion",
		"PairProductionInNuclearField",
		"PairProductionInElectronField",
		"TotalAttenuationWithCoherentScatt",
		"TotalAttenuationWithoutCoherentScatt"
	};

	TMultiGraph *mg = new TMultiGraph();
	TGraph *gr;
	for(int i=1;i<8;i++)
	{
		TString tag(Form("%s:%s",listTag[i].Data(),listTag[0].Data()));
		ntuple->Draw(tag.Data(),"","goff");
		gr = new TGraph(ntuple->GetSelectedRows(),ntuple->GetV2(),ntuple->GetV1());
		gr->SetTitle(listTag[i]);
		gr->SetLineColor(1+i);
		gr->SetLineWidth(2);
		gr->GetXaxis()->SetTitle("Photon Energy/MeV");
		gr->GetXaxis()->CenterTitle();
		gr->GetYaxis()->SetTitle("Photon Cross Section/cm2/g");
		gr->GetYaxis()->CenterTitle();
		//gr->SetMarkerStyle(21);
		mg->Add(gr);
	}

	TCanvas* c4 = new TCanvas(Form("%s Photon cross section",mat.Data()), "  ");
	mg->Draw("L a");
	gPad->SetLogx(1);
	gPad->SetLogy(1);
	mg->SetMinimum(1.0e-9);
	mg->SetTitle(Form("%s Attenuation Coefficients",mat.Data()));
	mg->GetXaxis()->SetTitle("Photon Energy/ MeV");
	mg->GetXaxis()->CenterTitle();
	mg->GetYaxis()->SetTitle("Photon Cross Section/ cm^{2}/g");
	mg->GetYaxis()->CenterTitle();
	gPad->Update();	

	c4->BuildLegend();
}

void drawTotalAttenuation()
{
	TString mats[]={
		"Air_N75.5O23.2","H2O","Air1",
		"W95Ni1Fe1","W95Ni7Fe3","Fe",
		"Pb","Al","Cu"
	};
	int num = 9;

	TNtupleD *ntuple;
	TMultiGraph *mg = new TMultiGraph();
	TGraph *gr;

	TString listTag[8]={
		"PhotonEnergy",
		"TotalAttenuationWithCoherentScatt",
	};
	for(int i=0;i<num;i++)
	{
		ntuple = readData(mats[i]);
		TString tag(Form("%s:%s",listTag[1].Data(),listTag[0].Data()));
		ntuple->Draw(tag.Data(),"","goff");
		gr = new TGraph(ntuple->GetSelectedRows(),ntuple->GetV2(),ntuple->GetV1());
		gr->SetTitle(mats[i]);
		gr->SetLineColor(1+i);
		gr->SetLineWidth(2);
		gr->GetXaxis()->SetTitle("Photon Energy/MeV");
		gr->GetXaxis()->CenterTitle();
		gr->GetYaxis()->SetTitle("Photon Cross Section/cm2/g");
		gr->GetYaxis()->CenterTitle();
		//gr->SetMarkerStyle(21);
		mg->Add(gr);
	}

	TCanvas* c3 = new TCanvas(" Photon cross section", "  ");
	mg->Draw("L a");
	gPad->SetLogx(1);
	gPad->SetLogy(1);
	mg->SetMinimum(1.0e-2);
	mg->GetXaxis()->SetTitle("Photon Energy/ MeV");
	mg->GetXaxis()->CenterTitle();
	mg->GetYaxis()->SetTitle("Photon Cross Section/ cm^{2}/g");
	mg->GetYaxis()->CenterTitle();
	gPad->Update();	

	//mg->GetXaxis()->SetRangeUser(0.01,15.);
	//mg->GetXaxis()->SetLimits(0.01,15.);
	c3->BuildLegend();
}

void plotGraph(TString fname="U")
{
//		drawTotalAttenuation();

		//TString mat("U");
		drawMaterial( fname);

}
