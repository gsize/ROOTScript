/********************************************************************/
//
//
//Draw the plots of the spectrum get at Xi'an at March 2012.
//
//
//
//
//
/********************************************************************/
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

int smooth1(float *s,float *d,int num)
{
    if(num<3)
    {
        for(int i=2; i<num-2; i++)
        {
            d[i]=(-3*(s[i+2]+s[i-2]) + 12*(s[i+1]+s[i-1])+17*s[i])/35;
        }
        return 0;
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
        return 0;
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

int rdspm_spc(const char* filename)
{
	Int_t chn_num=16384;
	char *buf=new char[9600];
	long long kk;
    Int_t words=8;
    FILE* stream;
    //unsigned long_4 ll;
    long_4 ll;

	/*fread(&(kk), sizeof(short), 1, stream);
    printf("%d\n",kk);
    fread(&(kk), sizeof(short), 1, stream);
    printf("%d\n",kk);

    fread(buf, sizeof(char), 6144, stream);
	//printf("short:%d\tint:%d\tlong long:%d\tlong_4:%d\n",sizeof(short),sizeof(int),sizeof(long long),sizeof(long_4));
    */

    if(ptra)
    {
        delete [] ptra;
        ptra=NULL;
    }
	ptra = new long_4[chn_num];
    //printf("int long=%d\n",sizeof(Int_t));
    if ((stream = fopen(filename, "rb")) == NULL)  return (-1);

    fread(buf, sizeof(char), 6144, stream);
    //fread(buf, sizeof(char), 9600, stream);

	for (Int_t i=0;i<chn_num;i++)
	{
    	fread(&(kk), sizeof(int), 1, stream);
     	ptra[i]=kk;
    }
    fclose(stream);
    return 0;
}

void Draw_spectrum()
{
	
}

void spectrum_spc()
{
	TString name_spectrum_p4[]={"p4-24-01","p4-24-02","p4-24-03"
								,"p4-24-04","p4-24-05","p4-24-06"
								,"p4-24-07"
							};
	TString name_spectrum_p5[]={"p5-1-24-01","p5-1-24-02","p5-1-24-03","p5-1-24-04"
								,"p5-1-24-05","p5-1-24-06"
							};
	TString path_spectrum("/home/gsz/work_dir/SPECTRUM/sinap4.12/");

	Int_t chn_num=16384;
	
	TH1F *th[7];
	TLegend* leg[7];
	TCanvas *c2[4];
	
	for(Int_t i=0;i<6;i++)
	{
		TString filename = path_spectrum + name_spectrum_p5[i] + ".Spc";
		printf("%s\n",filename.Data());
		rdspm_spc(filename.Data());
		
		if(i%2==0){
			c2[i/2] = new TCanvas(name_spectrum_p5[i].Data(),name_spectrum_p5[i].Data(),10,10,1000,600);
    		c2[i/2]->Divide(1,2);
		}
		c2[i/2]->cd(i%2+1);
		
   		gStyle->SetOptStat(kFALSE);
   		gPad->SetLogy();
		leg[i] = new TLegend(0.7, 0.7, 0.9, 0.9);
    	th[i] = new TH1F(name_spectrum_p5[i].Data(),name_spectrum_p5[i].Data(),chn_num,0,chn_num);
    	for (Int_t j=0;j<chn_num;j++)
		     th[i]->SetBinContent(j+1,ptra[j]);

	    th[i]->GetXaxis()->SetTitle("Channal");
		th[i]->GetXaxis()->CenterTitle();
		th[i]->GetYaxis()->SetTitle("Count");
		th[i]->GetYaxis()->CenterTitle();
		
		leg[i]->AddEntry(th[i], name_spectrum_p5[i].Data(), "p");
		//if(i%2==0)
		//{
			//th[i]->SetLineColor(48);
			th[i]->Draw();
		//}else
		//{
		//	th[i]->Draw("same");
			
		//}
		leg[i]->Draw();
	}

	delete [] ptra;

}
