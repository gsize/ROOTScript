double get_Z(double A)
{
    return A/(1.98+0.0155*pow(A,2/3.));
}
void betaLine()
{
Int_t n=220;
double *Z=new double[n];
double *A=new double[n];
for(Int_t i=0;i<n;i++)
{
    A[i]=i+1;
    Z[i]=get_Z(A[i]);
    if(Z[i]>20 && Z[i]<75) printf("%lf\t%lf\n",Z[i],A[i]);
}
TCanvas *c1 = new TCanvas("c1","Graph Draw Options",200,10,600,400);

TGraph *gr=new TGraph(n,Z,A);
gr->Draw("AC*");
}
