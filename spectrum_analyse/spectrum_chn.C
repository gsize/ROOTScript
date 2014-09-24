
	// ROOT style
 // gROOT->Reset();
#include "TAxis.h"
#include "TH1.h"
#include "TArrayD.h"

#define long_4 int

Double_t ScaleX(Double_t x)
{
  Double_t v;
  v = 0.157 +  0.136 * x  -3.172E-09*x*x; // "linear scaling" function example
  return v;
}

Double_t ScaleY(Double_t y)
{
  Double_t v;
  v = 20 * y + 200; // "linear scaling" function example
  return v;
}

Double_t ScaleZ(Double_t z)
{
  Double_t v;
  v = 30 * z + 300; // "linear scaling" function example
  return v;
}

void ScaleAxis(TAxis *a, Double_t (*Scale)(Double_t))
{
  if (!a) return; // just a precaution
  if (a->GetXbins()->GetSize())
    {
      // an axis with variable bins
      // note: bins must remain in increasing order, hence the "Scale"
      // function must be strictly (monotonically) increasing
      TArrayD X(*(a->GetXbins()));
      for(Int_t i = 0; i < X.GetSize(); i++) X[i] = Scale(X[i]);
      a->Set((X.GetSize() - 1), X.GetArray()); // new Xbins
    }
  else
    {
      // an axis with fix bins
      // note: we modify Xmin and Xmax only, hence the "Scale" function
      // must be linear (and Xmax must remain greater than Xmin)
      a->Set(a->GetNbins(),
        Scale(a->GetXmin()), // new Xmin
        Scale(a->GetXmax())); // new Xmax
    }
  return;
}

void ScaleXaxis(TH1 *h, Double_t (*Scale)(Double_t))
{
  if (!h) return; // just a precaution
  ScaleAxis(h->GetXaxis(), Scale);
  return;
}

void ScaleYaxis(TH1 *h, Double_t (*Scale)(Double_t))
{
  if (!h) return; // just a precaution
  ScaleAxis(h->GetYaxis(), Scale);
  return;
}

void ScaleZaxis(TH1 *h, Double_t (*Scale)(Double_t))
{
  if (!h) return; // just a precaution
  ScaleAxis(h->GetZaxis(), Scale);
  return;
}

/*
hist1->Draw();
ScaleXaxis(hist1, ScaleX);
hist1->Draw();
ScaleYaxis(hist1, ScaleY);
hist1->Draw();
hist1->ResetStats();
*/


/* ------------------ ORTEC PCAHEADER STRUCT ----------------------------------------------- */
typedef struct
{
    /* byte offset. Description of the variable                        */
    short   leader;      /* must be -1                                                      */
    short   mca_number;  /* MCA number                                                      */
    short   seg_number;  /* Segment number                                                  */
    char  sec[2];      /* ASCII seconds of start time                                     */
    long_4  realtime;    /* Realtime (increments of 20 ms)                                  */
    long_4  livetime;    /* Livetime (increments of 20 ms)                                  */
    char  date[8];     /* Start date as ASCII DDMMMYY0, or NULL, if not known             */
    char  time[4];     /* Start time as ASCII HHMM, or NULL,if not known(see sec[2] above)*/
    short   chn_offset;  /* Channel offset of data                                          */
    short   chn_number;  /* Number of channels (length of data)                             */
} PCAHEADER_O;
/* ---------------------- PCAHEADER STRUCT ------------------------------------------------- */

