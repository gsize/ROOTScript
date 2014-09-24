#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include<iostream>
#include<fstream>
#include <cstdlib>
#include<sstream>
#include<complex.h>
#include<stdio.h>
#include<math.h>
#define false 0
#define true 1
#define pi 3.14159

Int_t n=1000;
Double_t *in = new Double_t[2*((n+1)/2+1)];
int OK;
Int_t nsize=1001;

double filter1(double, double, double);
void INPUT(int *,double *, int *);

void DoRCFilterNew()
{
  //prepare the canvas for drawing
   TCanvas *myc = new TCanvas("myc", "Fast Fourier Transform", 800, 600);
   myc->SetFillColor(45);
   TPad *c1_1 = new TPad("c1_1", "c1_1",0.01,0.67,0.49,0.99);
   TPad *c1_2 = new TPad("c1_2", "c1_2",0.51,0.67,0.99,0.99);
   TPad *c1_3 = new TPad("c1_3", "c1_3",0.01,0.34,0.49,0.65);
   TPad *c1_4 = new TPad("c1_4", "c1_4",0.51,0.34,0.99,0.65);
   TPad *c1_5 = new TPad("c1_5", "c1_5",0.01,0.01,0.49,0.32);
   TPad *c1_6 = new TPad("c1_6", "c1_6",0.51,0.01,0.99,0.32);
   c1_1->Draw();
   c1_2->Draw();
   c1_3->Draw();
   c1_4->Draw();
   c1_5->Draw();
   c1_6->Draw();
   c1_1->SetFillColor(30);
   c1_1->SetFrameFillColor(42);
   c1_2->SetFillColor(30);
   c1_2->SetFrameFillColor(42);
   c1_3->SetFillColor(30);
   c1_3->SetFrameFillColor(42);
   c1_4->SetFillColor(30);
   c1_4->SetFrameFillColor(42);
   c1_5->SetFillColor(30);
   c1_5->SetFrameFillColor(42);
   c1_6->SetFillColor(30);
   c1_6->SetFrameFillColor(42);
   
   c1_1->cd();
   TH1::AddDirectory(kFALSE);
     
   //A function to sample
   TF1 *fsin = new TF1("fsin", "sin(x)+sin(2*x)+sin(0.5*x)+1", 0, 4*TMath::Pi());
   //TF1 *fsin = new TF1("fsin", "sin(x)", 0, 1000);
   //fsin->Draw();

   //TH1D *hsin = new TH1D("hsin", "hsin", n+1, 0, 4*TMath::Pi());
   TH1D *hsin = new TH1D("hsin", "hsin", n+1, 0, 1000);
   Double_t x;

   INPUT(&OK, in, &nsize);
   //Fill the histogram with function values
   for (Int_t i=0; i<=n; i++){
     //x = (Double_t(i)/n)*(4*TMath::Pi());
      x = (Double_t(i)/n)*(1000);
      //hsin->SetBinContent(i+1,in[i]*(1+.1*sin(55*x))+1);
      hsin->SetBinContent(i+1,in[i]+1);
   }
   //cout << hsin->GetBinContent(43) << endl;
   //hsin->Smooth(2500);
   hsin->Draw();
   fsin->GetXaxis()->SetLabelSize(0.05);
   fsin->GetYaxis()->SetLabelSize(0.05);
   
   c1_2->cd();
   //Compute the transform and look at the magnitude of the output
   TH1 *hm =0;
   TVirtualFFT::SetTransform(0);
   hm = hsin->FFT(hm, "MAG");
   hm->SetTitle("Magnitude of the 1st transform");
   hm->Draw();
   //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function 
   //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
   
   hm->SetStats(kFALSE);
   hm->GetXaxis()->SetLabelSize(0.05);
   hm->GetYaxis()->SetLabelSize(0.05);
   c1_3->cd();   
   //Look at the phase of the output   
   /*TH1 *hp = 0;
   hp = hsin->FFT(hp, "PH");
     hp->SetTitle("Phase of the 1st transform");*/
   //Do it with the imaginary part
   TVirtualFFT *fft1 = TVirtualFFT::GetCurrentTransform();
   TH1 *hp =0;
   hp = hsin->FFT(hp, "RE");
   hp->SetTitle("Imaginary Part of the 1st transform");
   hp->Draw();
   hp->SetStats(kFALSE);
   hp->GetXaxis()->SetLabelSize(0.05);
   hp->GetYaxis()->SetLabelSize(0.05);

   int yy, ww, kk, jj, ll;
   double zz, xx;
   double beta=0.25;
   double omega=1000.0;
   Double_t T=2*pi/omega;
   Double_t cc, dd, ee, ff;
   int stop=5;
   TComplex comp[n];
   zz = (1-beta)/(2*T);
   xx = (1+beta)/(2*T);
   ww = ceil(zz);
   yy = ceil(xx);
   //Look at the DC component and the Nyquist harmonic:
   Double_t re, im;
   //That's the way to get the current transform object:
   TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
   c1_4->cd();

   for (jj=1; jj <= zz; jj++) {
     fft->GetPointComplex(jj, re, im);
     cc = re*T;
     dd = im*T;
     comp[jj] = (cc,dd);
     //cout << comp[jj] << endl;
   }
   for (kk=ww; kk <= xx; kk++) {
     fft->GetPointComplex(jj, re, im);
     ee = re*filter1(kk, beta, T);
     ff = im*filter1(kk, beta, T);
     comp[kk] = (ee,ff);
     //cout << comp[kk] << endl;
   }
   for (ll=yy; ll<=n; ll++) {
     comp[ll] = 0;
     }
   //Use the following method to get just one point of the output
   fft->GetPointComplex(0, re, im);
   printf("1st transform: DC component: %f\n", re);
   fft->GetPointComplex(n/2+1, re, im);
   printf("1st transform: Nyquist harmonic: %f\n", re);

   //Use the following method to get the full output:
   Double_t *re_full = new Double_t[n];
   Double_t *im_full = new Double_t[n];
   fft->GetPointsComplex(re_full,im_full);
   
   //Now let's make a backward transform:
   TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &n, "C2R M K");
   fft_back->SetPointsComplex(re_full,im_full);

   for (jj=1; jj <= zz; jj++) {
     fft_back->SetPointComplex(jj, comp[jj]);
     }
   cout << (1-beta)/(2*T) << " : " << (1+beta)/(2*T) << endl;
   for (kk=ww; kk <= xx; kk++) {
     fft_back->SetPointComplex(kk, comp[kk]);
     }
   for (ll=yy; ll <= n; ll++) {
     fft_back->SetPointComplex(ll, comp[ll]);
     }
   fft_back->Transform();
   TH1 *hb = 0;
   //Let's look at the output
   hb = TH1::TransformHisto(fft_back,hb,"Re");
   hb->SetTitle("The backward transform result");
   //hb->Smooth(2500);
   hb->Draw();
   //NOTE: here you get at the x-axes number of bins and not real values
   //(in this case 25 bins has to be rescaled to a range between 0 and 4*Pi; 
   //also here the y-axes has to be rescaled (factor 1/bins)
   hb->SetStats(kFALSE);
   hb->GetXaxis()->SetLabelSize(0.05);
   hb->GetYaxis()->SetLabelSize(0.05);
   delete fft_back;
   fft_back=0;

