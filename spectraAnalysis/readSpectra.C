//#include <map>

class Spectra {
	public:
		Spectra();
		~Spectra();

		int Read(const TString filename);
		int Plot();
		TH1F *GetTH1();
		void Print();
		void Reset();

	private:
		int ReadChn();
		int ReadSpc();
		int ReadIEC();
TString FormatDateTime(char *yearStr,char *monthStr,char *dayStr,char *timeStr);

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
	printf("Dead Time:%5.2lf\%\n",100. * (realTime-liveTime)/realTime);
	printf("Channel Number:%d\n",channels);
	printf("Energy Calibration: E = %.3lf + %.3lf*chn +%.3lf*chn^2\n",energyCalPar[0],energyCalPar[1],energyCalPar[2]);

}

int Spectra::Plot()
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
	return h;;
}
int Spectra::Read(const TString filename)
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("readSpectra.C","");
	dir.ReplaceAll("/./","/");
	pathDir = dir;

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
	//	printf("file size:%d\n",file_size);
	metaData= new char[file_size];
	//BYTE * data= new BYTE[file_size];
	memset(metaData,0,file_size);

	input.read(metaData,file_size*sizeof(char));
	input.close();

	short d1,d2;
	memcpy(&d1,metaData,sizeof(short));
	memcpy(&d2,metaData+2,sizeof(short));
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
	if(filename.Contains("IEC"))
	{
		spectraType.Form("IEC");
		ReadIEC();
	}

	delete [] metaData ;
}
TString Spectra::FormatDateTime(char *yearStr,char *monthStr,char *dayStr,char *timeStr)
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
		yearStr[3]=='\0';
		dataTimeStr.Form("20%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr);
	}
	else
		dataTimeStr.Form("19%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr);

	return dataTimeStr;
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
	memcpy(&energy_par_t,metaData+32+4*chn+4,sizeof(float));
	energyCalPar[0]=energy_par_t;
	memcpy(&energy_par_t,metaData+32+4*chn+8,sizeof(float));
	energyCalPar[1]=energy_par_t;
	memcpy(&energy_par_t,metaData+32+4*chn+12,sizeof(float));
	energyCalPar[2]=energy_par_t;
}

int Spectra::ReadSpc()
{
	int shortSize=2;
	int floatSize=4;
	int doubleSize=8;

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
	memcpy(spectraData,metaData+(SPCTRP-1)*128,channels*4);
	//printf("channel 976:%d\n",spectraData[976]);
	for(int i=0; i<channels; i++)
	{
		counts.push_back((int)spectraData[i]);
	}
	delete [] spectraData ;
	//Short File Name
	char ACQStr[129];
	memset(ACQStr,0,129);
	memcpy(ACQStr,metaData+128*(ACQIRP-1),128);
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
	memcpy(CALRP1Str,metaData+128*(CALRP1-1),128);

	//Get energy calibration
	float energy_par_t;
	memcpy(&energy_par_t,CALRP1Str+20,floatSize);
	energyCalPar[0]=energy_par_t;
	memcpy(&energy_par_t,CALRP1Str+24,floatSize);
	energyCalPar[1]=energy_par_t;
	memcpy(&energy_par_t,CALRP1Str+28,floatSize);
	energyCalPar[2]=energy_par_t;
	//	printf("%s  %s  %s\n",CALDESStr,CALRP1Str,CALRP2Str);
}
void readSpectra(TString fileName = "lead_shield_background_2.Chn")
{
	//TString fileName("ba133_08_20131016_1.Spc");
	//TString fileName("ba133_8.Chn");
	//		TString fileName("lead_shield_background_2.Chn");
	//TString fNames[]={"i1-bg-pump.Chn","i1_p7-s1-U-pump.Spc","i1-s2-dianchenji_U-pump_p1.Spc"};
	TString fNames[]={"i1-bg-pump.Chn","i1-s2-dianchenji_U-pump_p1.Spc","i1-s1-U-pump_3.Spc"};
	TH1F *th1;
	Spectra *sp ;
	sp= new Spectra();
	sp->Read(fileName);
	th1=sp->GetTH1();
	THStack *hs = new THStack("hs","Alpha Spectra");
	/*	
		for(int i=0;i<3;i++)
		{
		sp= new Spectra();
		sp->Read(fNames[i]);
		th1=sp->GetTH1();

	//	th1->SetFillColor(2+i);
	th1->SetLineColor(2+i);
	hs->Add(th1);
	}
	*/
	sp->Print();
	hs->Add(th1);
	hs->Draw("nostack,elp");
	//	hs->Draw();
	hs->GetXaxis()->SetTitle("Channel");
	hs->GetXaxis()->CenterTitle(1);
	hs->GetYaxis()->SetTitle("counts");
	hs->GetYaxis()->CenterTitle(1);
	gPad->SetGrid();
	gPad->Update();
}