/* ---------------------- ORTEC END STRUCT ------------------------------------------- */
typedef struct
{
    short   leader;     /* Must be -101                                                     */
    short   spm_num;    /* spectrum number                                                  */
    float c0;         /* Energy calibration zero intercept, 0.0 for uncalibrated spectrum */
    float c1;         /* Energy calibration slope, 1.0 for uncalibrated spectrum          */
    float c2;         /* Reserved                                                         */
    float f0;         /* Peak shape calibration zero intercept, 1.0 for uncalibrated      */
    float f1;         /* Peak shape calibration slope, 1.0 for uncalibrated spectrum      */
    char  buffer[232];/* Reserved                                                         */
    char  length0;    /* Length of detector description                                   */
    char  desc_d[63]; /* Detector description                                             */
    char  length1;    /* Length of sample description                                     */
    char  desc_s[63]; /* Sample description                                               */
    char  buffr1[126];/* Reserved (The total length is 512 bytes)                         */
    short   naa_flag;   /* Must be 27575, if processed by NAA V2.60                         */
} PCA_END;
/* ------------------------ ORTEC END STRUCT ----------------------------------------- */

PCAHEADER_O pcaheader_o;
PCA_END pcaheader_end;
long_4 *ptra,*ptra_t;

short fast_log(unsigned long_4 count)
{
    register short l,i;
    char log2[25]=
    {
        -3, 0, 12,20,24,28,32,34,36,38,40,42,44,45,46,47,48,49,50,51,52,53,54,55,56,
    };

    l=0;
    while(count > 24)
    {
        count/=2;
        l++;
    }

    i=l*log2[2]+log2[count];
    i+=3;
    return(i);
}

int rdspm_o(const char* filename)
{
    short i;
    FILE* stream;
    //unsigned long_4 ll;
    long_4 ll;

    if(ptra)
    {
        delete [] ptra;
        ptra=NULL;
    }

    printf("filename=%s\n",filename);
    if ((stream = fopen(filename, "rb")) == NULL)  return (-1);
    //if (fread(&(pcaheader_o), sizeof(char), sizeof(PCAHEADER_O), stream) != sizeof(PCAHEADER_O))return(-1);

    fread(&(pcaheader_o.leader), sizeof(short), 1, stream);
    printf("leader=%d\n",pcaheader_o.leader);
    fread(&(pcaheader_o.mca_number), sizeof(short), 1, stream);
    printf("mca_number=%d\n",pcaheader_o.mca_number);
    fread(&(pcaheader_o.seg_number), sizeof(short), 1, stream);
    fread(pcaheader_o.sec, sizeof(char), 2, stream);
    fread(&(pcaheader_o.realtime), sizeof(long_4), 1, stream);
    fread(&(pcaheader_o.livetime), sizeof(long_4), 1, stream);
    fread(pcaheader_o.date, sizeof(char), 8, stream);
    fread(pcaheader_o.time, sizeof(char), 4, stream);
    fread(&(pcaheader_o.chn_offset), sizeof(short), 1, stream);
    fread(&(pcaheader_o.chn_number), sizeof(short), 1, stream);
    printf("2 chn_number=%d\n",pcaheader_o.chn_number);

    ptra= new long_4[pcaheader_o.chn_number];
    for (i = 0; i < pcaheader_o.chn_number; i++)
    {
        if (fread(&ll, sizeof(long_4), 1, stream) > 0)
        {
            ptra[i] = ll;
        }
    }
    //printf("2 is ok?\n");
    if (i != pcaheader_o.chn_number)
    {
        delete [] ptra;
        ptra=NULL;
        printf("No data!\n");
        return(-2);
    }
//printf("3 is ok?\n");
    //if (fread(&(pcaheader_end), sizeof(char), sizeof(PCA_END), stream) != sizeof(PCA_END))return(-1);

    fread(&(pcaheader_end.leader), sizeof(short), 1, stream);
    fread(&(pcaheader_end.spm_num), sizeof(short), 1, stream);
    fread(&(pcaheader_end.c0), sizeof(float), 1, stream);
    fread(&(pcaheader_end.c1), sizeof(float), 1, stream);
    fread(&(pcaheader_end.c2), sizeof(float), 1, stream);
    fread(&(pcaheader_end.f0), sizeof(float), 1, stream);
    fread(&(pcaheader_end.f1), sizeof(float), 1, stream);
    fread(pcaheader_end.buffer, sizeof(char), 232, stream);
    fread(&(pcaheader_end.length0), sizeof(char), 1, stream);
    fread(pcaheader_end.desc_d, sizeof(char), 63, stream);
    fread(&(pcaheader_end.length1), sizeof(char), 1, stream);
    fread(pcaheader_end.desc_s, sizeof(char), 63, stream);
    fread(pcaheader_end.buffr1, sizeof(char), 126, stream);
    fread(&(pcaheader_end.naa_flag), sizeof(short), 1, stream);

    fclose(stream);
    return (0);
}

