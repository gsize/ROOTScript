void deconve() {
/////////////////////////////////////////////////////////////////////////////
//
//  Example of One Dimensional deconvolution  function
//  ==================================================
//    from original example by Miroslav Morhac
//
//	The function decomposes the input spectrum according to the response 
// spectrum (usually one peak) stored in the response histogram.
// After deconvolution the result is stored in hdec1_out histogram.
// The influence of the parameter number_of_iterations is explained in the 
// publication concerning deconvolution. The method is stable. 
// It does not oscillate. On the other hand 
// it converges slowly so that the number of iterations should be >1000.
/////////////////////////////////////////////////////////////////////////////
TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("deconve.C","");
   dir.ReplaceAll("/./","/");
TString path_file;
path_file = dir + "testhists.root";
   TSpectrum pfinder;
   TFile *f = new TFile(path_file);
   TH1F *hins = (TH1F*)f->Get("konv");
   TH1F *hinr = (TH1F*)f->Get("beem"); 
   Int_t size = hins->GetXaxis()->GetNbins();
   float *source, *resp;
   source = new float [size];
   resp   = new float [size];
   Int_t i;
   for (i=0;i<size;i++) {
      source[i] = hins->GetBinContent(i+1);
      resp[i]   = hinr->GetBinContent(i+1);
   }

   pfinder.Deconvolution(source,resp,size,10,10,5);//function which deconvolves source spectrum according to response spectrum

   //      Deconvolution(float *source, const float *response,Int_t ssize, Int_t numberIterations,Int_t numberRepetitions, Double_t boost ); 

   TH1F *hout = new TH1F("hdec1_out","hdec1_out",size,0,size);
   for (i=0;i<size;i++) {
      hout->Fill(i+0.5,source[i]);
      //hout->SetBinContent(i+1,source[i]);
   }
   gStyle->SetFrameFillColor(18);
   TCanvas *c1 = new TCanvas("c1","Background estimator",600,800);
   c1->SetFillColor(38);
   c1->Divide(1,2);
   c1->cd(1);
   // draw original spectrum and response signal
   hinr->SetLineColor(4);
   hinr->Draw();
   hins->Draw("same");
   
   // draw deconvolution
   c1->cd(2);
   hout->Draw();
   c1->cd();
   delete [] resp;
   delete [] source;
}
