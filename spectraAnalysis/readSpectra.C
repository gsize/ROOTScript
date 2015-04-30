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
		int ReadChn();  //ORTEC spectra .chn
		int ReadSpc();//ORTEC spectra  .spc
		int ReadCnf(); //Canberra spectra .cnf
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
	//printf("channel 1000: %lf",h->GetBinContent(1000));
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


int Spectra::ReadCnf()
{
	int shortSize=2;
	int floatSize=4;
	int intSize=4;
	int doubleSize=8;

	int acqOffset=0, samOffset=0,effOffset=0,encOffset=0,chanOffset=0;
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
						encOffset = offSet;
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
	printf("datetime:%ld time_t:%d\n",da,t);
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
}
void readSpectra(TString fileName = "lead_shield_background_2.Chn")
{
	//TString fileName("ba133_08_20131016_1.Spc");
	//TString fileName("ba133_8.Chn");
	//		TString fileName("lead_shield_background_2.Chn");
	//TString fNames[]={"i1-bg-pump.Chn","i1_p7-s1-U-pump.Spc","i1-s2-dianchenji_U-pump_p1.Spc"};
	//	TString fNames[]={"i1-bg-pump.Chn","i1-s2-dianchenji_U-pump_p1.Spc","i1-s1-U-pump_3.Spc"};
	TString fileName("Test_spc.cnf");
	//TString fileName("Naidemo.cnf");
	TH1F *th1;
	Spectra *sp ;
	sp= new Spectra();
	sp->Read(fileName);
	sp->Print();
	//	/*	
	THStack *hs = new THStack("hs","Alpha Spectra");
	th1=sp->GetTH1();
	/*
	   for(int i=0;i<3;i++)
	   {
	   sp= new Spectra();
	   sp->Read(fNames[i]);
	   th1=sp->GetTH1();

	//	th1->SetFillColor(2+i);
	th1->SetLineColor(2+i);
	hs->Add(th1);
	}*/
	hs->Add(th1);
	hs->Draw("nostack,elp");
	//	hs->Draw();
	hs->GetXaxis()->SetTitle("Channel");
	hs->GetXaxis()->CenterTitle(1);
	hs->GetYaxis()->SetTitle("counts");
	hs->GetYaxis()->CenterTitle(1);
	gPad->SetGrid();
	gPad->Update();
	//	*/
}