//********* Data array - same transform ********//

   //Allocate an array big enough to hold the transform output
   //Transform output in 1d contains, for a transform of size N, 
   //N/2+1 complex numbers, i.e. 2*(N/2+1) real numbers
   //our transform is of size n+1, because the histogram has n+1 bins

   ofstream outdata;
   int Z;
   Double_t re_2,im_2;
   //cout << in[122] << " : " << in[133] << " : " << n << endl;
   for (Int_t i=0; i<=n; i++){
     //x = (Double_t(i)/n)*(4*TMath::Pi());
     x = (Double_t(i)/n)*(1000);
     //cout << x << endl;
     //in[i] =  fsin->Eval(x);
     in[i] =  in[i];
     //cout << in[i] << " : " << endl;
   }
   //Make our own TVirtualFFT object (using option "K")
   //Third parameter (option) consists of 3 parts:
   //-transform type:
   // real input/complex output in our case
   //-transform flag: 
   // the amount of time spent in planning
   // the transform (see TVirtualFFT class description)
   //-to create a new TVirtualFFT object (option "K") or use the global (default)
   Int_t n_size = n+1;
   TVirtualFFT *fft_own = TVirtualFFT::FFT(1, &n_size, "R2C ES K");
   if (!fft_own) return;
   fft_own->SetPoints(in);
   fft_own->Transform();

   //Copy all the output points:
   fft_own->GetPoints(in);
   //Draw the real part of the output
   c1_5->cd();
   TH1 *hr = 0;
   hr = TH1::TransformHisto(fft_own, hr, "RE");
   hr->SetTitle("Real part of the 3rd (array) transform");
   hr->Draw();
   hr->SetStats(kFALSE);
   hr->GetXaxis()->SetLabelSize(0.05);
   hr->GetYaxis()->SetLabelSize(0.05);
   c1_6->cd();
   TH1 *him = 0;
   him = TH1::TransformHisto(fft_own, him, "IM");
   him->SetTitle("Im. part of the 3rd (array) transform");
   him->Draw();
   him->SetStats(kFALSE);
   him->GetXaxis()->SetLabelSize(0.05);
   him->GetYaxis()->SetLabelSize(0.05);

   myc->cd();
   //Now let's make another transform of the same size
   //The same transform object can be used, as the size and the type of the transform
   //haven't changed
   /*TF1 *fcos = new TF1("fcos", "cos(x)+cos(0.5*x)+cos(2*x)+1", 0, 4*TMath::Pi());
   for (Int_t i=0; i<=n; i++){
      x = (Double_t(i)/n)*(4*TMath::Pi());
      in[i] =  fcos->Eval(x);
   }
   fft_own->SetPoints(in);
   fft_own->Transform();
   fft_own->GetPointComplex(0, re_2, im_2);
   printf("2nd transform: DC component: %f\n", re_2);
   fft_own->GetPointComplex(n/2+1, re_2, im_2);
   printf("2nd transform: Nyquist harmonic: %f\n", re_2);*/
   delete fft_own;
   delete [] in;
   delete [] re_full;
   delete [] im_full;
}

