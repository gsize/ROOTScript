
void DrawBatemanSolAct(TGeoBatemanSol *sol, Option_t *option);

void DrawPopulation(TObjArray *vect, TCanvas *can, Double_t tmin=0., 
		Double_t tmax=0., Bool_t logx=kFALSE);
		
void DrawPopulationAct(TObjArray *vect, TCanvas *can, Double_t tmin=0., 
		Double_t tmax=0., Bool_t logx=kFALSE);


int elemNR2ENDFCode(TString elementName)
{
	int ENDFCode;
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

void calculateActivity(void){

	TGeoManager *geom = new TGeoManager("","");
	TGeoElementTable *table = gGeoManager->GetElementTable();
	TGeoElement *O = table->FindElement("O");
	TGeoElement *N = table->FindElement("N");
	TGeoElement *U = table->FindElement("U");
	TGeoElement *H = table->FindElement("H");
	TGeoElementRN *UIso = table->GetElementRN(231,91);
	TGeoMaterial *decaymat;

	//Radioactive mixture
	TGeoMixture *mix = new TGeoMixture("mix", 4, 2.807);
	mix->AddElement(O, 14);
	mix->AddElement(N, 2);
	mix->AddElement(UIso, 1);
	mix->AddElement(H, 12);
	//mix->Print();

	for(int i=0; i<mix->GetNelements(); i++)
		if( mix->GetElement(i)->Z() == UIso->AtomicNo()) {
			Double_t radio_active =UIso->GetSpecificActivity() * UIso->NatAbun() * (mix->GetWmixt())[i];
			cout<<UIso->GetName() <<" SpecificActivity ="<<UIso->GetSpecificActivity() <<endl;
			cout<<UIso->GetName() <<" NatAbun =" <<UIso->NatAbun() <<endl;
			cout<<UIso->GetName() <<" weight =" <<(mix->GetWmixt())[i] <<endl;
			cout<<"radio_active="<< radio_active <<endl;
		}

	Double_t time =0.;
	time = 3600.*24*365*150;//3600.*24*365*56;// seconds
	
	// decaymat = mix->DecayMaterial(time);
	// decaymat->Print();

	TObjArray *vect = new TObjArray();
	/*
	   TCanvas *c1 = new TCanvas("c1","C14 decay", 800,600);
	   c1->SetGrid();
	   mix->FillMaterialEvolution(vect);
	   DrawPopulation(vect, c1, 0, time ,0);
	   */
	/*
	   vect->Clear();
	   TCanvas *c2 = new TCanvas("c1","Material decay", 1000,800);
	   TGeoMaterial *mat = new TGeoMaterial(UIso->GetName(), UIso, 1.3);
	   mat->FillMaterialEvolution(vect);
	   DrawPopulation(vect, c2, 0, time,0);
	   */

	vect->Clear();
	TString element_name("Pa-231");
	//TString element_name("Ac-227");
	double factor = 1.3e+20;
    int ENDFCode;
	ENDFCode = elemNR2ENDFCode(element_name);
	TGeoElementRN *nuclear = table->GetElementRN(ENDFCode );
	nuclear->FillPopulation(vect, 1.0e-6,factor);
	printf("%s specificActivity = %.3E Bq/g\n",nuclear->GetName(),nuclear->GetSpecificActivity());
	//TCanvas *c3 = new TCanvas(Form(" Decay",nuclear->GetName()),Form(" Decay",nuclear->GetName()), 1000,800);
	//DrawPopulation(vect, c3, 0, time,0);
	
	TCanvas *c4 = new TCanvas(Form(" Activity",nuclear->GetName()),Form(" Activity",nuclear->GetName()), 1000,800);
	DrawPopulationAct(vect, c4, 0, time,0);
}

void DrawPopulation(TObjArray *vect, TCanvas *can, Double_t tmin, 
		Double_t tmax, Bool_t logx)
{
	Int_t n = vect->GetEntriesFast();
	TGeoElementRN *elem;
	TGeoBatemanSol *sol;
	can->SetLogy();

	if (logx) can->SetLogx();

	for (Int_t i=0; i<n; i++) {
		TGeoElement *el = (TGeoElement*)vect->At(i);
		if (!el->IsRadioNuclide()) continue;
		TGeoElementRN *elem = (TGeoElementRN *)el;
		TGeoBatemanSol *sol = elem->Ratio();

		double Ni= sol->Concentration(tmax);
		if(elem->HalfLife()>0.)
			printf("%s, Ni= %E, Mi= %E, Ai= %E\n",sol->GetElement()->GetName(),Ni,elem->A()*Ni/TMath::Na(),Ni*TMath::Log(2.)/elem->HalfLife());
		else
			printf("%s, Ni= %E, Mi= %E, No RadioNuclide\n",sol->GetElement()->GetName(),Ni,elem->A()*Ni/TMath::Na());

		if (sol && elem->HalfLife()>0. ) {
			sol->SetLineColor(1+(i%9));
			sol->SetLineWidth(2);
			if (tmax>0.) sol->SetRange(tmin,tmax);
			if (i==0) {
				sol->Draw();
				TF1 *func = (TF1*)can->FindObject(
						Form("conc%s",sol->GetElement()->GetName()));
				if (func) {
					func->SetTitle(Form("Concentration of %s derived elements;time[year];Ni/N0",sol->GetElement()->GetName()));
					func->GetXaxis()->SetTimeDisplay(1);
					//func->GetXaxis()->SetTimeOffset(0);
					func->GetXaxis()->SetTimeFormat("%y%F2000-01-01 00:00:01");
				} 
			}
			else 
			{
			     sol->Draw("SAME");
			}
			
			TString nameRN;
			double t;
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

void DrawPopulationAct(TObjArray *vect, TCanvas *can, Double_t tmin, 
		Double_t tmax, Bool_t logx)
{
Int_t n = vect->GetEntriesFast();
	TGeoElementRN *elem;
	TGeoBatemanSol *sol;
	can->SetLogy();

	if (logx) can->SetLogx();

	for (Int_t i=0; i<n; i++) {
		TGeoElement *el = (TGeoElement*)vect->At(i);
		if (!el->IsRadioNuclide()) continue;
		TGeoElementRN *elem = (TGeoElementRN *)el;
		TGeoBatemanSol *sol = elem->Ratio();

		double Ni= sol->Concentration(tmax);

		if (sol && elem->HalfLife()>0.) {
			sol->SetLineColor(1+(i%9));
			sol->SetLineWidth(2);
			if (tmax>0.) sol->SetRange(tmin,tmax);
			if (i==0) {
			DrawBatemanSolAct(sol,"");
				TF1 *func = (TF1*)can->FindObject(
						Form("Act%s",sol->GetElement()->GetName()));
				if (func) {
					func->SetTitle(Form("Activity of %s derived elements;time[s];Activity",sol->GetElement()->GetName()));
					//func->GetXaxis()->SetTimeDisplay(1);
					func->GetXaxis()->SetTimeOffset(0);
					func->GetXaxis()->SetTimeFormat("%y%F2000-01-01 00:00:01");
				} 
			}
			else 
			{
			     DrawBatemanSolAct(sol,"same");
			}
			
			TString nameRN;
			double t;
			if(i%2==0)
				t=0.6*(tmin+tmax);
			else
				t=0.3*(tmin+tmax);
			nameRN.Form("%s", elem->GetName());
			TLatex *tex = new TLatex(t,TMath::Log(2)/(sol->GetElement()->HalfLife())*sol->Concentration(t),nameRN);
			tex->SetTextSize(0.0388601);
			tex->SetTextColor(sol->GetLineColor());
			//tex->Draw();
		}
	}
}

void DrawBatemanSolAct(TGeoBatemanSol *sol, Option_t *option)
{
// Draw the time evolution of a radionuclide.
   Int_t ncoeff = sol->GetNcoeff();
   if (!ncoeff) return;
   Double_t tlo=0., thi=0.;
   Double_t cn=0., lambda=0.;
   Int_t i;
   sol->GetRange(tlo, thi);
   Bool_t autorange = (thi==0.)?kTRUE:kFALSE;
   
   // Try to find the optimum range in time.
   if (autorange) tlo = 0.;
   sol->GetCoeff(0, cn, lambda);
   Double_t lambdamin = lambda;
   TString formula = "(";
   for (i=0; i<ncoeff; i++) {
      sol->GetCoeff(i, cn, lambda);
      formula += TString::Format("%g*exp(-%g*x)",cn, lambda);
      if (i < ncoeff-1) formula += "+";
      if (lambda < lambdamin &&
          lambda > 0.) lambdamin = lambda;
   }
   if (autorange) thi = 10./lambdamin;
   formula += TString::Format(")*%g",TMath::Log(2)/(sol->GetElement()->HalfLife()));
   formula += ";time[s]";
   formula += TString::Format(";Activity_of_%s",sol->GetElement()->GetName());
   // Create a function
   TF1 *func = new TF1(TString::Format("Act%s",sol->GetElement()->GetName()), formula.Data(), tlo,thi);
   func->SetMinimum(1.e-3);
   func->SetMaximum(1.25*TMath::Max(sol->Concentration(tlo), sol->Concentration(thi)));
   func->SetLineColor(sol->GetLineColor());
   func->SetLineStyle(sol->GetLineStyle());
   func->SetLineWidth(sol->GetLineWidth());
   func->SetMarkerColor(sol->GetMarkerColor());
   func->SetMarkerStyle(sol->GetMarkerStyle());
   func->SetMarkerSize(sol->GetMarkerSize());
   func->Draw(option);
}   

