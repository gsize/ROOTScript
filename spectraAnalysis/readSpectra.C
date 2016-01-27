#include "TAxis.h"
#include "TH1.h"
#include "TArrayD.h"
//#include <map>

void ScaleAxis(TAxis *a, TF1 *scaleFun);
void ScaleXaxis(TH1 *h, TF1 *scaleFun);
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
		TF1* GetEnergyCalib(){return energyCalibrationFun;};

	private:
		int ReadChn();  //ORTEC spectra .chn
		int ReadSpc();//ORTEC spectra  .spc
		int ReadCnf(); //Canberra spectra .cnf
		int ReadIEC();
		TString FormatDateTime(char *yearStr,char *monthStr,char *dayStr,const char *timeStr);
		void UpdateEnergyCalibrationFun();

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
		TF1* energyCalibrationFun;
};

Spectra::Spectra()
{
	liveTime=0.;
	realTime=0.;
	energyCalibrationFun = new TF1("EnergyCal","pol2",0.05,8192);
	energyCalibrationFun ->SetParameters(0.,1.,0.);
}

Spectra::~Spectra()
{
	//	delete metaData;
	delete energyCalibrationFun;

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
	delete energyCalibrationFun;
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
	printf("Energy Calibration: E = %.3E + %.3E*chn +%.3E*chn^2\n",energyCalPar[0],energyCalPar[1],energyCalPar[2]);

}

void Spectra::Plot()
{
	TCanvas *c1 = new TCanvas("spectra","spectra",10,10,1000,600);
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

	TH1F *h = new TH1F("h","Spectra",nbins,xmin,xmax);
	h->SetXTitle("Energy/keV");
	h->GetXaxis()->CenterTitle(1);
	h->SetYTitle("counts");
	h->GetYaxis()->CenterTitle(1);
	h->SetStats(0);

	for(int i=0; i<nbins; i++){
		h->SetBinContent(i+1,counts[i]);
	}
	UpdateEnergyCalibrationFun();
	ScaleXaxis(h,energyCalibrationFun );

	return h;;
}

