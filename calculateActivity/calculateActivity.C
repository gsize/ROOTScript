void xiaosuanyouxian(void){
	
   TGeoManager *geom = new TGeoManager("","");
   TGeoElementTable *table = gGeoManager->GetElementTable();
   TGeoElement *O = table->GetElement(8);
   TGeoElement *N = table->GetElement(7);
   TGeoElement *U = table->GetElement(92);
   TGeoElement *H = table->GetElement(1);
   TGeoElementRN *U235 = table->GetElementRN(235,92);
   TGeoMaterial *decaymat;

   //Radioactive mixture
   TGeoMixture *mix = new TGeoMixture("mix", 4, 2.807);
   mix->AddElement(O, 14);
   mix->AddElement(N, 2);
   mix->AddElement(U, 1);
   mix->AddElement(H, 12);
   
   mix->Print();
   
   for(int i=0; i<mix->GetNelements(); i++)
   		if( mix->GetElement(i)->Z() == 92) {
   			Double_t radio_active =U235->GetSpecificActivity() * U235->NatAbun() * (mix->GetWmixt())[i];
   			cout<<"U235 SpecificActivity ="<<U235->GetSpecificActivity() <<endl;
   			cout<<"U235 NatAbun =" <<U235->NatAbun() <<endl;
   			cout<<"U235 weight =" <<(mix->GetWmixt())[i] <<endl;
    		cout<<"radio_active="<< radio_active <<endl;
   		}

   Double_t time = 2.22e+16; // seconds
   decaymat = mix->DecayMaterial(time);
   decaymat->Print();
   
   TObjArray *vect = new TObjArray();
   //TCanvas *c1 = new TCanvas("c1","C14 decay", 800,600);
   //c1->SetGrid();
   mix->FillMaterialEvolution(vect);
   //DrawPopulation(vect, c1, 0, time ,kTRUE);

}

void DrawPopulation(TObjArray *vect, TCanvas *can, Double_t tmin=0., 
   Double_t tmax=0., Bool_t logx=kFALSE)
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
