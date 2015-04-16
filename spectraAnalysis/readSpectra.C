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
	char monthStr[4],dayStr[3],yearStr[3];
	memset(monthStr,0,4);
	memset(dayStr,0,3);
	memset(yearStr,0,3);
	memcpy(dayStr,metaData+16,2);
	memcpy(monthStr,metaData+18,3);
	memcpy(yearStr,metaData+21,2);

	char month[12][4] = {"Jan","Feb","Mar","Apr",
		"May","Jun","Jul","Aug",
		"Sep","Oct","Nov","Dec" } ;
	int nMonth=0;
	TString monthStr1(monthStr);
	while(!monthStr1.Contains(month[nMonth]) && nMonth<11)  nMonth++;
	nMonth++;
	TString dataTimeStr;
	if(metaData[23]=='1')
		dataTimeStr.Form("20%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr.Data());
	else
		dataTimeStr.Form("19%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr.Data());
	measureTime.Set(dataTimeStr.Data());
	measureTime.Print();

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
	//	map<string,int> mapData ;
	int shortSize=2;
	int floatSize=4;
	int doubleSize=8;
	/*
	   string mapTag[] ={
	   "ACQIRP", "SAMDRP", "DETDRP", "EBRDESC",
	   "ANARP1", "ANARP2", "ANARP3", "ANARP4",
	   "SRPDES", "IEQDESC","GEODES", "MPCDESC",
	   "CALDES", "CALRP1", "CALRP2", "SPCTRP",
	   "SPCRCN", "SPCCHN", "ABSTCH", "ACQTIM",
	   "ACQTI8", "SEQNUM", "MCANU",  "SEGNUM",
	   "MCADVT", "CHNSRT", "RLTMDT", "LVTMDT"
	   };
	   for(int i=0;i<34;i++)
	   {
	   short d;
	   memcpy(&d,metaData+4+i*shortSize,shortSize);
	   mapData[mapTag[i]]=(int)d;
	   }
	   */
	short	ACQIRP, SAMDRP, DETDRP, EBRDESC,
			ANARP1, ANARP2, ANARP3, ANARP4,
			SRPDES, IEQDESC,GEODES, MPCDESC,
			CALDES, CALRP1, CALRP2, SPCTRP,
			SPCRCN, SPCCHN, ABSTCH, ACQTIM,
			ACQTI8, SEQNUM, MCANU,  SEGNUM,
			MCADVT, CHNSRT;
	short d[15];
	double RLTMDT, LVTMDT;
	for(int i=0;i<15;i++)	
	{
		memcpy(d+i,metaData+8+i*shortSize,shortSize);
	}
	ACQIRP = d[0]; SAMDRP = d[1]; DETDRP = d[2]; EBRDESC = d[3];
	ANARP1 = d[4]; ANARP2 = d[5]; ANARP3 = d[6]; ANARP4 = d[7];
	SRPDES = d[8]; IEQDESC = d[9];GEODES = d[10]; MPCDESC = d[11];
	CALDES = d[12]; CALRP1 = d[13]; CALRP2 = d[14];

	memcpy(&SPCTRP,metaData+60,shortSize);
	memcpy(&SPCRCN,metaData+62,shortSize);
	memcpy(&SPCCHN,metaData+64,shortSize);
	memcpy(&ABSTCH,metaData+66,shortSize);
	memcpy(&ACQTIM,metaData+68,shortSize);
	memcpy(&ACQTI8,metaData+70,floatSize);
	memcpy(&SEQNUM,metaData+74,doubleSize);
	memcpy(&MCANU,metaData+82,shortSize);
	memcpy(&SEGNUM,metaData+84,shortSize);
	memcpy(&MCADVT,metaData+86,shortSize);
	memcpy(&CHNSRT,metaData+88,shortSize);
	//Get RealTime and LiveTime
	float d1;
	memcpy(&d1,metaData+90,floatSize);
	realTime= (double)d1;
	memcpy(&d1,metaData+94,floatSize);
	liveTime= (double)d1;

	char ACQINFO[129];
	memset(ACQINFO,0,129);
	memcpy(ACQINFO,metaData+128*ACQIRP,128);
	char SAMDINFO[129];
	memset(SAMDINFO,0,129);
	memcpy(SAMDINFO,metaData+128*SAMDRP ,128);
	channels= SPCCHN;
	//channels= 2048;
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
	char ACQStr[91];
	memset(ACQStr,0,91);
	memcpy(ACQStr,metaData+128*(ACQIRP-1),90);
	//printf("%s\n",ACQStr);
	char shortFileName[17];
	memset(shortFileName,0,17);
	memcpy(shortFileName,ACQStr,16);
	//Measure DateTime
	char monthStr[4],dayStr[3],yearStr[3];
	memset(monthStr,0,4);
	memset(dayStr,0,3);
	memset(yearStr,0,3);
	memcpy(dayStr,ACQStr+16,2);
	memcpy(monthStr,ACQStr+19,3);
	memcpy(yearStr,ACQStr+23,2);
	char timeStr[11];
	memset(timeStr,0,11);
	memcpy(timeStr,ACQStr+28,10);

	char month[12][4] = {"Jan","Feb","Mar","Apr",
		"May","Jun","Jul","Aug",
		"Sep","Oct","Nov","Dec" } ;
	int nMonth=0;
	TString monthStr1(monthStr);
	while(!monthStr1.Contains(month[nMonth]) && nMonth<11)  nMonth++;
	nMonth++;
	TString dataTimeStr;
	if(ACQStr[25]=='1')
		dataTimeStr.Form("20%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr);
	else
		dataTimeStr.Form("19%s-%d-%s %s",yearStr,nMonth,dayStr,timeStr);
	measureTime.Set(dataTimeStr.Data());
	/*	//LiveTime and RealTime
		char liveTimeStr[11];
		memset(liveTimeStr,0,11);
		memcpy(liveTimeStr,ACQStr+38,10);
		char realTimeStr[11];
		memset(realTimeStr,0,11);
		memcpy(realTimeStr,ACQStr+48,10);
	//	printf("LT:%lf");
	//sample_desc  
	char sampleDescStr[129];
	memset(sampleDescStr,0,129);
	memcpy(sampleDescStr,metaData+128*(SAMDRP-1),128);
	//det_desc
	char detDescStr[129];
	memset(detDescStr,0,129);
	memcpy(detDescStr,metaData+128*(DETDRP-1),128);
	//det_ebr_desc
	char detEbrStr[129];
	memset(detEbrStr,0,129);
	memcpy(detEbrStr,metaData+128*(EBRDESC-1),128);
	//	printf("%s  %s  %s\n",sampleDescStr,detDescStr,detEbrStr);
	//analysis_param
	char ANARP1Str[129];
	memset(ANARP1Str,0,129);
	memcpy(ANARP1Str,metaData+128*(ANARP1-1),128);
	char ANARP2Str[129];
	memset(ANARP2Str,0,129);
	memcpy(ANARP2Str,metaData+128*(ANARP2-1),128);
	char ANARP3Str[129];
	memset(ANARP3Str,0,129);
	memcpy(ANARP3Str,metaData+128*(ANARP3-1),128);
	//	printf("%s  %s  %s\n",ANARP1Str,ANARP2Str,ANARP3Str);

	char CALDESStr[129];
	memset(CALDESStr,0,129);
	memcpy(CALDESStr,metaData+128*(CALDES-1),128);
	*/

	char CALRP1Str[129];
	memset(CALRP1Str,0,129);
	memcpy(CALRP1Str,metaData+128*(CALRP1-1),128);
	char CALRP2Str[129];
	memset(CALRP2Str,0,129);
	memcpy(CALRP2Str,metaData+128*(CALRP2-1),128);

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
	THStack *hs = new THStack("hs","Alpha Spectra");
	for(int i=0;i<3;i++)
	{
		sp= new Spectra();
		sp->Read(fNames[i]);
		th1=sp->GetTH1();

		//	th1->SetFillColor(2+i);
		th1->SetLineColor(2+i);
		hs->Add(th1);
	}
	hs->Draw("nostack,elp");
	//	hs->Draw();
	hs->GetXaxis()->SetTitle("Channel");
	hs->GetXaxis()->CenterTitle(1);
	hs->GetYaxis()->SetTitle("counts");
	hs->GetYaxis()->CenterTitle(1);
	gPad->SetGrid();
	gPad->Update();
}
