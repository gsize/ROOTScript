
	// ROOT style
  gROOT->Reset();

#define long_4 int

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

void spectrum_chn_com()
{
  
    char *fname="lead_shield_background_2.Chn";
    char *fname1="xiaosuanyouxian.Chn";
    //char *fname="2N021.chn";

    int r=rdspm_o(fname);
    if(r<0) return;

    Int_t nbins=pcaheader_o.chn_number;


    Double_t xmin  = 0;
    Double_t xmax  = (Double_t)nbins;
    Float_t * dest = new float[nbins];
    //Float_t * background = new float[nbins];

    TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,600);
    TH1F *h = new TH1F("h1","background",nbins,xmin,xmax);
    TH1F *d = new TH1F("UO2(NO3)26H2O","uranyl nitrate hexahydrate",nbins,xmin,xmax);
	for(Int_t i=0; i<nbins; i++) h->SetBinContent(i+1,ptra[i]);
	int r=rdspm_o(fname1);
	for(Int_t i=0; i<nbins; i++) d->SetBinContent(i+1,ptra[i]);
	d->SetLineColor(kRed);
	//gPad->SetLogy();
d->GetXaxis()->SetTitle("channel");
d->GetXaxis()->CenterTitle();
   d->GetYaxis()->SetTitle("Count");
   d->GetYaxis()->CenterTitle();
	
	d->Draw();
	h->Draw("SAME");
}