void Spectra::UpdateEnergyCalibrationFun()
{
	energyCalibrationFun->SetRange(0.,channels);
	energyCalibrationFun->SetParameters(energyCalPar[0],energyCalPar[1],energyCalPar[2]);
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
		tex->SetTextSize(0.026);
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

void ScaleAxis(TAxis *a, TF1 *scaleFun)
{
	if (!a) return; // just a precaution
	if (a->GetXbins()->GetSize())
	{
		// an axis with variable bins
		// note: bins must remain in increasing order, hence the "Scale"
		// function must be strictly (monotonically) increasing
		TArrayD X(*(a->GetXbins()));
		for(Int_t i = 0; i < X.GetSize(); i++) X[i] = scaleFun->Eval(X[i]);
		a->Set((X.GetSize() - 1), X.GetArray()); // new Xbins
	}
	else
	{
		// an axis with fix bins
		// note: we modify Xmin and Xmax only, hence the "Scale" function
		// must be linear (and Xmax must remain greater than Xmin)
		a->Set(a->GetNbins(),
				scaleFun->Eval(a->GetXmin()), // new Xmin
				scaleFun->Eval(a->GetXmax())); // new Xmax
	}
	return;
}

void ScaleXaxis(TH1 *h, TF1 *scaleFun)
{
	if (!h) return; // just a precaution
	ScaleAxis(h->GetXaxis(), scaleFun);
	return;
}

int elemNR2ENDFCode(TString elementName)
{
	int ENDFCode;

	TPRegexp re("^[A-Za-z]{1,2}-?\\d+[Mm]?");
	TPRegexp re1("^[A-Za-z]{1,2}-?\\d+");
	TPRegexp re2("^[A-Za-z]{1,2}-?\\d+[Mm]$");

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

void readNuclideList( std::vector<string> &element, std::vector<double> &rate)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("readSpectra.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;

	pathName = dir + "nuclideList.txt";

	ifstream in;
	in.open(pathName.Data());
	if(!(in.is_open()))
	{
		printf("open file failed!");
		return;
	}

	string str_tmp;
	double rate_tmp;
	char elementSTR[9];
	TPRegexp re("[A-Za-z]{1,2}-?\\d{1,3}[Mm]?[ \t]+\\d[\\d.+\\-Ee]+");

	while(getline(in,str_tmp)) {
		TString str(str_tmp);
		if(str.Contains(re))
		{
			sscanf(str_tmp.c_str(),"%s%lg",elementSTR, &rate_tmp);
			string tmpSTR(elementSTR);
			element.push_back(tmpSTR);
			rate.push_back(rate_tmp);
		}
	}
}

TH1* plotGammaSpectra(TH1 *hs1,double xmin, double xmax)
{
	std::vector<string> nuclides;
	std::vector<double> rateCuts;

	readNuclideList( nuclides, rateCuts);

	vector<double> gammaRay;
	vector<double> intensity;
	//	cSp->cd(i+1);
	TCanvas *cSp = new TCanvas(Form("gamma%.0lf",xmin),Form("gamma %.0lf",xmin),10,10,1000,600);

	TH1 *hs = (TH1 *)hs1->Clone("PartOfH");
	hs->Draw();
	hs->GetXaxis()->SetRangeUser(xmin,xmax);
	hs->GetXaxis()->SetTitle("energy/keV");
	hs->GetXaxis()->CenterTitle(1);
	hs->GetYaxis()->SetTitle("counts");
	hs->GetYaxis()->CenterTitle(1);
	gPad->SetGridx(1);
	gPad->SetGridy(1);
	//	gPad->SetLogy(1);

	for(int j=0;j<nuclides.size();j++)
	{
		TString nuclearName(nuclides[j]);
		int endf = elemNR2ENDFCode(nuclearName);
		if(GetGammaRay(endf,1.,gammaRay,intensity) < 1)
		{
			printf("don't have nuclear %d\n",endf);
			break;
		}
		for(int k=0;k<gammaRay.size();k++)
		{
			double e = gammaRay[k]*1000.+1;

			if(e > 100. && e > xmin&& e < xmax  && intensity[k] > rateCuts[j] )
			{
				TString str;
				str.Form("%s, %.3lf",nuclearName.Data(),100.*intensity[k]);
				//str.Form("%s",nuclearName[j].Data());
				DrawTag(hs,e,str);
			}
		}
		gammaRay.clear();
		intensity.clear();
	}
	return hs;
}

TH1* plotAlphaSpectra(TH1 *hs1,double xmin, double xmax)
{
	TString nuclearName[]={"U-238","U-235","U-234"};
	Double energy[] = {4.18,4.4,4.78};

	TCanvas *cSp = new TCanvas(Form("Alpha%.0lf",xmin),Form("Alpha %.0lf",xmin),10,10,1000,600);

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
	double offset=250.;

	for(int i=0;i<n;i++)
	{

		plotGammaSpectra(hs1,offset,offset+length[i]);
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

void readSpectra(TString fName = "lead_shield_background_2.Chn")
{
	TGeoManager *geom = new TGeoManager("","");
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("readSpectra.C","");
	dir.ReplaceAll("/./","/");

	int numSpc = 1;
	//TString fNames[]={"i1-bg-pump.Chn","i1-p3-s2-dianchenji_U-pump.Spc","i1-s1-U-pump_1.Chn"};
	/*TString fNames[]={"i1-p3_s5-dianchenji_U-pump_1.Spc"
	  ,"i1-p3_ds6-dianchenji_U-pump_1.Spc"
	  ,"i1-p3_ds7-dianchenji_U-pump_1.Spc"
	  ,"i1-p3_ds8-dianchenji_U-pump_1.Spc"
	  ,"i1-p3_ds9-dianchenji_U-pump_1.Spc"
	  };
	  */
	//TString fNames[]={"Test_spc.cnf","Naidemo.cnf"};

	TString fNames[]={"8-19-pa-231.Spc"};
	THStack *hs = new THStack("hs","Spectra");
	TH1F *th1;
	Spectra *sp ;
	TString pDir;
	//pDir.Form("%s../../spectrum/alpha/gaosize/gsz20150327/",dir.Data());
	//pDir.Form("%s../../spectrum/GEM30P4-76-pl/gaosize/Re188/",dir.Data());
	pDir.Form("%s",dir.Data());

	for(int i=0;i<numSpc;i++)
	{
		sp= new Spectra();
		sp->SetPathDir(pDir);
		sp->Read(fNames[i]);
		th1 = sp->GetTH1();
		TF1 *f1;
		hs->Add(th1);
		sp->Print();
		sp->Plot();
	}
	if(0)
	{
		hs->Draw("nostack,elp");
		hs->GetXaxis()->SetTitle("Channel");
		hs->GetXaxis()->CenterTitle(1);
		hs->GetYaxis()->SetTitle("counts");
		hs->GetYaxis()->CenterTitle(1);
		gPad->SetGridx(1);
		gPad->SetGridy(1);
		gPad->SetLogy(1);
	}
	//	plotSpectraAll(th1);
}
