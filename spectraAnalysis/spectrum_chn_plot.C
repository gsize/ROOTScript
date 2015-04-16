
// ROOT style
// gROOT->Reset();
#include "TAxis.h"
#include "TH1.h"
#include "TArrayD.h"

#define long_4 int

Double_t ScaleX(Double_t x)
{
	Double_t v;//0.171,0.136,-2.758e-9
	v = 0.157 +  0.136 * x  -3.172E-09*x*x; // "linear scaling" function example
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

/*
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

double eff_fun(double *x,double *par)
{
	double eff=0;
	for(int i=1;i<7;i++)
	{
		eff += par[i-1]*TMath::Power(x[0],2-i);
	}
	return (TMath::Exp(eff));
}
void ReadTag()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("spectrum_chn_plot.C","");
	dir.ReplaceAll("/./","/");

	ifstream in;
	in.open(Form("%stagList.txt",dir.Data()));
	if(!(in.is_open()))
	{
		printf("open file failed!");
		return;
	}

}
void DrawTag(TH1 *h, double xTag, TString strTag)
{
double xMin= h->GetXaxis()->GetXmin();
double xMax= h->GetXaxis()->GetXmax();

        if(xTag > xMin && xTag < xMax)
{
		TLatex *tex = new TLatex(xTag,h->GetBinContent(h->FindBin(xTag)),strTag.Data());
		tex->SetTextSize(0.02688601);
		tex->SetLineWidth(2);
		//tex->SetTextAlign(12);
		tex->SetTextAngle(90.);
		tex->Draw();
}
}

void spectrum_chn_plot()
{

	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("spectrum_chn_plot.C","");
	dir.ReplaceAll("/./","/");
	TString pathFile;
	pathFile = dir + "abbb1_08.Chn";

	int r=rdspm_o(pathFile.Data());
	if(r<0) return;

	Int_t nbin=pcaheader_o.chn_number;

	Int_t i,nfound,bin;
	Double_t nbins = nbin,a;
	Double_t xmin  = 0;
	Double_t xmax  = (Double_t)nbins;

	TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,600);

	TH1F *h = new TH1F("h"," "/*"^{230}Pa Spectra"*/,nbins,xmin,xmax);
	//TH1F *h_copy = new TH1F("h"," "/*"^{230}Pa Spectra"*/,nbins,xmin,xmax);
	h->SetXTitle("Channel");
	h->GetXaxis()->CenterTitle(1);
	h->SetYTitle("counts");
	h->GetYaxis()->CenterTitle(1);
	h->SetStats(0);

	gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);

	//for(int i=0; i<nbin; i++) h->SetBinContent(i+1,fast_log(ptra[i]));
	for(int i=0; i<nbin; i++){
		h->SetBinContent(i+1,ptra[i]);
		//h_copy->SetBinContent(i+1,ptra[i]);
	}
	h->GetXaxis()->SetRange(3000,7500);

	TPad *pad1 = new TPad("pad1","p1",0.5,0.6,0.85,0.9);;
	pad1->cd();
TH1F *h_copy = h->Clone();
	gPad->SetLogy(1);
	h_copy->GetXaxis()->SetRange(1,8192);
	h_copy->SetStats(0);
	h_copy->Draw("L");

	c1->cd();
	ScaleXaxis(h,ScaleX);
	h->Draw("L");

	float d_tmp[]={122.6 ,898, 60105,
		136.47 ,1004 , 7492,  //"^{57}Co"
		846.04 ,6233 , 194,   //"^{56}Co"
		811.85, 5972 ,245,
		158.38 ,1166 ,1188,  //"^{56}Ni"
		127.16 ,936, 11527, 
		1377.63, 10131, 1707,
		1919.52 ,14129 ,423, //"^{57}Ni"
		93.35, 687, 6407,
		443.74 ,3268 ,346,
		454.92 ,3350 ,405,
		898.66 ,6619 ,121,
		918.50, 6766 ,147,
		951.88 ,7011 ,444,  //"^{230}Pa"
		228.16, 1681 ,1087,  //"^{132}Te"
		667.71, 4914,407,
		772.6,5686,265,  //"^{132}I"
		364.48, 2680 , 1127, //"^{131}I"
		511.0,0,0
	} ;

	TString iso_name[]={"^{57}Co","^{57}Co","^{56}Co","^{56}Ni","^{56}Ni","^{57}Ni","^{57}Ni","^{57}Ni","^{230}Pa","^{230}Pa","^{230}Pa","^{230}Pa","^{230}Pa","^{230}Pa","^{132}Te","^{132}I","^{132}I","^{131}I"," "};

	TLatex *tex;
	TString str_tmp;
double xMin= h_copy->GetXaxis()->GetXmin();
double xMax= h_copy->GetXaxis()->GetXmax();
cout<<xMin<<":"<<xMax<<endl;
	for(int i=0;i< 19;i++)
	{
        if(d_tmp[i*3 ] > xMin && d_tmp[i*3 ] < xMax)
{
		str_tmp.Form("%.2fkeV %s",d_tmp[i*3 ], iso_name[i].Data());
		Int_t bin_tmp= (int)((d_tmp[i*3]-0.171)/0.136)+4;
		//tex = new TLatex(bin_tmp,h->GetBinContent(bin_tmp)+1,str_tmp.Data());
		tex = new TLatex(d_tmp[i*3],h->GetBinContent(h->FindBin(d_tmp[i*3])),str_tmp.Data());
		tex->SetTextSize(0.02688601);
		tex->SetLineWidth(2);
		//tex->SetTextAlign(12);
		tex->SetTextAngle(90.);
		tex->Draw();
}
	}
	pad1 ->Draw();

}
