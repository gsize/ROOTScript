#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"

//void FFT()
void FFTconvolution()
{

Int_t n2=8000;

//int setHistoRange=4*TMath::Pi();
//int setHistoRange=4;
Int_t setHistoRange=8;
//Int_t setHistoRange=10;





   TCanvas *myc2 = new TCanvas("myc2", "Fast Fourier Transform", 800, 600);
   myc2->cd();
//   TH1 *hr2 = 0;
////   hr2 = TH1::TransformHisto(fft_own2, hr, "RE");
////   hr2->Draw();
//   fsin2->Draw();
//   fcos->Draw("SAME");


//• The predefined functions:
//– "gaus" = p0*exp(-0.5*pow((x-p1)/p2),2)
//– "expo" = exp(p0+p1*x)
//– "polN" = p0 + p1*x + p2*pow(x,2) + p3*...
//– "landau" (guess the formula!)


   //A function to sample
//   TF1 *fsin2 = new TF1("fsin2", "sin(x)+sin(2*x)+sin(0.5*x)+1", 0, setHistoRange);

//   TF1 *fsin2 = new TF1("fsin2","TMath::Landau(x,[0],[1],0)" , 0, setHistoRange);
//   fsin2->SetParameters(0.2,1.3); 

   TF1 *fsin2 = new TF1("fsin2", "gaus" , 0, setHistoRange);
//– "gaus" = p0*exp(-0.5*pow((x-p1)/p2),2)
//   fsin2->SetParameters(0.5,4,5); 
//   fsin2->SetParameters(0.5,4,1); 
   fsin2->SetParameters(0.5,4,0.3); 
   fsin2->GetXaxis()->SetLimits(0.0,8.0);
   fsin2->GetYaxis()->SetLimits(0.0,1.1);
   fsin2->GetXaxis()->SetRangeUser(0.0,8.0);
   fsin2->GetYaxis()->SetRangeUser(0.0,1.1);
   fsin2->Draw();
   
//   Int_t n2=2500;
   TH1D *hsin2 = new TH1D("hsin2", "hsin2", n2+1, 0, setHistoRange);
   Double_t x2;
   
   //Fill the histogram with function values
   for (Int_t i=0; i<=n2; i++){
      x2 = (Double_t(i)/n2)*(setHistoRange);
      hsin2->SetBinContent(i+1, fsin2->Eval(x2));
   }
//   hsin2->Draw("SAME");
//   fsin2->GetXaxis()->SetLabelSize(0.05);
//   fsin2->GetYaxis()->SetLabelSize(0.05);
   
//   myc2->cd();
//   //Compute the transform and look at the magnitude of the output
   TH1 *hm2 =0;
   TVirtualFFT::SetTransform(0);
   hm2 = hsin2->FFT(hm2, "MAG2");
//   hm2->SetTitle("Magnitude of the 1st transform");
//   hm2->Draw("SAME");
//   //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function 
//   //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
//   hm2->SetStats(kFALSE);
//   hm2->GetXaxis()->SetLabelSize(0.05);
//   hm2->GetYaxis()->SetLabelSize(0.05);

//   myc2->cd();
//   //Look at the phase of the output
//   TH1 *hp2 = 0;
//   hp2 = hsin2->FFT(hp2, "PH");
//   hp2->SetTitle("Phase of the 1st transform");
//   hp2->Draw();
//   hp2->SetStats(kFALSE);
//   hp2->GetXaxis()->SetLabelSize(0.05);
//   hp2->GetYaxis()->SetLabelSize(0.05);
   
   //Look at the DC component and the Nyquist harmonic:
//   TVirtualFFT *fft2 = TVirtualFFT::GetCurrentTransform();
   Double_t re2, im2;
   //That's the way to get the current transform object:
   TVirtualFFT *fft2 = TVirtualFFT::GetCurrentTransform();
   myc2->cd();
//   //Use the following method to get just one point of the output
   fft2->GetPointComplex(0, re2, im2);
//   printf("1st transform: DC component: %f\n", re2);
   fft2->GetPointComplex(n2/2+1, re2, im2);
//   printf("1st transform: Nyquist harmonic: %f\n", re2);

   //Use the following method to get the full output:
   Double_t *re_full2 = new Double_t[n2];
   Double_t *im_full2 = new Double_t[n2];
   fft2->GetPointsComplex(re_full2,im_full2);
  

//   //Now let's make a backward transform:
//   TVirtualFFT *fft_back2 = TVirtualFFT::FFT(1, &n2, "C2R M K");
//   fft_back2->SetPointsComplex(re_full2,im_full2);
//   fft_back2->Transform();
//   TH1 *hb2 = 0;
//   //Let's look at the output
//   hb2 = TH1::TransformHisto(fft_back2,hb2,"Re");
//   hb2->SetTitle("The backward transform result");
//   hb2->Draw("SAME");
//   //NOTE: here you get at the x-axes number of bins and not real values
//   //(in this case 25 bins has to be rescaled to a range between 0 and 4*Pi; 
//   //also here the y-axes has to be rescaled (factor 1/bins)
//   hb2->SetStats(kFALSE);
//   hb2->GetXaxis()->SetLabelSize(0.05);
//   hb2->GetYaxis()->SetLabelSize(0.05);
//   delete fft_back2;
////   fft_back2=0;






////   TF1 *fcos = new TF1("fcos", "cos(x)+cos(0.5*x)+cos(2*x)+1", 0, setHistoRange);
//   TF1 *fgaus = new TF1("fgaus", "gaus", 0, setHistoRange);
//   for (Int_t i=0; i<=n2; i++){
//      x2 = (Double_t(i)/n2)*(setHistoRange);
//      in2[i] = fgaus->Eval(x2);
//   }
//   fft_own2->SetPoints(in2);
//   fft_own2->Transform();
//   fft_own2->GetPointComplex(0, re_2, im_2);
//   printf("2nd transform: DC component: %f\n", re_2);
//   fft_own2->GetPointComplex(n2/2+1, re_2, im_2);
//   printf("2nd transform: Nyquist harmonic: %f\n", re_2);
   //A function to sample
//   TF1 *fgaus = new TF1("fgaus", "gaus", 0, setHistoRange);

//   TF1 *fcos = new TF1("fcos", "cos(x)", 0, setHistoRange);

   TF1 *fcos = new TF1("fcos", "gaus" , 0, setHistoRange);
//– "gaus" = p0*exp(-0.5*pow((x-p1)/p2),2)
//   fcos->SetParameters(1.0,4,5); 
//   fcos->SetParameters(1.0,4,1); 
   fcos->SetParameters(1.0,4,0.3); 
//   fcos->GetXaxis()->SetLimits(0.0,8.0);
//   fcos->GetYaxis()->SetLimits(0.0,4.0);
   fcos->GetXaxis()->SetRangeUser(0.0,8.0);
   fcos->GetYaxis()->SetRangeUser(0.0,4.0);
   fcos->Draw("SAME");
//   TF1 *fcos = new TF1("fcos", "gaus" , 0, setHistoRange);
//   fcos->Draw("SAME");

//   TF1 *fgaus = new TF1("fgaus", "sin(x)+sin(2*x)+sin(0.5*x)+1", 0, setHistoRange);
//   fgaus->Draw("SAME");
   
//   Int_t n2=2500;
   TH1D *hgaus = new TH1D("hgaus", "hgaus", n2+1, 0, setHistoRange);
   Double_t x3;
   
   //Fill the histogram with function values
   for (Int_t i=0; i<=n2; i++){
      x3 = (Double_t(i)/n2)*(setHistoRange);
//      hgaus->SetBinContent(i+1, fgaus->Eval(x3));
      hgaus->SetBinContent(i+1, fcos->Eval(x3));
   }
//   hgaus->Draw("SAME");
//   fcos->GetXaxis()->SetLabelSize(0.05);
//   fcos->GetYaxis()->SetLabelSize(0.05);
//   fgaus->GetXaxis()->SetLabelSize(0.05);
//   fgaus->GetYaxis()->SetLabelSize(0.05);
   
////   myc2->cd();
   //Compute the transform and look at the magnitude of the output
   TH1 *hm3 =0;
   TVirtualFFT::SetTransform(0);
   hm3 = hgaus->FFT(hm3, "MAG3");
//   hm3->SetTitle("Magnitude of the 1st transform");
//   hm3->Draw("SAME");
//   //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function 
//   //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
//   hm3->SetStats(kFALSE);
//   hm3->GetXaxis()->SetLabelSize(0.05);
//   hm3->GetYaxis()->SetLabelSize(0.05);

//   myc2->cd();
//   //Look at the phase of the output
//   TH1 *hp3 = 0;
//   hp3 = hsin2->FFT(hp3, "PH");
//   hp3->SetTitle("Phase of the 1st transform");
//   hp3->Draw();
//   hp3->SetStats(kFALSE);
//   hp3->GetXaxis()->SetLabelSize(0.05);
//   hp3->GetYaxis()->SetLabelSize(0.05);
   
   //Look at the DC component and the Nyquist harmonic:
//   TVirtualFFT *fft2 = TVirtualFFT::GetCurrentTransform();
   //That's the way to get the current transform object:
   TVirtualFFT *fft3 = TVirtualFFT::GetCurrentTransform();
//   myc2->cd();
////   //Use the following method to get just one point of the output
//   Double_t re3, im3;
//   fft3->GetPointComplex(0, re3, im3);
////   printf("1st transform: DC component: %f\n", re3);
//   fft3->GetPointComplex(n2/2+1, re3, im3);
////   printf("1st transform: Nyquist harmonic: %f\n", re3);

   //Use the following method to get the full output:
   Double_t *re_full3 = new Double_t[n2];
   Double_t *im_full3 = new Double_t[n2];
   fft3->GetPointsComplex(re_full3,im_full3);
  

   //Now let's make a backward transform:
   TVirtualFFT *fft_back3 = TVirtualFFT::FFT(1, &n2, "C2R M K");
   fft_back3->SetPointsComplex(re_full3,im_full3);
   fft_back3->Transform();
   TH1 *hb3 = 0;
   //Let's look at the output
   hb3 = TH1::TransformHisto(fft_back3,hb3,"Re");
//   hb3->SetTitle("The backward transform result");
//   hb3->Draw("SAME");
//   //NOTE: here you get at the x-axes number of bins and not real values
//   //(in this case 25 bins has to be rescaled to a range between 0 and 4*Pi; 
//   //also here the y-axes has to be rescaled (factor 1/bins)
//   hb3->SetStats(kFALSE);
//   hb3->GetXaxis()->SetLabelSize(0.05);
//   hb3->GetYaxis()->SetLabelSize(0.05);
//   delete fft_back3;
////   fft_back3=0;


   Double_t *xHistoRange = new Double_t[n2];
   for (Int_t i=0; i<=n2; i++){
      xHistoRange[i] = (Double_t(i)/n2)*(setHistoRange);
//      hsin2->SetBinContent(i+1, fsin2->Eval(x2));
   }
   Double_t *re_full_conv = new Double_t[n2];
   Double_t *im_full_conv = new Double_t[n2];
//   //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function 
//   //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
//   fft3->GetPointsComplex(re_full3,im_full3);

   for (Int_t i=0; i<=n2; i++){
	re_full_conv[i]=re_full2[i]/n2*re_full3[i]/n2;//sqrt(n2);
	im_full_conv[i]=-im_full2[i]/n2*im_full3[i]/n2;//sqrt(n2);
//	re_full_conv[i]=re_full2[i]/sqrt(n2)*re_full3[i]/sqrt(n2);//sqrt(n2);
//	im_full_conv[i]=-im_full2[i]/sqrt(n2)*im_full3[i]/sqrt(n2);//sqrt(n2);
//	im_full_conv[i]=-im_full2[i]*im_full3[i];//sqrt(n2);
//        x3 = (Double_t(i)/n2)*(setHistoRange);
//        hgaus->SetBinContent(i+1, fgaus->Eval(x3));
   }

////  int roundN2 = round(n2/2); 
//  int roundN2 = ceil(n2/2);
//  int roundN2 = (n2/2);
//  cout<<n2<<endl; 
//  cout<<roundN2<<endl; 
////      for (Int_t i=0; i<=roundN2; i++){
////	re_full_conv[i]=re_full2[i+roundN2+1]/n2*re_full3[i+roundN2+1]/n2;//sqrt(n2);
////	im_full_conv[i]=-im_full2[i+roundN2+1]/n2*im_full3[i+roundN2+1]/n2;//sqrt(n2);
////      }	
////      for (Int_t i=0; i<=roundN2; i++){
////	re_full_conv[i+roundN2+1]=re_full2[i]/n2*re_full3[i]/n2;//sqrt(n2);
////	im_full_conv[i+roundN2+1]=-im_full2[i]/n2*im_full3[i]/n2;//sqrt(n2);
////      }	
//
//      for (Int_t i=0; i<=roundN2; i++){
//	re_full_conv[i]=re_full2[i+roundN2]/n2*re_full3[i+roundN2]/n2;//sqrt(n2);
//	im_full_conv[i]=-im_full2[i+roundN2]/n2*im_full3[i+roundN2]/n2;//sqrt(n2);
//      }	
//      for (Int_t i=0; i<=roundN2; i++){
//	re_full_conv[i+roundN2]=re_full2[i]/n2*re_full3[i]/n2;//sqrt(n2);
//	im_full_conv[i+roundN2]=-im_full2[i]/n2*im_full3[i]/n2;//sqrt(n2);
//      }	


   //Now let's make a backward transform:
   TVirtualFFT *fft_back_conv = TVirtualFFT::FFT(1, &n2, "C2R M K");
//   TVirtualFFT *fft_back_conv = TVirtualFFT::FFT(1, &setHistoRange, "C2R M K");
   fft_back_conv->SetPointsComplex(re_full_conv,im_full_conv);
   fft_back_conv->Transform();
   TH1 *hb_conv = 0;
   //Let's look at the output
//   hb_conv = TH1::TransformHisto(fft_back_conv,hb_conv,"Re_conv");
   hb_conv = TH1::TransformHisto(fft_back_conv,hb_conv,"REconv");

//   hb_conv->Rebin(n2/setHistoRange);
//   hb_conv->Sumw2();
//   hb_conv->Scale(1.0 / hb_conv->Integral(),"width");  //see doc of TH1::Scale
//   hb_conv->SetMaximum(setHistoRange);

//   hb_conv->SetBit(TH1::kCanRebin);
////   hb_conv->RebinAxis(setHistoRange, hb_conv->GetXaxis());
//   hb_conv->RebinAxis(n2*2, hb_conv->GetXaxis());
////   hb_conv->RebinAxis(9000, fcos->GetXaxis());

//   hb_conv->SetAxisRange(0, setHistoRange);

//////   hb_conv = TH1::TransformHisto(fft_back_conv,hb_conv,"IM");
//////   Double_t *re_full_conv = new Double_t[n2];
//      for (Int_t i=0; i<roundN2; i++){
//	hb_conv[i]=hb_conv[i+roundN2]/n2*hb_conv[i+roundN2]/n2;//sqrt(n2);
//      }	
//      for (Int_t i=0; i<roundN2; i++){
//	hb_conv[i+roundN2]=[i]/n2*hb_conv[i]/n2;//sqrt(n2);
//      }	  

   hb_conv->SetTitle("The backward transform result");
   //NOTE: here you get at the x-axes number of bins and not real values
   //(in this case 25 bins has to be rescaled to a range between 0 and 4*Pi; 
   //also here the y-axes has to be rescaled (factor 1/bins)
   hb_conv->SetLineColor(kGreen);
   hb_conv->SetStats(kFALSE);
   hb_conv->GetXaxis()->SetLabelSize(0.05);
   hb_conv->GetYaxis()->SetLabelSize(0.05);
//   hb_conv->GetXaxis()->SetRange(50,80);
//   hb_conv->Draw();

//   hb_conv->Draw("SAME");
//   hb_conv->Draw("");

//   hb_conv->GetXaxis()->SetLimits(0.0,8.0);
//   hb_conv->GetYaxis()->SetLimits(0.0,4.0);
//   hb_conv->GetXaxis()->SetRangeUser(0.0,8.0);
//   hb_conv->GetYaxis()->SetRangeUser(0.0,4.0);
//   hb_conv->SetMinimum(-7.0);
//   hb_conv->SetMaximum(2000.0);
//   hb_conv->GetXaxis()->Set(n2,-10.0,20.0);
////   hb_conv->GetXaxis()->SetRange(-7,20);
////   hb_conv->GetYaxis()->SetRange(-5,1000);
//   hb_conv->Draw("SAME");


//   fcos->GetXaxis()->SetLimits(-7.0,20.0);
//   fcos->GetYaxis()->SetLimits(-5.0,1000.0);
////   fcos->GetXaxis()->SetRangeUser(-7.0,20.0);
////   fcos->GetYaxis()->SetRangeUser(-5.0,1000.0);
//////   fcos->GetXaxis()->SetRange(-7.0,20.0);
//////   fcos->GetYaxis()->SetRange(-5.0,1000.0);
//   fcos->Draw("SAME");
//   myc2->Modified();                 
//   myc2->Update();                 


   TCanvas *myc3 = new TCanvas("myc3", "Convolution", 800, 600);
   myc3->cd();
//   hb_conv->GetXaxis()->SetRangeUser(0.0,setHistoRange);
////   hb_conv->GetYaxis()->SetRangeUser(-2.0,20.0);
//   hb_conv->GetYaxis()->SetRangeUser(-0.1,3.0);
////   hb_conv->GetXaxis()->SetLimits(-2.0,20.0);
//   hb_conv->GetYaxis()->SetLimits(-0.1,3.0);
   hb_conv->Draw("");
//   fsin2->Draw("SAME");
//   fcos->Draw("SAME");

   delete fft2;
//   delete fft_own2;
//   delete [] in2;
   delete [] re_full2;
   delete [] im_full2;
//   delete fft_back2;

   delete fft3;
//   delete fft_own2;
   delete fft_back3;
//   delete fft_back_conv;
   delete [] re_full3;
   delete [] im_full3;
   delete [] re_full_conv;
   delete [] im_full_conv;




////********* Data array - same transform ********//
//
//   //Allocate an array big enough to hold the transform output
//   //Transform output in 1d contains, for a transform of size N, 
//   //N/2+1 complex numbers, i.e. 2*(N/2+1) real numbers
//   //our transform is of size n+1, because the histogram has n+1 bins
//
//   Double_t *in2 = new Double_t[2*((n2+1)/2+1)];
//   Double_t re_2,im_2;
//   for (Int_t i=0; i<=n2; i++){
//      x2 = (Double_t(i)/n2)*(setHistoRange);
//      in2[i] =  fsin2->Eval(x2);
//   }
//
//   //Make our own TVirtualFFT object (using option "K")
//   //Third parameter (option) consists of 3 parts:
//   //-transform type:
//   // real input/complex output in our case
//   //-transform flag: 
//   // the amount of time spent in planning
//   // the transform (see TVirtualFFT class description)
//   //-to create a new TVirtualFFT object (option "K") or use the global (default)
//   Int_t n_size2 = n2+1;
//   TVirtualFFT *fft_own2 = TVirtualFFT::FFT(1, &n_size2, "R2C ES K");
//   if (!fft_own2) return;
//   fft_own2->SetPoints(in2);
//   fft_own2->Transform();
//
//   //Copy all the output points:
//   fft_own2->GetPoints(in2);
//   //Draw the real part of the output
//   myc2->cd();
//   TH1 *hr2 = 0;
//   hr2 = TH1::TransformHisto(fft_own2, hr2, "RE");
////   hr2->SetTitle("Real part of the 3rd (array) tranfsorm");
////   hr2->Draw("SAME");
////   hr2->SetStats(kFALSE);
////   hr2->GetXaxis()->SetLabelSize(0.05);
////   hr2->GetYaxis()->SetLabelSize(0.05);
////   myc2->cd();
//
//   TH1 *him2 = 0;
//   him2 = TH1::TransformHisto(fft_own2, him2, "IM");
////   him2->SetTitle("Im. part of the 3rd (array) transform");
////   him2->Draw("SAME");
////   him2->SetStats(kFALSE);
////   him2->GetXaxis()->SetLabelSize(0.05);
////   him2->GetYaxis()->SetLabelSize(0.05);
//
//

////   TF1 *fcos = new TF1("fcos", "cos(x)+cos(0.5*x)+cos(2*x)+1", 0, setHistoRange);
//   TF1 *fgaus = new TF1("fgaus", "gaus", 0, setHistoRange);
//   for (Int_t i=0; i<=n2; i++){
//      x2 = (Double_t(i)/n2)*(setHistoRange);
//      in2[i] = fgaus->Eval(x2);
//   }
//   fft_own2->SetPoints(in2);
//   fft_own2->Transform();
////   fft_own2->GetPointComplex(0, re_2, im_2);
////   printf("2nd transform: DC component: %f\n", re_2);
////   fft_own2->GetPointComplex(n2/2+1, re_2, im_2);
////   printf("2nd transform: Nyquist harmonic: %f\n", re_2);









//   //Now let's make a backward transform:
//   TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &n2, "C2R M K");
//   fft_back->SetPointsComplex(re_full2,im_full2);
//   fft_back->Transform();
//   TH1 *hb = 0;
//   //Let's look at the output
//   hb = TH1::TransformHisto(fft_back,hb,"Re");
//   hb->SetTitle("The backward transform result");
//   hb->Draw();
//   //NOTE: here you get at the x-axes number of bins and not real values
//   //(in this case 25 bins has to be rescaled to a range between 0 and 4*Pi; 
//   //also here the y-axes has to be rescaled (factor 1/bins)
//   hb->SetStats(kFALSE);
//   hb->GetXaxis()->SetLabelSize(0.05);
//   hb->GetYaxis()->SetLabelSize(0.05);
//   delete fft_back;
//   fft_back=0;

//   TVirtualFFT *fft_own2_reverse = TVirtualFFT::FFT(1, &n_size, "C2R ES K");
//   fft_own2_reverse->SetPoints(in);
//   fft_own2_reverse->Transform();
//   delete fft_own2_reverse;

}






