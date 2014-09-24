
double fg_fun(double h,double r)
{
    return (1-h/sqrt(h*h+r*r))/2;
}

void jiheyizi()
{
    //double fg;
    double step=1.;
    double r=2.36;
    int num=50;
    double h0=0.834;
    double xmax=num*step+1.;
    //double *h=new double[num];
   // double *fg=new double[num];
    double sum=0.;
    TH1D *d = new TH1D("d","",num,0,xmax);
    TH1D *d1 = new TH1D("d","",num,0,xmax);
    for(int i=0;i<num;i++)
    {
        sum+=step;
        h0+=step;
        d->SetBinContent(i+1,fg_fun(sum,r));
        d1->SetBinContent(i+1,fg_fun(h0,r));
    }
    d->SetLineColor(kRed);
    d->Draw("L");
    d1->Draw("SAME L");
}
