void reb() {
TCanvas *c1 = new TCanvas("c1","c1",800,1000);
c1->Divide(1,2);
c1->cd(1);
TF1 *fun1 = new TF1("fun1", "exp(-x)", 0, 5.);
TF1 *fun2 = new TF1("fun2", "exp(-2*x)", 0, 3.);

fun1->Draw();
fun2->Draw("same");

TH1F *hfun1 = new TH1F("hfun1","fun1",100,0,5);
TH1F *hfun2 = new TH1F("hfun2","fun2",100,0,3);
hfun1->Eval(fun1);
hfun2->Eval(fun2);

c1->cd(2);
hfun1->DrawCopy();
hfun2->Draw("same");
//to bypass the warning
hfun2->GetXaxis()->SetLimits(0,5);
hfun1->Add(hfun2, -1);
hfun2->GetXaxis()->SetLimits(0,3);
hfun1->SetLineColor(kBlue);
hfun1->Draw("same");
}