//This tutorial illustrates the Fast Fourier Transforms interface in ROOT.
//FFT transform types provided in ROOT:
// - "C2CFORWARD" - a complex input/output discrete Fourier transform (DFT) 
//                  in one or more dimensions, -1 in the exponent
// - "C2CBACKWARD"- a complex input/output discrete Fourier transform (DFT) 
//                  in one or more dimensions, +1 in the exponent
// - "R2C"        - a real-input/complex-output discrete Fourier transform (DFT)
//                  in one or more dimensions,
// - "C2R"        - inverse transforms to "R2C", taking complex input 
//                  (storing the non-redundant half of a logically Hermitian array) 
//                  to real output
// - "R2HC"       - a real-input DFT with output in ¡Èhalfcomplex¡É format, 
//                  i.e. real and imaginary parts for a transform of size n stored as
//                  r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1
// - "HC2R"       - computes the reverse of FFTW_R2HC, above
// - "DHT"        - computes a discrete Hartley transform
// Sine/cosine transforms:
//  DCT-I  (REDFT00 in FFTW3 notation)
//  DCT-II (REDFT10 in FFTW3 notation)
//  DCT-III(REDFT01 in FFTW3 notation)
//  DCT-IV (REDFT11 in FFTW3 notation)
//  DST-I  (RODFT00 in FFTW3 notation)
//  DST-II (RODFT10 in FFTW3 notation)
//  DST-III(RODFT01 in FFTW3 notation)
//  DST-IV (RODFT11 in FFTW3 notation)
//First part of the tutorial shows how to transform the histograms
//Second part shows how to transform the data arrays directly
//Authors: Anna Kreshuk and Jens Hoffmann


