#include "TAxis.h"
#include "TH1.h"
#include "TArrayD.h"
//#include <map>

int  GetGammaRay(int endfcode, double ac, std::vector<double> &energy, std::vector<double> &intensity );
class Spectra {
	public:
		Spectra();
		~Spectra();

		int Read(const TString filename);
		void Plot();
		TH1F *GetTH1();
		void Print();
		void Reset();
		void SetPathDir(const TString path){pathDir = path;};
		void FitAlpha();

	private:
		int ReadChn();  //ORTEC spectra .chn
		int ReadSpc();//ORTEC spectra  .spc
		int ReadCnf(); //Canberra spectra .cnf
		int ReadIEC();
		TString FormatDateTime(char *yearStr,char *monthStr,char *dayStr,const char *timeStr);

	private:
		std::vector<int> counts;
		int channels;
		double energyCalPar[3];
		double liveTime;
		double realTime;
		TDatime measureTime;

		TString fileName;
		TString spectraType;
		TString pathDir;
		char * metaData;

		UInt_t fileSize;
};

Spectra::Spectra()
{
	liveTime=0.;
	realTime=0.;
}

Spectra::~Spectra()
{
	//	delete metaData;

}
void Spectra::Reset()
{
	counts.clear();
	channels=0;
	liveTime=0.;
	realTime=0.;

	fileName.Clear();
	spectraType.Clear();
	pathDir.Clear();
};


void Spectra::Print()
{
	printf("FileName:%s\n",fileName.Data());
	printf("SpectraType:%s\n",spectraType.Data());
	printf("Start Acquite ");
	measureTime.Print();
	printf("Real Time:%.3lf\t",realTime);
	printf("Live Time:%.3lf\t",liveTime);
	printf("Dead Time:%5.2lf%%\n",100. * (realTime-liveTime)/realTime);
	printf("Channel Number:%d\n",channels);
	printf("Energy Calibration: E = %.3lf + %.3lf*chn +%.3lf*chn^2\n",energyCalPar[0],energyCalPar[1],energyCalPar[2]);

}

void Spectra::Plot()
{
	TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,600);
	TH1F *th1;
	th1 = GetTH1();
	gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	th1->Draw();
}

TH1F *Spectra::GetTH1()
{
	double xmin=0.;
	double xmax;
	xmax=(double)channels;
	int nbins=channels;

	TH1F *h = new TH1F("h"," gamma Spectra",nbins,xmin,xmax);
	h->SetXTitle("Channel");
	h->GetXaxis()->CenterTitle(1);
	h->SetYTitle("counts");
	h->GetYaxis()->CenterTitle(1);
	h->SetStats(0);

	for(int i=0; i<nbins; i++){
		h->SetBinContent(i+1,counts[i]);
	}
	//printf("channel 1000: %lf",h->GetBinContent(1000));
	return h;;
}
int Spectra::Read(const TString filename)
{

	fileName = filename;

	ifstream input;
	TString filePath(Form("%s%s",pathDir.Data(),filename.Data()));
	input.open(filePath.Data(),ios::binary);
	if(!input.is_open())
	{
		cout<<"Can't open "<<filePath<<endl;
		return 0;
	}
	istream::pos_type current_pos = input.tellg();
	input.seekg(0,ios_base::end);
	istream::pos_type file_size = input.tellg();
	input.seekg(current_pos);
	metaData= new Char_t[file_size];
	memset(metaData,0,file_size);
	fileSize = (UInt_t)file_size;

	input.read(metaData,file_size*sizeof(Char_t));
	input.close();

	Short_t d1,d2;
	memcpy(&d1,metaData,sizeof(Short_t));
	memcpy(&d2,metaData+2,sizeof(Short_t));
	if(d1 == -1)
	{
		spectraType.Form("CHN");
		ReadChn();
	}
	if(d1==1 &&d2==1)
	{
		spectraType.Form("SPC");
		ReadSpc();
	}
	if(filename.Contains(".cnf"))
	{
		spectraType.Form("CNF");
		ReadCnf();
	}
	if(filename.Contains("IEC"))
	{
		spectraType.Form("IEC");
		ReadIEC();
	}

	delete [] metaData ;

	return 1;
}

