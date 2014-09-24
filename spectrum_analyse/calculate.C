/*
Calculate
*/

#define LN2  log(2)
//#define LN2  0.693
#define Avogadro_num 6.02E+23
//#define Avogadro_num 6.02
//#define HalfLife_233Th

void calculate()
{
    //double t_i=;  //
    double t_d=2*24*3600.;
    double t_c=300;
    double HalfLife=32.01*24*3600;

    double eff=0.35;
    double eff_r=0.08;

    int ato_mass=233;

    int n=10;
    double *A=new double[n];
    double *mass=new double[n];
    //double S=1-exp(-LN2*t_i/HalfLife);

    double D=exp(-LN2*t_d/HalfLife);
    double C=(1-exp(-LN2*t_c/HalfLife))/t_c;
    //printf("D=%lf\tC=%lf\n",D,C);
    //TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,600);
    for(int i=0; i<n; i++) {
        A[i]=10000.*(i+1);

        double N0=A[i]/(C*eff*eff_r*D);
        mass[i]=N0*ato_mass/Avogadro_num;
        //printf("nuclear number=%lf\tMass=%lf\n",N0[i],mass[i]);
    }
    TGraph *gr=new TGraph(n,A,mass);
    gr->Draw("AC*");

    //printf("nuclear number=%lf\tMass=%lf\n",N0,mass);
    //double W=ato_mass*A/(Avogadro_num*iso_abundance*neutron_flux*neu_capture_cross_section*gamma_abundance*eff_peak*S*D*C);
}