//********* Histograms ********//


//   //prepare the canvas for drawing
//   TCanvas *myc = new TCanvas("myc", "Fast Fourier Transform", 800, 600);
//   myc->SetFillColor(45);
//   TPad *c1_1 = new TPad("c1_1", "c1_1",0.01,0.67,0.49,0.99);
//   TPad *c1_2 = new TPad("c1_2", "c1_2",0.51,0.67,0.99,0.99);
//   TPad *c1_3 = new TPad("c1_3", "c1_3",0.01,0.34,0.49,0.65);
//   TPad *c1_4 = new TPad("c1_4", "c1_4",0.51,0.34,0.99,0.65);
//   TPad *c1_5 = new TPad("c1_5", "c1_5",0.01,0.01,0.49,0.32);
//   TPad *c1_6 = new TPad("c1_6", "c1_6",0.51,0.01,0.99,0.32);
//   c1_1->Draw();
//   c1_2->Draw();
//   c1_3->Draw();
//   c1_4->Draw();
//   c1_5->Draw();
//   c1_6->Draw();
//   c1_1->SetFillColor(30);
//   c1_1->SetFrameFillColor(42);
//   c1_2->SetFillColor(30);
//   c1_2->SetFrameFillColor(42);
//   c1_3->SetFillColor(30);
//   c1_3->SetFrameFillColor(42);
//   c1_4->SetFillColor(30);
//   c1_4->SetFrameFillColor(42);
//   c1_5->SetFillColor(30);
//   c1_5->SetFrameFillColor(42);
//   c1_6->SetFillColor(30);
//   c1_6->SetFrameFillColor(42);
//   
//   c1_1->cd();
//   TH1::AddDirectory(kFALSE);
//
//   //A function to sample
//   TF1 *fsin = new TF1("fsin", "sin(x)+sin(2*x)+sin(0.5*x)+1", 0, setHistoRange);
//   fsin->Draw();
//   
//   Int_t n=2500;
//   TH1D *hsin = new TH1D("hsin", "hsin", n+1, 0, setHistoRange);
//   Double_t x;
//   
//   //Fill the histogram with function values
//   for (Int_t i=0; i<=n; i++){
//      x = (Double_t(i)/n)*(setHistoRange);
//      hsin->SetBinContent(i+1, fsin->Eval(x));
//   }
//   hsin->Draw("same");
//   fsin->GetXaxis()->SetLabelSize(0.05);
//   fsin->GetYaxis()->SetLabelSize(0.05);
//   
//   c1_2->cd();
//   //Compute the transform and look at the magnitude of the output
//   TH1 *hm =0;
//   TVirtualFFT::SetTransform(0);
//   hm = hsin->FFT(hm, "MAG");
//   hm->SetTitle("Magnitude of the 1st transform");
//   hm->Draw();
//   //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function 
//   //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
//   
//   hm->SetStats(kFALSE);
//   hm->GetXaxis()->SetLabelSize(0.05);
//   hm->GetYaxis()->SetLabelSize(0.05);
//   c1_3->cd();
//   //Look at the phase of the output
//   TH1 *hp = 0;
//   hp = hsin->FFT(hp, "PH");
//   hp->SetTitle("Phase of the 1st transform");
//   hp->Draw();
//   hp->SetStats(kFALSE);
//   hp->GetXaxis()->SetLabelSize(0.05);
//   hp->GetYaxis()->SetLabelSize(0.05);
//   
//   //Look at the DC component and the Nyquist harmonic:
////   TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
//   Double_t re, im;
//   //That's the way to get the current transform object:
//   TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
//   c1_4->cd();
//   //Use the following method to get just one point of the output
//   fft->GetPointComplex(0, re, im);
//   printf("1st transform: DC component: %f\n", re);
//   fft->GetPointComplex(n/2+1, re, im);
//   printf("1st transform: Nyquist harmonic: %f\n", re);
//
//   //Use the following method to get the full output:
//   Double_t *re_full = new Double_t[n];
//   Double_t *im_full = new Double_t[n];
//   fft->GetPointsComplex(re_full,im_full);
//  
//
//   //Now let's make a backward transform:
//   TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &n, "C2R M K");
//   fft_back->SetPointsComplex(re_full,im_full);
//   fft_back->Transform();
//   TH1 *hb = 0;
//   //Let's look at the output
//   hb = TH1::TransformHisto(fft_back,hb,"Re");
//   hb->SetTitle("The backward transform result");
//   hb->Draw();
//   //NOTE: here you get at the x-axes number of bins and not real values
//   //(in this case 25 bins has to be rescaled to a range between 0 and 4*Pi; 
//   //also here the y-axes has to be rescaled (factor 1/bins)
//   hb->SetStats(kFALSE);
//   hb->GetXaxis()->SetLabelSize(0.05);
//   hb->GetYaxis()->SetLabelSize(0.05);
//   delete fft_back;
////   fft_back=0;


