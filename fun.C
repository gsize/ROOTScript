double fun1(double x)
{
	if(x>=0. && x<4.)
		return 0.164253*pow(x,4)-1.64495*pow(x,3)+6.6379*x*x-10.4153*x;
	if(x>=4.)
		return 0.472672*x*x+2.29629*x-15.6717;
}

void fun()
{
	TCanvas *myc = new TCanvas("myc", "FUN", 800, 600);
//	myc->Devid
THStack hs("hs","test stacked histograms");

	TF1 *fal1 = new TF1("f1","-0.077*exp(-x)+0.031",0,9);
	TH1 *h1=fal1->GetHistogram();
	TF1 *fal2 = new TF1("f1","(-0.036*exp(-2.1*x)-0.01)",0,9);
	TH1 *h2=fal1->GetHistogram();
	TF1 *fal3 = new TF1("f3","fun1(x)",0,9);
	TH1 *h3=fal3->GetHistogram();

	TH1F *hr = myc->DrawFrame(-0.4,-1.,10.0,50);
   hr->SetXTitle("X title");
   hr->SetYTitle("Y title");
   myc->GetFrame()->SetFillColor(21);
   myc->GetFrame()->SetBorderSize(12);
	    //TF1 *f5 = new TF1();
	    //f5->Draw();
	//fal1->SetLineColour(1); 
	//fal2->SetLineColour(50); 
	//fal3->SetLineColour(90); 
	    
	fal3->Draw();
	//TCanvas *c2 = new TCanvas("c2","c2",700,500);
	fal2->DrawIntegral("SAME");
	//TCanvas *c3 = new TCanvas("c3","c3",700,500);
	fal1->DrawIntegral("SAME");
	//TCanvas *c4 = new TCanvas("c4","c4",700,500);
	//fal3->DrawDerivative("SAME");

std::vector<double> test;
for(int i=0;i<10;i++) test.push_back(i*2.0);
std::vector<double>::iterator iter=test.begin();
for(;iter!=test.end();iter++) cout<<(*iter)<<std::endl;
/*
hs.Add(h1);
hs.Add(h2);
hs.Add(h3);
hs.Draw();
*/
}
