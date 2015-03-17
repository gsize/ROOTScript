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
	TString tag(Form("%s:%s:%s:%s:%s:%s:%s:%s",listTag[0].Data(),listTag[1].Data(),listTag[2].Data(),listTag[3].Data(),listTag[4].Data(),listTag[5].Data(),listTag[6].Data(),listTag[7].Data()));
	TNtupleD *ntuple= new TNtupleD(filename.Data(),Form("%s Photon cross section",filename.Data()),tag.Data());
	int nlines =ntuple->ReadFile(fname.Data(),tag.Data());
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
		gr->SetLineWidth(2.5);
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
	int num = 3;

	TNtupleD *ntuple;
	TMultiGraph *mg = new TMultiGraph();
	TGraph *gr;

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
	for(int i=0;i<num;i++)
	{
		ntuple = readData(mats[i]);
		TString tag(Form("%s:%s",listTag[6].Data(),listTag[0].Data()));
		ntuple->Draw(tag.Data(),"","goff");
		gr = new TGraph(ntuple->GetSelectedRows(),ntuple->GetV2(),ntuple->GetV1());
		gr->SetTitle(mats[i]);
		gr->SetLineColor(1+i);
		gr->SetLineWidth(2.5);
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

	c3->BuildLegend();
}

void plotGraph()
{
	drawTotalAttenuation();

//	TString mat("Fe");
//	drawMaterial( mat);

}