////********* Data array - same transform ********//
//
//   //Allocate an array big enough to hold the transform output
//   //Transform output in 1d contains, for a transform of size N, 
//   //N/2+1 complex numbers, i.e. 2*(N/2+1) real numbers
//   //our transform is of size n+1, because the histogram has n+1 bins
//
//   Double_t *in = new Double_t[2*((n+1)/2+1)];
//   Double_t re_2,im_2;
//   for (Int_t i=0; i<=n; i++){
//      x = (Double_t(i)/n)*(setHistoRange);
//      in[i] =  fsin->Eval(x);
//   }
//
//   //Make our own TVirtualFFT object (using option "K")
//   //Third parameter (option) consists of 3 parts:
//   //-transform type:
//   // real input/complex output in our case
//   //-transform flag: 
//   // the amount of time spent in planning
//   // the transform (see TVirtualFFT class description)
//   //-to create a new TVirtualFFT object (option "K") or use the global (default)
//   Int_t n_size = n+1;
//   TVirtualFFT *fft_own = TVirtualFFT::FFT(1, &n_size, "R2C ES K");
//   if (!fft_own) return;
//   fft_own->SetPoints(in);
//   fft_own->Transform();
//
//   //Copy all the output points:
//   fft_own->GetPoints(in);
//   //Draw the real part of the output
//   c1_5->cd();
//   TH1 *hr = 0;
//   hr = TH1::TransformHisto(fft_own, hr, "RE");
//   hr->SetTitle("Real part of the 3rd (array) tranfsorm");
//   hr->Draw();
//   hr->SetStats(kFALSE);
//   hr->GetXaxis()->SetLabelSize(0.05);
//   hr->GetYaxis()->SetLabelSize(0.05);
//   c1_6->cd();
//   TH1 *him = 0;
//   him = TH1::TransformHisto(fft_own, him, "IM");
//   him->SetTitle("Im. part of the 3rd (array) transform");
//   him->Draw();
//   him->SetStats(kFALSE);
//   him->GetXaxis()->SetLabelSize(0.05);
//   him->GetYaxis()->SetLabelSize(0.05);

////   myc->cd();
//   //Now let's make another transform of the same size
//   //The same transform object can be used, as the size and the type of the transform
//   //haven't changed
//   TF1 *fcos = new TF1("fcos", "cos(x)+cos(0.5*x)+cos(2*x)+1", 0, setHistoRange);
//   for (Int_t i=0; i<=n; i++){
//      x = (Double_t(i)/n)*(setHistoRange);
//      in[i] =  fcos->Eval(x);
//   }
//   fft_own->SetPoints(in);
//   fft_own->Transform();
//   fft_own->GetPointComplex(0, re_2, im_2);
//   printf("2nd transform: DC component: %f\n", re_2);
//   fft_own->GetPointComplex(n/2+1, re_2, im_2);
//   printf("2nd transform: Nyquist harmonic: %f\n", re_2);
//
//   delete fft_own;
//   delete [] in;
//   delete [] re_full;
//   delete [] im_full;

