#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <math>
#include <fstream>
using std::string;
#define ln2 0.693

typedef struct
{
    char nuclear[20];
    double abundant;
    double mass;
    double th;
    double i;
    double energy;
    double ratio;
    double halflive;
    char halflive_unit;
} nucl;
double count_ratio(double count,double ratio,double eff)
{
    return count/(0.01*0.01*ratio*eff);
}

double get_halflive(nucl& a)
{
//printf("kkk=%c\n",a.halflive_unit);
    double ti=0.;
    switch(a.halflive_unit)
    {
    case 's':
        ti=a.halflive;
        break;
    case 'm':
        ti=a.halflive*60.;
        break;
    case 'h':
        ti=a.halflive*3600.;
        break;
    case 'd':
        ti=a.halflive*86400.;
        break;
    case 'y':
    case 'a':
        ti=a.halflive*3.15576e7;
        break;

    default:ti=a.halflive;
    }
return ti;
}
double para_S(double irradiate,nucl& a)
{
//printf("name=%s/tthalflive=%lf\n",a.nuclear,get_halflive(a));
    return 1-exp(-ln2*irradiate/get_halflive(a));
}
double para_D(double decay,nucl& a)
{
    return exp(-ln2*decay/get_halflive(a));
}
int cal_neutron_flux()
{
    double count1=358.,count2=695.;
    double eff1=0.1,eff2=0.18;
    double irradiate1=72800.,irradiate2=72800.;
    double decay1=172800.,decay2=172800.;

    nucl c1;
    nucl c2;

	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("plotGraph.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;
	pathName = dir + "input.txt";
    ifstream in;
    in.open(pathName.Data());
    string str_tmp;

    getline(in,str_tmp);
    getline(in,str_tmp);
	char tmp_str[6];
    sscanf(str_tmp.c_str(),"%s%lf%lf%lf%lf%lf%lf%lf%s",c1.nuclear,&(c1.abundant),&(c1.mass),&(c1.th),&(c1.i),&(c1.energy),&(c1.ratio),&(c1.halflive),tmp_str);
    c1.halflive_unit=tmp_str[0];
printf("c1=%c\n",c1.halflive_unit);
    getline(in,str_tmp);
    sscanf(str_tmp.c_str(),"%s%lf%lf%lf%lf%lf%lf%lf%s",c2.nuclear,&(c2.abundant),&(c2.mass),&(c2.th),&(c2.i),&(c2.energy),&(c2.ratio),&(c2.halflive),tmp_str);
    cout<<str_tmp<<endl;
    c2.halflive_unit=tmp_str[0];
printf("c2=%c\n",c2.halflive_unit);
    double A1=count_ratio(count1,c1.ratio,eff1);
    double A2=count_ratio(count2,c2.ratio,eff1);
    double s1=para_S(irradiate1,c1);
    double s2=para_S(irradiate2,c2);
    double d1=para_D(decay1,c1);
    double d2=para_D(decay2,c2);
printf("A1=%lf,A2=%lf\n",A1,A2);
printf("s1=%lf,s2=%lf\n",s1,s2);
printf("d1=%lf,d2=%lf\n",d1,d2);
    double k1=A1/A2;
    double k2=c1.abundant/c2.abundant;
    double k3=s1*d1/(s2*d2);
    double k4=k1/(k2*k3);
    double k5=(c1.i-k4*c2.i)/(k4*c2.th-c1.th);
printf("k1=%lf\n",k1);
printf("k2=%lf\n",k2);
printf("k3=%lf\n",k3);
printf("k4=%lf\n",k4);
printf("k5=%lf\n",k5);

return 0;
}