void INPUT(int *OK, double *Y, int *n)
{
   int K, J, FLAG;
   char A;
   char NAME[30];
   FILE *INP; 

   printf("This is the Fast Fourier Transform.\n\n");
   printf("The user must make provisions if the\n");
   printf("interval is not [-pi,pi].\n");
   printf("The example illustrates the required\n");
   printf("provisions under input method 3.\n");
   *OK = false;
   while (!(*OK)) {
      printf("Choice of input method:\n");
      printf("2. Input data from a text file\n");
      scanf("%d", &FLAG);
      if ((FLAG == 1) || (FLAG == 2) || (FLAG == 3)) *OK = true;
   }
   switch (FLAG) {
      case 2:
         printf("Has a text file been created with the ");
         printf("entries y(0),...,y(2m-1)\n");
         printf("separated by a blank?\n");
         printf("Enter Y or N\n");
         scanf("\n%c", &A);
         if ((A == 'Y') || (A == 'y')) {
            printf("Input the file name in the form - ");
            printf("drive:name.ext or /datafolder/name.ext\n");
            printf("for example:   A:DATA.DTA or /usr/local/data/Data.dta\n");
            scanf("%s", NAME);
            INP = fopen(NAME, "r");
            *OK = false;
            while (!(*OK)) {
	      //*n = 1000;
               if (*n > 0) {
                  for (K=1; K<=*n; K++)  
		  {
			fscanf(INP, "%lf", &Y[K-1]);
		  }
		fclose(INP);
		*OK = true;
               }
               else printf("Number must be a positive integer.\n");
            }
         }
         else {
            printf("The program will end so the input file can ");
            printf("be created.\n");
            *OK = false;
         }
	 //cout << Y[122] << " : " << Y[133] << endl;
   }
}

double filter1 (double f, double beta, double T)
{
	double Hf;
	Hf = T*(1+cos(pi*T*(fabs(f)-(1-beta)/(2*T))/beta))/2;
	return (Hf);
}
