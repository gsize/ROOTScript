#include "TGlobal.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"


Double_t cal_radius2(Double_t x,Double_t y,Double_t z)
{
    return (x*x+y*y+z*z);
}

Double_t cal_distance2(Double_t x1,Double_t y1,Double_t z1,Double_t x2,Double_t y2,Double_t z2)
{
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2);
}

void cal_center_sphere(TString FileName = "points.root")
{
    Double_t radius_max=24.5,radius_ball=0.5 ;
    Int_t count_sphere=10000;
    Double_t *center_x= new Double_t [count_sphere];
    Double_t *center_y= new Double_t [count_sphere];
    Double_t *center_z= new Double_t [count_sphere];
	Double_t radiusMax2 = 0.;
	Double_t distanceMax2 = 0.;

	radiusMax2 = radius_max * radius_max;
	distanceMax2 = 4.0 * radius_ball*radius_ball;
	
    for(Int_t i=0; i<count_sphere; i++)
    {
        center_x[i]=0.;
        center_y[i]=0.;
        center_z[i]=0.;
    }

    gRandom->SetSeed(5);
    Double_t px, py, pz;

    TRandom3 r;
    Int_t i=1;
    Int_t k=0;
    Int_t all_num=0;

    while(i<count_sphere)
    {
        all_num++;
        px=r.Uniform(-radius_max,radius_max);
        py=r.Uniform(-radius_max,radius_max);
        pz=r.Uniform(-radius_max,radius_max);

        if( cal_radius2(px,py,pz)<radiusMax2 )
        {
            Int_t j=0;
            k++;
            for(j=0; j<i; j++)
            {
                if( cal_distance2( center_x[j],center_y[j],center_z[j] ,px,py,pz)< distanceMax2 )
                {
                    break;
                }
            }
            if(j==i)
            {
                center_x[i]=px;
                center_y[i]=py;
                center_z[i]=pz;

                if(i % 1000 == 0)
                    printf("%lf\t%lf\t%lf\t%lf\n",center_x[i],center_y[i],center_z[i],sqrt(cal_radius2(center_x[i],center_y[i],center_z[i])));

                i++;
            }
        }
    }
    printf("i=%d\tk=%d\tall_num=%d\ti/all_num=%lf\n",i,k,all_num,k*1.0/all_num);
 
    TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("cal_center_sphere.C","");
   dir.ReplaceAll("/./","/");
    TString PathName;
    PathName = dir + FileName;
    TFile pf(PathName,"recreate");
    TTree *tr = new TTree("points","tree points");
//tr->Branch("count_sphere",count_sphere,"count_sphere/I");
    tr->Branch("px",&px,"px/D");
    tr->Branch("py",&py,"py/D");
    tr->Branch("pz",&pz,"pz/D");
    for(Int_t i=0; i<count_sphere; i++)
    {
        px=center_x[i];
        py=center_y[i];
        pz=center_z[i];
        tr->Fill();
    }

    tr->Write();
    pf.Close();
}
