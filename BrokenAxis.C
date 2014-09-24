void BrokenAxis()
{
   TCanvas *c = new TCanvas("c", "c",700,900);         

   TPad *p1 = new TPad("p1","p1",0.1,0.5,0.9,0.901);
   p1->SetBottomMargin(0.);           
   p1->SetBorderMode(0);
   p1->Draw();

   TPad *p2 = new TPad("p2","p2",0.1,0.1,0.9,0.501);
   p2->SetTopMargin(0.);   
   p2->SetBorderMode(0);
   p2->Draw();

   float x1[] = {3,5,6,9}; float y1[] = {100,900,400,200};
   TGraph *gr1 = new TGraph(4,x1,y1);
   gr1->GetXaxis()->SetLimits(0.,10.);                       
   gr1->SetTitle("");
   gr1->GetXaxis()->SetLabelSize(0);
   gr1->GetXaxis()->SetTickLength(0);   
   gr1->SetMarkerStyle(20);

   float x2[] = {1,2,3,8}; float y2[] = {4,7,6,5};
   TGraph *gr2 = new TGraph(4,x2,y2);
   gr2->GetXaxis()->SetLimits(0.,10.);     
   gr2->SetTitle("");
   gr2->SetMarkerStyle(22);

   p1->cd();
   gr1->Draw("ALP");
   gr1->GetHistogram()->SetMinimum(-20.);       
   
   p2->cd();
   gr2->Draw("ALP");
   gr2->GetHistogram()->SetMaximum(7.5); 

   c->cd();
   TPad *b = new TPad("b","b",0.1,0.46,0.8199,0.54);
   b->SetBorderMode(0);
   b->Draw();
   b->cd();
   TLine *line = new TLine(0.11,0,0.1105677,0.399656);
   line->Draw();
   line = new TLine(0.1105677,0.5860092,0.11,1);
   line->Draw();
   line = new TLine(0.076639,0.5143349,0.1524797,0.6863532);
   line->Draw();
   line = new TLine(0.076639,0.3423165,0.1524797,0.5143349);
   line->Draw();
}
