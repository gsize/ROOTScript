

/* ------------------ ORTEC PCAHEADER STRUCT ----------------------------------------------- */
typedef struct {
    /* byte offset. Description of the variable                        */
    short   leader;      /* must be -1                                                      */
    short   mca_number;  /* MCA number                                                      */
    short   seg_number;  /* Segment number                                                  */
    char  sec[2];      /* ASCII seconds of start time                                     */
    long  realtime;    /* Realtime (increments of 20 ms)                                  */
    long  livetime;    /* Livetime (increments of 20 ms)                                  */
    char  date[8];     /* Start date as ASCII DDMMMYY0, or NULL, if not known             */
    char  time[4];     /* Start time as ASCII HHMM, or NULL,if not known(see sec[2] above)*/
    short   chn_offset;  /* Channel offset of data                                          */
    short   chn_number;  /* Number of channels (length of data)                             */
} PCAHEADER_O;
/* ---------------------- PCAHEADER STRUCT ------------------------------------------------- */

/* ---------------------- ORTEC END STRUCT ------------------------------------------- */
typedef struct {
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
long *ptra;

int rdspm_o(const char* filename)
{
    short i;
    FILE* stream;
    //unsigned long ll;
    long ll;

    if(ptra) {
        delete [] ptra;
        ptra=NULL;
    }

    printf("filename:%s\n",filename);
    if ((stream = fopen(filename, "rb")) == NULL)  return (-1);
    //if (fread(&(pcaheader_o), sizeof(char), sizeof(PCAHEADER_O), stream) != sizeof(PCAHEADER_O))return(-1);

    fread(&(pcaheader_o.leader), sizeof(short), 1, stream);
    fread(&(pcaheader_o.mca_number), sizeof(short), 1, stream);
    fread(&(pcaheader_o.seg_number), sizeof(short), 1, stream);
    fread(pcaheader_o.sec, sizeof(char), 2, stream);
    fread(&(pcaheader_o.realtime), sizeof(long), 1, stream);
    fread(&(pcaheader_o.livetime), sizeof(long), 1, stream);
    fread(pcaheader_o.date, sizeof(char), 8, stream);
    fread(pcaheader_o.time, sizeof(char), 4, stream);
    fread(&(pcaheader_o.chn_offset), sizeof(short), 1, stream);
    fread(&(pcaheader_o.chn_number), sizeof(short), 1, stream);

    printf("2 chn_number=%d\n",pcaheader_o.chn_number);

    ptra= new long[pcaheader_o.chn_number];
    for (i = 0; i < pcaheader_o.chn_number; i++) {
        if (fread(&ll, sizeof(long), 1, stream) > 0) {
            ptra[i] = ll;
        }
    }
    //printf("2 is ok?\n");
    if (i != pcaheader_o.chn_number) {
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

int peaksearch(double *s,int num)
{
    int count=0;

    for(int i=0;i<num;i++)
    {
        W=a+b*sqrt(E);
        b_l=(s[i-W] + s[i-W+1] +s[i-W-1])/3;
        b_r = (s[i+W] + s[i+W+1] +s[i+W-1])/3;

       if( s[i]-b_l>2*abs(s[i-W] - b_l) && s[i]-b_l>2*abs(s[i+W] - b_r) && s[i]>=s[i+1] && s[i]>=s[i+1] )
       {
           count++;
       }
    }
    return count;
}



void FitAwmi()
{
    char *fname="D:\\root\\root_work_dir\\spectrum_analyse\\2N021.chn";
    int r=rdspm_o(fname);
    if(r<0) return;

    Double_t a;
    Int_t nbins=pcaheader_o.chn_number/4;
    Int_t xmin  = 0;
    Int_t xmax  = nbins;
    Int_t i,nfound,bin;
    Float_t * source = new float[nbins];
    Float_t * dest = new float[nbins];
    TH1F *h = new TH1F("h","Fitting using AWMI algorithm",nbins,xmin,xmax);
    TH1F *sm = new TH1F("smoothing","",nbins,xmin,xmax);
    TH1F *d = new TH1F("d","",nbins,xmin,xmax);
    TH1F *d1 = new TH1F("background","",nbins,xmin,xmax);
    for (i = 0; i < nbins; i++) h->SetBinContent(i + 1,ptra[i]);
    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    TCanvas *Fit1 = gROOT->GetListOfCanvases()->FindObject("Fit1");
    if (!Fit1) Fit1 = new TCanvas("Fit1","Fit1",10,10,1000,700);
    h->Draw("L");

    TSpectrum *s = new TSpectrum();

    Bool_t *FixPos = new Bool_t[nfound];
    Bool_t *FixAmp = new Bool_t[nfound];
    for(i = 0; i< nfound ; i++) {
        FixPos[i] = kFALSE;
        FixAmp[i] = kFALSE;
    }
//smoothing
    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    s->SmoothMarkov(source,nbins,3);  //3, 7, 10
    for (i = 0; i < nbins; i++) sm->SetBinContent(i + 1,source[i]);
    sm->SetLineColor(kBlack);
    sm->Draw("SAME L");


    //searching for candidate peaks positions
    nfound = s->SearchHighRes(source, dest, nbins, 2, 0.1, kFALSE, 10000, kFALSE, 0);
    printf("number of peaks:%d\n",nfound);


    //filling in the initial estimates of the input parameters
    //Float_t *PosX = new Float_t[nfound];
    Float_t *PosY = new Float_t[nfound];
    Float_t *PosX= s->GetPositionX();
    for (i = 0; i < nfound; i++) {
        a=PosX[i];
        bin = 1 + Int_t(a + 0.5);
        PosY[i] = h->GetBinContent(bin);
    }
    //fit background
    for (i = 0; i < nbins; i++) source[i]=h->GetBinContent(i + 1);
    s->Background(source,nbins,6,s->kBackDecreasingWindow,s->kBackOrder2,kTRUE,
                  s->kBackSmoothing3,kTRUE);
    for (i = 0; i < nbins; i++) d1->SetBinContent(i + 1,source[i]);
    d1->SetLineColor(kGreen);
    d1->Draw("SAME L");

    TSpectrumFit *pfit=new TSpectrumFit(nfound);
    pfit->SetFitParameters(xmin, xmax-1, 1000, 0.1, pfit->kFitOptimChiCounts, pfit->kFitAlphaHalving, pfit->kFitPower2, pfit->kFitTaylorOrderFirst);
    pfit->SetPeakParameters(2, kFALSE, PosX, (Bool_t *) FixPos, PosY, (Bool_t *) FixAmp);

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
        PosX[i] = d->GetBinCenter(bin);
        PosY[i] = d->GetBinContent(bin);
    }
    TPolyMarker * pm = (TPolyMarker*)h->GetListOfFunctions()->FindObject("TPolyMarker");
    if (pm) {
        h->GetListOfFunctions()->Remove(pm);
        delete pm;
    }
    pm = new TPolyMarker(nfound, PosX, PosY);
    h->GetListOfFunctions()->Add(pm);
    pm->SetMarkerStyle(23);
    pm->SetMarkerColor(kRed);
    pm->SetMarkerSize(1);
}