TString Spectra::FormatDateTime(char *yearStr,char *monthStr,char *dayStr,const char *timeStr)
{
	char month[12][4] = {"Jan","Feb","Mar","Apr",
		"May","Jun","Jul","Aug",
		"Sep","Oct","Nov","Dec" } ;
	int nMonth=0;
	TString monthStr1(monthStr);
	while(!monthStr1.Contains(month[nMonth]) && nMonth<11)  nMonth++;
	nMonth++;
	TString dataTimeStr;
	if(yearStr[3]=='1')
	{
		yearStr[3]='\0';
		dataTimeStr.Form("20%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr);
	}
	else
	{
		yearStr[3]='\0';
		dataTimeStr.Form("19%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr);
	}
	return dataTimeStr;
}


int Spectra::ReadCnf()
{
	int shortSize=2;
	int floatSize=4;
	int intSize=4;
	int doubleSize=8;

	unsigned int acqOffset=0, samOffset=0,effOffset=0,encOffset=0,chanOffset=0;
	const char *beg = metaData;
	const char *end =beg + fileSize;
	for(const char* ptr= beg+ 112; ptr +48 <end;ptr+= 48)
	{
		unsigned int offset=0;
		memcpy(&offset,ptr+10,intSize);
		if((ptr[1] == 0x20 && ptr[2] == 0x01) || ptr[1] == 0 ||ptr[2]==0)
		{
			switch(ptr[0]){
				case 0:
					if(acqOffset == 0)
						acqOffset = offset;
					else
						encOffset = offset;
					break;
				case 1:
					if(samOffset == 0)
						samOffset = offset;
					break;
				case 2:
					if(effOffset == 0)
						effOffset = offset;
					break;
				case 5:
					if(chanOffset == 0)
						chanOffset = offset;
					break;
				default:
					break;

			}
			if(acqOffset !=0 && samOffset!=0  && effOffset!=0 && chanOffset!=0)
				break;
		}
	}
	if(encOffset == 0)
		encOffset = acqOffset ;

	const char* acqPtr = beg +acqOffset;
	short offset1,offset2,chn;

	memcpy(&offset1,acqPtr+34,shortSize);
	memcpy(&offset2,acqPtr+36,shortSize);
	memcpy(&chn,acqPtr+48+128+10,shortSize);
	channels = chn* 256;	

	//Get energy calibration
	float energy_par_t;
	memcpy(&energy_par_t,beg+encOffset+80 +36 ,floatSize);
	energyCalPar[0]=energy_par_t;
	memcpy(&energy_par_t,beg+encOffset+80 +40,floatSize);
	energyCalPar[1]=energy_par_t;
	memcpy(&energy_par_t,beg+encOffset+80 +44,floatSize);
	energyCalPar[2]=energy_par_t;

	const char* datePtr = acqPtr+48+offset2+1;
	ULong64_t da;
	memcpy(&da,datePtr,8);
	//takes it to 17 Nov 1858, base of modified Julian Calendar ,change to 1970-jan-1
	UInt_t t =(UInt_t) (da/10000000 - 3506716800u);
	//printf("datetime:%ld time_t:%d\n",da,t);
	measureTime.Set(t);

	memcpy(&da,datePtr+8,8);
	realTime = (~da)*1.0e-7f;
	memcpy(&da,datePtr+16,8);
	liveTime = (~da)*1.0e-7f;

	const char *chanPtr = beg+chanOffset;
	UInt_t *spectraData = new UInt_t[channels]; 
	memcpy(spectraData ,chanPtr+512,4*channels);
	for(int i=0; i<channels; i++)
	{
		counts.push_back(spectraData[i]);
	}
	delete [] spectraData ;

	return 1;
}

int Spectra::ReadChn()
{
	int shortSize=2;
	int floatSize=4;
	int doubleSize=8;
	//Get real time and live time
	int RT,LT;
	memcpy(&RT,metaData+8,4);
	realTime= 0.02 *RT;
	memcpy(&LT,metaData+12,4);
	liveTime= 0.02 *LT;
	//start Acquire Date
	char hourStr[3],minuteStr[3],secondStr[3];
	memset(hourStr,0,3);
	memset(minuteStr,0,3);
	memset(secondStr,0,3);
	memcpy(secondStr,metaData+6,2);
	memcpy(hourStr,metaData+24,2);
	memcpy(minuteStr,metaData+26,2);
	TString timeStr;
	timeStr.Form("%s:%s:%s",hourStr,minuteStr,secondStr);
	//start Acquire Date
	char monthStr[4],dayStr[3],yearStr[4];
	memset(monthStr,0,4);
	memset(dayStr,0,3);
	memset(yearStr,0,4);
	memcpy(dayStr,metaData+16,2);
	memcpy(monthStr,metaData+18,3);
	memcpy(yearStr,metaData+21,3);

	TString dataTimeStr;	
	dataTimeStr = FormatDateTime(yearStr,monthStr,dayStr,timeStr.Data());

	measureTime.Set(dataTimeStr.Data());
	//measureTime.Print();

	short chn;
	memcpy(&chn,metaData+30,2);
	channels =(int)chn;
	unsigned int *spectraData = new unsigned int[chn]; 
	memcpy(spectraData,metaData+32,4*chn);
	for(int i=0; i<channels; i++)
	{
		counts.push_back(spectraData[i]);
	}
	delete [] spectraData ;

	//Get energy calibration
	float energy_par_t;
	memcpy(&energy_par_t,metaData+32+4*chn+4,floatSize);
	energyCalPar[0]=energy_par_t;
	memcpy(&energy_par_t,metaData+32+4*chn+8,floatSize);
	energyCalPar[1]=energy_par_t;
	memcpy(&energy_par_t,metaData+32+4*chn+12,floatSize);
	energyCalPar[2]=energy_par_t;

	return 1;
}

int Spectra::ReadSpc()
{
	int shortSize=2;
	int floatSize=4;
	int doubleSize=8;
	int sectionSize=128;

	short	ACQIRP,CALRP1,SPCTRP,SPCRCN,SPCCHN;
	double RLTMDT, LVTMDT;
	memcpy(&ACQIRP,metaData+8,shortSize);
	memcpy(&CALRP1,metaData+34,shortSize);
	memcpy(&SPCTRP,metaData+60,shortSize);
	memcpy(&SPCRCN,metaData+62,shortSize);
	memcpy(&SPCCHN,metaData+64,shortSize);

	//Get RealTime and LiveTime
	float d1;
	memcpy(&d1,metaData+90,floatSize);
	realTime= (double)d1;
	memcpy(&d1,metaData+94,floatSize);
	liveTime= (double)d1;

	channels= SPCCHN;
	unsigned int *spectraData = new unsigned int[channels]; 
	//	memcpy(spectraData,metaData+(SPCTRP-1)*128,SPCRCN*128);
	memcpy(spectraData,metaData+(SPCTRP-1)*sectionSize,channels*4);
	//printf("channel 976:%d\n",spectraData[976]);
	for(int i=0; i<channels; i++)
	{
		counts.push_back((int)spectraData[i]);
	}
	delete [] spectraData ;
	//Short File Name
	char ACQStr[129];
	memset(ACQStr,0,129);
	memcpy(ACQStr,metaData + sectionSize*(ACQIRP-1),sectionSize);
	//Measure DateTime
	char monthStr[4],dayStr[3],yearStr[4];
	memset(monthStr,0,4);
	memset(dayStr,0,3);
	memset(yearStr,0,4);
	memcpy(dayStr,ACQStr+16,2);
	memcpy(monthStr,ACQStr+19,3);
	memcpy(yearStr,ACQStr+23,3);
	char timeStr[11];
	memset(timeStr,0,11);
	memcpy(timeStr,ACQStr+28,10);

	TString dataTimeStr;	
	dataTimeStr = FormatDateTime(yearStr,monthStr,dayStr,timeStr);

	measureTime.Set(dataTimeStr.Data());

	char CALRP1Str[129];
	memset(CALRP1Str,0,129);
	memcpy(CALRP1Str,metaData + sectionSize*(CALRP1-1),sectionSize);

	//Get energy calibration
	float energy_par_t;
	memcpy(&energy_par_t,CALRP1Str+20,floatSize);
	energyCalPar[0]=energy_par_t;
	memcpy(&energy_par_t,CALRP1Str+24,floatSize);
	energyCalPar[1]=energy_par_t;
	memcpy(&energy_par_t,CALRP1Str+28,floatSize);
	energyCalPar[2]=energy_par_t;
	//	printf("%s  %s  %s\n",CALDESStr,CALRP1Str,CALRP2Str);

	return 1;
}

int Spectra::ReadIEC()
{
	return 1;
}

void Spectra::FitAlpha()
{
	TH1F *th;
	th = GetTH1();
	TSpectrum *sp = new TSpectrum();
	Int_t nfound = sp->Search( th ,2,"nodraw");
	int p1 = (sp->GetPositionX())[0];
	double xMin , xMax;
	xMin=p1-100 ;xMax=p1+100;
	for(int i=0;i<nfound;i++)
	{
		int p = (sp->GetPositionX())[i];
		int a = th->GetBinContent(p1) ;
		printf("No. %d  peaks:%d\n",p,a );
	}
	TF1 *lg1 = new TF1("lg","[0]*TMath::Gaus(x,[1],[2])+[3]*TMath::Landau(-x+[4],[5],[6])", xMin, xMax);
	lg1->SetParameters( th->GetBinContent(p1),  p1, 4.6, th->GetBinContent(p1),  2.* p1,  p1, 5);
	th->Fit("lg","R+");

	delete sp;

	TCanvas *c2 = new TCanvas("c2","c2",10,10,1000,600);
	gStyle->SetOptFit(1111);
	gPad->SetLogy(1);
	gPad->SetGridy(1);
	gPad->SetGridx(1);
	th->Draw();
}

void DrawTag(TH1 *h, double xTag, TString strTag)
{
	double xMin= h->GetXaxis()->GetXmin();
	double xMax= h->GetXaxis()->GetXmax();

	if(xTag > xMin && xTag < xMax)
	{
		TLatex *tex = new TLatex(xTag,1.1 * h->GetBinContent(h->FindBin(xTag)),strTag.Data());
		tex->SetTextSize(0.03688601);
		tex->SetLineWidth(2);
		//tex->SetTextAlign(12);
		tex->SetTextAngle(90.);
		tex->Draw();
	}
}

short fast_log(unsigned int count)
{
	short l,i;
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

Double_t ScaleX(Double_t x)
{ 
	//Double_t v;//0.171,0.136,-2.758e-9
	//v = 0.157 +  0.136 * x  -3.172E-09*x*x; // "linear scaling" function example
	//v = 0.263 +  0.366 * x  -0.000*x*x; // "linear scaling" function example
	TF1 *f = new TF1("FWHM","pol2",0.05,1500);
	f->SetParameters(0.263,0.366,0.000);
	return f->Eval(x);
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
   hist1->ResetStats();
   */

int elemNR2ENDFCode(TString elementName)
{
	int ENDFCode;

	TPRegexp re("^[A-Z][a-z]?-\\d+m?");
	TPRegexp re1("^[A-Z][a-z]?-\\d+");
	TPRegexp re2("^[A-Z][a-z]?-\\d+m$");

	TGeoElementTable *table = gGeoManager->GetElementTable();
	if(elementName.Contains("-"))
		elementName.ReplaceAll("-","");
		
	if(elementName.Contains(re))
	{
		int a=0,z=0,iso=0;
		char str[4];

		sscanf(elementName.Data(),"%[A-Za-z]%d",str,&a);
		TGeoElement *elem = table->FindElement(str);
		z = elem->Z();

		if(elementName.Contains(re2))
			iso=1;
		else
			iso=0;

		ENDFCode = 10000*z+10*a+iso;
	}
	else
		ENDFCode=0;

	return ENDFCode;
}


int  GetGammaRay(int endfcode, double ac, std::vector<double> &energy, std::vector<double> &intensity )
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("readSpectra.C","");
	dir.ReplaceAll("/./","/");

	TString dbname(Form("sqlite://%s../../NuclearDatabase/nucleidata.sqlite",dir.Data()));
	TSQLiteServer *f = new TSQLiteServer(dbname.Data());

	TSQLiteRow *row;
	TSQLiteResult *res;
	int nfield;
	int hasgammaray=0;
	TString sql(Form("select * from rayInfor where ENDFCode=%d and rayType = \"G\" ",endfcode));
	res = (TSQLiteResult*) f->Query(sql.Data());
	if(res==0)
	{
		f->Close();
		hasgammaray = -1;
	}
	else
	{
		nfield = res->GetFieldCount();
		while((row = (TSQLiteRow *)( res->Next())))
		{
			TString str(row->GetField(3));
			energy.push_back(str.Atof());
			str= row->GetField(2);
			intensity.push_back(str.Atof());
			delete row;
		}
		delete res;
	}
	f->Close();
	hasgammaray=energy.size();
	return hasgammaray;
}

TH1* plotSpectra(TH1 *hs1,double xmin, double xmax)
{
	int num =8;
	TString nuclearName[]={"Pb-211","Th-227","Rn-219","Ac-227","Pa-231","Bi-211","Tl-207","Ra-223"};
	vector<double> gammaRay;
	vector<double> intensity;
	//	cSp->cd(i+1);
	TCanvas *cSp = new TCanvas(Form("gamma%.0lf",xmin),Form("gamma %.0lf",xmin),10,10,1000,600);
	
	TH1 *hs=(TH1 *)hs1->Clone("PartOfH");
	hs->Draw();
	hs->GetXaxis()->SetRangeUser(xmin,xmax);
	hs->GetXaxis()->SetTitle("energy/keV");
	hs->GetXaxis()->CenterTitle(1);
	hs->GetYaxis()->SetTitle("counts");
	hs->GetYaxis()->CenterTitle(1);
	gPad->SetGridx(1);
	gPad->SetGridy(1);
	//	gPad->SetLogy(1);

	for(int j=0;j<num;j++)
	{
		int endf = elemNR2ENDFCode(nuclearName[j]);
		if(GetGammaRay(endf,1.,gammaRay,intensity) < 1)
		{
			printf("don't have nuclear %d\n",endf);
			break;
		}
		double cutSet = 0.002;
		if(nuclearName[j].Contains("Th-227"))
			cutSet=0.008;
		if(nuclearName[j].Contains("Pa-231"))
			cutSet=0.01;
		for(int k=0;k<gammaRay.size();k++)
		{
			double e = gammaRay[k]*1000.+1;

			if(e > 100. && e > xmin&& e < xmax  && intensity[k] > cutSet )
			{
				TString str;
				str.Form("%s, %.3lf",nuclearName[j].Data(),100.*intensity[k]);
				//str.Form("%s",nuclearName[j].Data());
				DrawTag(hs,e,str);
			}
		}
		gammaRay.clear();
		intensity.clear();
	}
	return hs;
}

int plotSpectraAll(TH1 *hs1)
{
	int n=3;
	double length[3]={200,180,600};
	double offset=100.;

	for(int i=0;i<n;i++)
	{

		plotSpectra(hs1,offset,offset+length[i]);
		offset += length[i];
	}
	return 0;
}

double GetArea( TH1 *h,int bin,int winbin)
{
	double area=0.;
	int halfWin = TMath::CeilNint(0.5*winbin);
	area = h->Integral(bin-halfWin ,bin+halfWin );
	return (area );
}
double GetBackgroundByBin( TH1 *h, int bin,int winbin)
{	
	double bgArea=0.;
	TH1* bg = h->ShowBackground(20,"compton");
	bgArea = GetArea(bg,bin,winbin);
	return (bgArea );
}

double GetBackgroundByEnergy( TH1 *h, double energy,int winbin)
{
	int bin = h->FindBin(energy);
	double bgArea =0.;
	bgArea = GetBackgroundByBin(h,bin,winbin);
	return (bgArea);
}

double GetNetArea( TH1 *h, double energy,int winbin)
{
	int bin = h->FindBin(energy);
	double bgArea=0.;
	double grossArea=0.;
	bgArea =GetBackgroundByBin(h,bin,winbin);
	grossArea =GetArea(h,bin,winbin);
	return (grossArea -bgArea);
}

int calPa231Datime(TH1 *h)
{
	double xmin = 319.64;
	double xmax =356.22 ;

	//TH1F *hs = h->Clone();
	TH1 *hs = plotSpectra(h, xmin,xmax);
	gStyle->SetOptStat(kFALSE);
	//hs->GetXaxis()->SetRangeUser(xmin,xmax);
	TSpectrum *s = new TSpectrum();
	int nfound = s->Search(hs,3,"nodraw",0.051);
	TH1 *hb = s->Background(hs,30,"same");

	double r1[5] = {119.86,41.83,34.76,21.59,8.61};
	double rate[5],ene[5];
	double i_Ra_324 = 3.99,i_Th_324 = 0.01;
	double i_Ra_330 = 0.209,i_Th_330 = 2.9,i_Pa_330 = 1.396;
	double i_Ra_334 = 0.101,i_Th_334 = 1.14;
	double i_Ra_338 = 2.84;
	for(int i=0;i<nfound;i++)
	{
		ene[i] = (double)(s->GetPositionX())[i];
		rate[i] = GetNetArea( hs,  ene[i],14);
		cout<<ene[i]<<" area: "<<rate[i];
		rate[i]/=495.6;

		cout<<" rate: "<<rate[i]<<endl;
		rate[i] = r1[i];
	}
	for(int i=2;i<5;i++)
	{
		double i_k = 0;
		if(i==4) i_k = i_Ra_334 +i_Th_334  ;
		if(i==2) i_k = i_Ra_324 +i_Th_324  ;
		if(i==3) i_k = i_Ra_338 ;
		double Ai = rate[i]/i_k;
		double k =1./( (rate[1]/Ai-(i_Th_330 + i_Ra_330))/i_Pa_330);
		double y = 22.77*TMath::Log2(1./(1-k));
		//double y = -TMath::Log(1-k)*21.77/0.693;
		cout<<ene[i]<<" k= "<<k<<" y="<<y<<endl;
	}
	return 1;
}

void readSpectra(TString fName = "lead_shield_background_2.Chn")
{
	TGeoManager *geom = new TGeoManager("","");
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("readSpectra.C","");
	dir.ReplaceAll("/./","/");

	//TString fNames("lead_shield_background_2.Chn");
	//TString fNames[]={"i1-bg-pump.Chn","i1_p7-s1-U-pump.Spc","i1-s2-dianchenji_U-pump_p1.Spc"};
	//TString fNames[]={"i1-p3_s5-dianchenji_U-pump_1.Spc"
					//	,"i1-p3_ds6-dianchenji_U-pump_1.Spc"
					//	,"i1-p3_ds7-dianchenji_U-pump_1.Spc"
					//	,"i1-p3_ds8-dianchenji_U-pump_1.Spc"
					//	,"i1-p3_ds9-dianchenji_U-pump_1.Spc"
					//	};
	//TString fNames[]={"Test_spc.cnf","Naidemo.cnf"};

	TString fNames[]={"8-19-pa-231.Spc"};
	int numSpc = 1;
	TH1F *th1;
	Spectra *sp ;
	TString pDir;
	pDir.Form("%s../../spectrum/yuanku_pa231/",dir.Data());
	THStack *hs = new THStack("hs","Alpha Spectra");
	for(int i=0;i<numSpc;i++)
	{
		sp= new Spectra();
		sp->SetPathDir(pDir);
		sp->Read(fNames[i]);
		th1=sp->GetTH1();
		ScaleXaxis(th1, ScaleX);
		hs->Add(th1);
	}
	//hs->Draw("nostack,elp");

//	plotSpectraAll(th1);
	calPa231Datime(th1);

}
