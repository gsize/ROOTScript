/*
#include "TGlobal.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
*/
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"


double cal_radius2(double x,double y,double z)
{
return (x*x+y*y+z*z);
}

double cal_distance2(double x1,double y1,double z1,double x2,double y2,double z2)
{
return ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
}

//void cal_center_sphere()
int main(int argc, char *argv[])
{

using namespace std;
//using std::
double radius_max=25.,radius_ball=1.;
int count_sphere=5000;
if(argc>1)
{
	count_sphere=atoi(argv[1]);
}
printf("number:%d\n",count_sphere);

double *center_x= new double [count_sphere];
double *center_y= new double [count_sphere];
double *center_z= new double [count_sphere];

srand((unsigned)time(NULL));

for(int i=0;i<count_sphere;i++)
{
center_x[i]=0.;
  center_y[i]=0.;
  center_z[i]=0.;
}

//  gRandom->SetSeed(5);
  double px, py, pz;

//TRandom3 r;
int i=1;
int k=0;
int all_num=0;

while(i<count_sphere){
	all_num++;
	px=((double)rand()/RAND_MAX)*radius_max*2-radius_max;
	py=((double)rand()/RAND_MAX)*radius_max*2-radius_max;
	pz=((double)rand()/RAND_MAX)*radius_max*2-radius_max;
    /*
    px=r.Uniform(-radius_max,radius_max);
     py=r.Uniform(-radius_max,radius_max);
     pz=r.Uniform(-radius_max,radius_max);
*/
     if( cal_radius2(px,py,pz)<625.0 )
     {
     	int j=0;
     	k++;
     	for(j=0;j<i;j++)
     	{
     	if( cal_distance2( center_x[j],center_y[j],center_z[j] ,px,py,pz)<4.0 ){
     		 break;
     	}
     	}
     	if(j==i)
     	{
     		center_x[i]=px;
     		center_y[i]=py;
     		center_z[i]=pz;

     		if(i%1000 == 0)
                printf("%lf\t%lf\t%lf\t%lf\n",center_x[i],center_y[i],center_z[i],sqrt(cal_radius2(center_x[i],center_y[i],center_z[i])));

     		i++;

     	}
     }
  }
printf("i=%d\tk=%d\tall_num=%d\ti/all_num=%lf\n",i,k,all_num,k*1.0/all_num);

FILE *fp;
if((fp=fopen("points.txt","w"))== NULL)
{
printf("Can't open file");
exit(1);
}

for(int k=0; k<count_sphere; k++)
{
	fprintf(fp,"%lf\t%lf\t%lf\n",center_x[k],center_y[k],center_z[k]);
}
fclose(fp);
printf("Saved file!!\n");
 /*
for(int ii=0;ii<10;ii++)
{
	printf("%lf\t%lf\t%lf\t%lf\n",center_x[ii],center_y[ii],center_z[ii],cal_radius(center_x[ii],center_y[ii],center_z[ii]));
}



TCanvas *c1 = new TCanvas("c1","o",200,10,600,900);
TH3D *hp    = new TH3D("hp","Normal histogram",100,-radius_max,radius_max,100,-radius_max,radius_max,100,-radius_max,radius_max);
for(int iii=0;iii<count_sphere;iii++)
{
hp->Fill(center_x[iii],center_y[iii],center_z[iii]);
}
hp->Draw();

TCanvas *c2 = new TCanvas("c2","c2",20,10,600,900);
TGraph2D *dt= new TGraph2D(count_sphere,center_x,center_y,center_z);
dt->Draw();

TFile pf("points.root","recreate");
TTree *tr = new TTree("t1","tree points");
tr->Branch("px",center_x,"center_x[count_sphere]/D");
tr->Branch("py",center_y,"center_y[count_sphere]/D");
tr->Branch("pz",center_z,"center_z[count_sphere]/D");
tr->Fill();
tr->Write();
pf.Close();
*/

return 0;
}