int func2()
{
    unsigned long  *pp;
    register short jj;
    short kk,numb,num0;
    unsigned long ll, *lc, *lc0;

    ptra_t=new unsigned long[pcaheader_o.chn_number];
    num0=pcaheader_o.chn_number;
    for(Int_t i =0; i<num0; i++) ptra_t[i]=ptra[i];
    kk=1;// pcaheader.n.chn_number/num0;
    kk=kk<1? 1:kk;
    pp=lc=lc0=ptra_t;
    numb=0;
    while( numb<num0 )
    {
        ll=0;
        jj=1;
        while(jj<=kk)
        {
            ll+=*lc;
            lc++;
            jj++;
        }
        if(numb < 6)
        {
            *pp= *lc0;
            lc0++;
        }
        else
            *pp=ll%0x1000000;
        numb++;
        pp++;
    }
}

int smooth1(Float_t *s,Float_t *d,Int_t num)
{
    if(num<3)
    {
        for(int i=2; i<num-2; i++)
        {
            //d[i]=(-3*(s[i+2]+s[i-2]) + 12*(s[i+1]+s[i-1])+17*s[i])/35;
            d[i]=((s[i+2]+s[i-2]) + 4*(s[i+1]+s[i-1])+6*s[i])/16;
        }
        return 1;
    }
    else
        return -1;
}

int smooth2(float *s,float *d,int num)
{
    if(num<3)
    {
        for(int i=2; i<num-2; i++)
        {
            d[i]=(s[i-2]-8*s[i-1] + 8*s[i+1]-s[i-2])/12;
        }
        return 1;
    }
    else
        return -1;
}


typedef struct
{
    int left;
    int righ;
} endpoint;

void find_endpoint(float* source,int  nbins,float* peak_list,int num_peak,endpoint* endpoint_list)
{
    double K=1.;
    int n;
    float *y=source;
    //endpoint_list=new endpoint[num_peak];

    for(int i=0; i<num_peak; i++)
    {
        n=2;
        int peak_addr=(int)peak_list[i];
        while(y[peak_addr-1-n]<(y[peak_addr-n]-K*sqrt(y[peak_addr-n])))
        {
            if(peak_addr-n<1)
            {
                break;
            };
            n++;
        }
        endpoint_list[i].left=peak_list[i]-1-n;

        n=2;
        while(y[peak_addr+1+n]<(y[peak_addr+n]-K*sqrt(y[peak_addr+n])))
        {
            if(peak_addr+n>nbins-2)
            {
                break;
            }
            n++;
        }
        endpoint_list[i].righ=peak_list[i]+1+n;
    }
}

