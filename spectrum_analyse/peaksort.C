//#include <stdio.h>
//#include <string.h>
//#include <string>

#define int16_t short
#define int16_tt short
typedef struct
{
    //float peaks,areas,errs,err2,fwhms,energys;
    float areas;
    float peaks;
    
    float errs;
    float err2;
    float energys;
    float fwhms;
    
    char nuclides[39];
    char mks;
} PKS;

struct date
{
    short da_year;
    char da_day;
    char da_mon;
};

struct time
{
    unsigned char ti_min;
    unsigned char ti_hour;
    unsigned char ti_hund;
    unsigned char ti_sec;
};

PKS *pks0;
short pks_max;
	TString str[10];
	TString str_name[10];

int rd_grph(int num)
{
	//short pks_max;
    short* ppk_num0 = &pks_max;
    short irange, len, start, resv, i;
    char ck_version,brk_pk;
    char textbuf[512];
    float livetime, realtime;
    float ene_fact[3],fwhm_fact[5],hw,limit_resolve;
    FILE* stream;
    PKS pks00;
    struct date dt_start;
    struct time tm_start;

    if ((stream = fopen(str[num].Data(), "rb")) != NULL)
    {
        fread(&irange, sizeof(int16_t), 1, stream);
    fread(&pks_max, sizeof(int16_tt), 1, stream);
            pks0= new PKS[pks_max];
                
            fread(ene_fact, sizeof(float), 3, stream);
            fread(fwhm_fact, sizeof(float), 5, stream);
            fread(&hw, sizeof(hw), 1, stream);
            fread(&ck_version, 1, 1, stream);
            fread(&brk_pk, sizeof(brk_pk), 1, stream);
            fread(&resv, sizeof(int16_tt), 1, stream);
            fread(&limit_resolve, sizeof(limit_resolve), 1, stream);
            fread(&livetime, sizeof(float), 1, stream);
            fread(&realtime, sizeof(float), 1, stream);
            //printf("%f\n",livetime);
            fread(&(dt_start.da_year), sizeof(short), 1, stream);
            fread(&(dt_start.da_day), sizeof(char), 1, stream);
            fread(&(dt_start.da_mon), sizeof(char), 1, stream);
            fread(&tm_start, sizeof(tm_start), 1, stream);
                fread(textbuf, 64, 1, stream);
                for (i = 0; i < pks_max; i++)
                {
                    //fread(&pks00, sizeof(PKS), 1, stream);
                    fread(&(pks00.areas), sizeof(float), 1, stream);
                    fread(&(pks00.errs), sizeof(float), 1, stream);
                    fread(&(pks00.err2), sizeof(float), 1, stream);
                    
                    fread(&(pks00.fwhms), sizeof(float), 1, stream);
                    
                    fread(&(pks00.energys), sizeof(float), 1, stream);
                    fread(&(pks00.peaks), sizeof(float), 1, stream);
                    
                    fread(pks00.nuclides, sizeof(char), 39, stream);
                    fread(&(pks00.mks), sizeof(char), 1, stream);
                   // printf("%f\t%f\t%f\n",pks00.energys,pks00.fwhms,pks00.areas);
                    //memcpy(pks0 + i, &pks00, sizeof(PKS));
                    (pks0+i)->energys=pks00.energys;(pks0+i)->fwhms=pks00.fwhms;(pks0+i)->areas=pks00.areas;
                }
            return 0;
    }
    return -1;
}


   /*for (int i = 0; i < pks_max; i++)
   {
     //printf("%f\t%f\t%f\n",(pks0+i)->peaks,(pks0+i)->energys,(pks0+i)->areas);
     printf("%f\t%f\t%f\n",(pks0+i)->energys,(pks0+i)->fwhms,(pks0+i)->areas);
     printf("%f\t%f\t%f\n",((pk_list[2])+i)->energys,((pk_list[2])+i)->fwhms,((pk_list[2])+i)->areas);
   }*/
void peaksort()
{
  PKS* pk_list[10];
  int pk_num[10];
	pks0=0;
	for(int i=0;i<10;i++){
		str[i].Form("/home/gsz/work_dir/SPECTRUM/sample/Ba133_eff_15cm_%d.grp",i+1);
		str_name[i].Form("Ba133_eff_15cm_%d",i+1);
	}
	for (int i=0;i<10;i++)
	{
   		if(-1==rd_grph(i))
   			printf("fail!\n");
   		pk_num[i]=pks_max;
   		pk_list[i]=pks0;

   }
   TCanvas *c1 = new TCanvas("c1","c1",700,500);
   TMultiGraph *mg = new TMultiGraph();
   
   double *areas_l=new double[10];
   double *nx1= new double[10];
   for(int j=0;j<10;j++){
   		nx1[j]=(j+1)*1.;
   }
   for(int j=0;j<10;j++){
   double *nx= new double[pk_num[j]];
   double *ny= new double[pk_num[j]];
   double *ex= new double[pk_num[j]];
   double *ey= new double[pk_num[j]];
   for(int i=0;i<pk_num[j];i++){
   		nx[i]=(j+1)*1.;
   		ex[i]=0.;
   		ny[i]=((pk_list[j])+i)->energys;
   		ey[i]=0.5*(((pk_list[j])+i)->fwhms);
   		//printf("%lf\n",ny[i]);
   		if(abs(ny[i]-356.)<1.0)
   		 areas_l[j]=(((pk_list[j])+i)->areas)/900;
   }
   TGraph *gr1 =new TGraphErrors(pk_num[j],nx,ny,ex,ey);
   mg->Add(gr1);
   delete [] nx;
   delete [] ny;
   delete [] ex;
   delete [] ey;
   }
   mg->Draw("A*");
   
   TCanvas *c2 = new TCanvas("c2","c2",700,500);
   TGraph *gr2 = new TGraph(10,nx1,areas_l);
   TAxis *ya= gr2->GetYaxis();
   ya->SetLimits(1.0,38.0);
   gr2->Draw("AC*");
   
   for(int i=0;i<10;i++){
   	if (pk_list[i])
   		delete [] pk_list[i];
   	}
}
