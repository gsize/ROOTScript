
int smooth1(Float_t *s,Float_t *d,Int_t num)
{
    if(num<3)
    {
        for(int i=2; i<num-2; i++)
        {
            //d[i]=(-3*(s[i+2]+s[i-2]) + 12*(s[i+1]+s[i-1])+17*s[i])/35;
            d[i]=((s[i+2]+s[i-2]) + 4*(s[i+1]+s[i-1])+6*s[i])/16;
        }
        return 0;
    }
    else
        return -1;
}

void spectra_deconve() {
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
   dir.ReplaceAll("spectra_deconve.C","");
   dir.ReplaceAll("/./","/");
TString path_file;
path_file = dir + "a/Cs137.spe";
   TSpectrum pfinder;
   TTree *T =new TTree("source","data from spectra");
   TTree *T1 =new TTree("source1","data from spectra");
   Long_t nlines= T->ReadFile(path_file.Data(),"x/F");
path_file = dir + "a/xiangying.spe";
   Long_t nlines1= T1->ReadFile(path_file.Data(),"x/F");
   printf("found %ld::%ld points\n",nlines,nlines1);
   
   Int_t nentries=T->GetEntries();
	printf("len=%d\n",nentries);
	Float_t temp,temp1;
	T->SetBranchAddress("x",&temp);
	T1->SetBranchAddress("x",&temp1);
	Int_t size = nentries;
   //TFile *f = new TFile("testhists.root");
   TH1F *hins = new TH1F("source","source",size,0,size);
   TH1F *hinr = new TH1F("resp","resp",size,0,size);
   float *source= new float [size];
   float *resp= new float [size];

   for (Int_t i=0;i<size;i++) {
   	  T->GetEntry(i);
   	  T1->GetEntry(i);
   	  source[i]=temp;
   	  resp[i]=temp1;
      hins->SetBinContent(i + 1,temp);
      hinr->SetBinContent(i+1,temp1);
   }
   
   float *source_t= new float [size];
   float *resp_t= new float [size];
   smooth1(source,source_t,size);
   smooth1(resp,resp_t,size);

   Int_t averWindow=7;
   pfinder.SmoothMarkov(source,size,averWindow);
   pfinder.SmoothMarkov(resp,size,averWindow);
   TH1F *hins_smooth = new TH1F("source_smooth","source_smooth",size,0,size);
   TH1F *hinr_smooth = new TH1F("resp_smooth","resp_smooth",size,0,size);
   for (Int_t i=0;i<size;i++) {
      hins_smooth ->SetBinContent(i + 1,source[i]);
      hinr_smooth ->SetBinContent(i + 1,resp[i]);
      source[i]=source_t[i];
      resp[i]=resp_t[i];
      /*if(source_t[i]<0.00001)
      	source[i]=0.000001;
      if( resp_t[i]<0.00001)
      	resp[i]=0.00001;
   */
   }
   /*
   Int_t index_max=TMath::LocMax(size,source);
   Float_t max_s=source[index_max];
   index_max=TMath::LocMax(size,resp);
   Float_t max_r=resp[index_max];
   for (Int_t i=0;i<size;i++) {
   	  source[i]/=max_s;
   	  resp[i]/=max_r;
   }
*/

   pfinder.Deconvolution(source,resp,size,100,10,5);//function which deconvolves source spectrum according to response spectrum

   //      Deconvolution(float *source, const float *response,Int_t ssize, Int_t numberIterations,Int_t numberRepetitions, Double_t boost ); 

	//Int_t index_max=TMath::LocMax(size,source);
   //source[index_max] = 0.;
   TH1F *hout = new TH1F("hdec1_out","hdec1_out",size,0,size);
   for (Int_t i=0;i<size;i++) {
      hout->Fill(i+0.5,source[i]);
      //hout->SetBinContent(i+1,source[i]);
   }
   gStyle->SetFrameFillColor(18);
   TCanvas *c1 = new TCanvas("c4","Background estimator",600,800);
   c1->SetFillColor(38);
   c1->Divide(1,2);
   c1->cd(1);
   // draw original spectrum and response signal
   hinr->SetLineColor(kRed);
   hins_smooth->Draw();
   hinr->Draw("same");
   hins->Draw("same");
   hinr_smooth->Draw("same");
   
   // draw deconvolution
   c1->cd(2);
   hout->Draw();
   c1->cd();
   delete [] resp;
   delete [] source;

}