void cal_area(float *source,int nbins,endpoint* endpoint_l,int num_l,double *area,double *Va)
{
    //Va=new double[num_l];
    //area=new double[num_l];
    for(int i=0; i<num_l; i++)
    {
        double sum=0;
        int l=endpoint_l[i].left;
        int r=endpoint_l[i].righ;
        for(int j=l; j<=r; j++) sum+=source[j];
        Va[i]=sqrt(sum+(r-l+1)*(r-l)*(source[l]+source[r])/4);
        area[i]=sum-(r-l+1)*(source[l]+source[r])/2;
    }
}
/**/
void spectrum_chn()
{

    //char *fname="Cs137_eff_15cm_1.Chn";
    //char *fname="2N021.chn";
	 TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("spectrum_chn.C","");
   dir.ReplaceAll("/./","/");
    TString pathFile;
    pathFile = dir + "ba133_8.Chn";

    int r=rdspm_o(pathFile.Data());
    if(r<0) return;

    Int_t nbin=pcaheader_o.chn_number;

    Int_t fNPeaks = 0;
    Int_t i,nfound,bin;
    Int_t nbins = nbin;
    Double_t xmin  = 0;
    Double_t xmax  = (Double_t)nbins;
    Float_t * source = new float[nbins];
    Float_t * dest = new float[nbins];

    TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,600);
    TH1F *h = new TH1F("h","peak searching",nbins,xmin,xmax);
    TH1F *d = new TH1F("d","",nbins,xmin,xmax);
    TH1F *d1 = new TH1F("background","",nbins,xmin,xmax);
    TH1F *d2 = new TH1F("d2","",nbins,xmin,xmax);
    TH1F *d3 = new TH1F("smoothing","",nbins,xmin,xmax);
    TH1F *d4 = new TH1F("d4","smooth1",nbins,xmin,xmax);
    TH1F *d5 = new TH1F("d5","",nbins,xmin,xmax);
    TH1F *d6 = new TH1F("d6","",nbins,xmin,xmax);
    //TH1F *d7 = new TH1F("d7","",nbins,xmin,xmax);
    gPad->SetLogy(1);

    //for(int i=0; i<nbin; i++) h->SetBinContent(i+1,fast_log(ptra[i]));
    for(int i=0; i<nbin; i++) h->SetBinContent(i+1,ptra[i]);
    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    h->Draw("L");
    // h->SetMaximum(1300);
    TSpectrum *s = new TSpectrum(500,3);
    ///*
        s->SmoothMarkov(source,nbins,7);  //3, 7, 10
        for (i = 0; i < nbins; i++) d3->SetBinContent(i + 1,source[i]);
        d3->SetLineColor(kGreen);
        d3->Draw("SAME");
    //*/
    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    nfound = s->SearchHighRes(source, dest, nbins, 2, 0.8, kTRUE, 2, kTRUE, 2);
    printf("nfound2=%d\n",nfound);

    for (i = 0; i < nbins; i++) d6->SetBinContent(i + 1,dest[i]);
    d6->SetLineColor(kYellow);
    //d6->Draw("SAME");

    Float_t *xpeaks = s->GetPositionX();
    Float_t *fPositionX = new Float_t[nfound];
    Float_t *fPositionY = new Float_t[nfound];
    for (i = 0; i < nfound; i++)
    {
    	Float_t a=0.;
        a=xpeaks[i];
        bin = 1 + Int_t(a + 0.5);
        fPositionX[i] = h->GetBinCenter(bin);
        //if(i<10) printf("%f\n",fPositionX[i]);
        fPositionY[i] = h->GetBinContent(bin);
    }


TCanvas *c2 = new TCanvas("c2","c2",10,10,1000,600);
    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    //h->Draw("L");
    ScaleXaxis(h, ScaleX);
    h->Draw();
    h->ResetStats();
 /*   endpoint* endpoint_list=new endpoint[nfound];;
    Float_t *Va=new double[nfound];
    Float_t *area=new double[nfound];
    Float_t *des=new double[nbins];
    //smooth1(source,des,nbins);
        if( smooth1(source,des,nbins) >= 0)
        {
        for (i = 0; i < nbins; i++)
        	d4->SetBinContent(i + 1,des[i]);
        d4->SetLineColor(kYellow);
        d4->Draw("L");
        }
        find_endpoint( des,nbins,xpeaks,nfound,endpoint_list);
        for(int i=0; i<nfound; i++) {
            printf("%6d\t%6d\t%6d\t%6d\t%6d\n",i,endpoint_list[i].left,xpeaks[i],endpoint_list[i].righ,endpoint_list[i].righ-endpoint_list[i].left);
        }
            cal_area(source,nbins,endpoint_list, nfound,area,Va);
            for(int i=0;i<nfound;i++)
            {
                printf("%6d\t%6d\t%15.6lf\t%10.6lf\n",i,xpeaks[i],area[i],Va[i]);
            }
    */
    
/*    TPolyMarker * pm = (TPolyMarker*)h->GetListOfFunctions()->FindObject("TPolyMarker");
    if (pm)
    {
        h->GetListOfFunctions()->Remove(pm);
        delete pm;
        h->GetListOfFunctions()->Remove(pm);
        delete pm;
    }
    pm = new TPolyMarker(nfound, fPositionX, fPositionY);
    h->GetListOfFunctions()->Add(pm);
    pm->SetMarkerStyle(23);
    pm->SetMarkerColor(kRed);
    pm->SetMarkerSize(1.3);

    TH1F *d7=s->Background(h, 8 );
    d7->Draw("SAME");

//Peak fitting
    Bool_t *FixPos = new Bool_t[nfound];
    Bool_t *FixAmp = new Bool_t[nfound];
    for(i = 0; i< nfound ; i++)
    {
        FixPos[i] = kTRUE;
        FixAmp[i] = kTRUE;
    }
    Float_t *PosX=fPositionX;
    Float_t *PosY=fPositionY;

    TSpectrumFit *pfit=new TSpectrumFit(nfound);
    pfit->SetFitParameters((Int_t)xmin, (Int_t)xmax-1, 3, 0.001, pfit->kFitOptimChiCounts, pfit->kFitAlphaHalving, pfit->kFitPower2, pfit->kFitTaylorOrderFirst);
    pfit->SetPeakParameters(3, kFALSE, PosX, (Bool_t *) FixPos, PosY, (Bool_t *) FixAmp);

    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    pfit->FitAwmi(source);

    //pfit->FitStiefel(source);
    Double_t *CalcPositions;// = new Double_t[nfound];
    Double_t *CalcAmplitudes;// = new Double_t[nfound];
    CalcPositions=pfit->GetPositions();
    CalcAmplitudes=pfit->GetAmplitudes();
    for (i = 0; i < nbins; i++) d->SetBinContent(i + 1,source[i]);
    d->SetLineColor(kRed);
    d->Draw("SAME L");

        for (i = 0; i < nfound; i++) {
            a=CalcPositions[i];
            bin = 1 + Int_t(a + 0.5);
            PosX[i] = h->GetBinCenter(bin);
            PosY[i] = h->GetBinContent(bin);
        }
    */
    /*
        for (i = 0; i < nbins; i++) d1->SetBinContent(i + 1,dest[i]);
        d1->SetLineColor(kRed);
        d1->Draw("SAME");

        for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
        s->Background(source,nbins,48,s->kBackDecreasingWindow,s->kBackOrder8,kTRUE,
                      s->kBackSmoothing5,kFALSE);
        for (i = 0; i < nbins; i++) d2->SetBinContent(i + 1,source[i]);
        d2->SetLineColor(kBlack);
        d2->Draw("SAME");

        float *ff= new float[nbins];

        for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
        if(smooth(source,ff,nbins)<0) printf("smaooth failed!\n");
        else {
            for (i = 0; i < nbins; i++) d5->SetBinContent(i + 1,ff[i]);
            d5->SetLineColor(kBlack);
            d5->Draw("SAME");
        }
    */

   /*
    func2();
    TCanvas *c2 = new TCanvas("c2","c2",10,10,1000,600);
    TH1F *d7 = new TH1F("d7","",nbins,xmin,xmax);
    TH1F *d8 = new TH1F("d7","",nbins,xmin,xmax);
    for(int i=0; i<pcaheader_o.chn_number; i++) d7->SetBinContent(i+1,ptra_t[i]);
    for(int i=0; i<pcaheader_o.chn_number; i++) d8->SetBinContent(i+1,ptra[i]);
    d7->SetLineColor(kRed);
    d7->Draw("L");
    d8->Draw("SAME L");
    */
}
