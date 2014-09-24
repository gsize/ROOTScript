void particle_yields4(){

  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

//130-200GeV
   const int N4 = 5;
   double y4[N4] = {21.24,2.82,2.10,1.41,0.39}; 
   double y42[N4] = {21.30,2.35,2.27,1.215,0.392};
   double ey4[N4] = {0.39,0.19,0.12,0.18,0.03};
   double x4[N4] = {1,2,3,4,5}; 
   double ex4[N4] = {0.1,0.1,0.1,0.1,0.1};

   const char* labels4[N4] = {
      "#pi^{+}",
      "K^{+}",
      "K^{0}",
      "p",
      "#Lambda",
   };

//91GeV old version
   const int N3 = 33;
   double y3[N3] = {17.02,9.42,2.228,2.049,1.049,0.152,0.146,0.27,1.231,2.40,1.016,0.715,0.738,0.0963,0.165,0.056,0.166,0.012,0.084,1.05,0.3915,0.076,0.081,0.107,0.174, 0.0258,0.085,0.0240,0.0239,0.0462,0.0055,0.0016,0.0222}; 
   double y32[N3] = {18.21,10.42,2.022,1.9506,0.922,0.0693,0.0987,0.212,1.206,2.102,0.9587,0.63,0.62,0.129,0.0612,0.029,0.133,0.01594,0.056,1.0254,0.337,0.08222,0.07696,0.07904,
   0.156,0.02104,0.1638,0.0395,0.0404,0.0799,0.008258,0.001269,0.0229};  
   double ey3[N3] = {0.19,0.32,0.059,0.026,0.080,0.020,0.012,0.11,0.098,0.43,0.065,0.059,0.024,0.0032,0.051,0.012,0.020,0.006,0.022,0.032,0.0065,0.011,0.010,0.011,0.009, 0.0010,0.014,0.0017,0.0015,0.0028,0.0005,0.0003,0.0027};
   double ex3[N3] = {0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4}; 
   double x3[N3] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33};  

   const char* labels3[N3] = {
      "#pi^{+}",
      "#pi^{0}",
      "K^{+}",
      "K^{0}",
      "#eta",
      "#eta'",
      "f_{0}",
      "a_{0}^{#pm}",
      "#rho^{0}",
      "#rho^{#pm}",
      "#omega",
      "K*^{#pm}",
      "K*^{0}",
      "#phi",
      "f_{1}(1285)",
      "f_{1}(1420)",
      "f_{2}",
      "f_{2}'",
      "K_{2}*^{0}",
      "p",
      "#Lambda",
      "#Sigma^{0}",
      "#Sigma^{-}",
      "#Sigma^{+}",
      "#Sigma^{#pm}",
      "#Xi^{-}",
      "#Delta^{++}",
      "#Sigma(1385)^{-}",
      "#Sigma(1385)^{+}",
      "#Sigma(1385)^{#pm}",
      "#Xi(1530)^{0}",
      "#Omega^{-}",
      "#Lambda(1520)"
   };


//29-35GeV
   const int N2 = 21;
   double y2[N2] = {10.3,5.83,1.48,1.48,0.61,0.26,0.05,0.81,0.64,0.56,0.085,0.14,0.09,0.12,0.64,0.205,0.0176,0.017,0.017,0.033,0.014}; 
   double y22[N2] = {10.99,6.48,1.34,1.29,0.666,0.0590,0.0656,0.775,0.423,0.415,0.123,0.0928,0.0423,0.0414,0.564,0.218,0.0148,0.0248,0.0253,0.0501,0.000977};
   double ey2[N2] = {0.4,0.28,0.09,0.07,0.07,0.10,0.02,0.08,0.05,0.06,0.011,0.04,0.03,0.06,0.050,0.010,0.0027,0.004,0.004,0.008,0.007};
   double ex2[N2] = {0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4}; 
   double x2[N2] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};  

   const char* labels2[N2] = {
      "#pi^{+}",
      "#pi^{0}",
      "K^{+}",
      "K^{0}",
      "#eta",
      "#eta'",
      "f_{0}",
      "#rho^{0}",
      "K*^{#pm}",
      "K*^{0}",
      "#phi",
      "f_{2}",
      "K_{2}*^{#pm}",
      "K_{2}*^{0}",
      "p",
      "#Lambda",
      "#Xi^{-}",
      "#Sigma(1385)^{-}",
      "#Sigma(1385)^{+}",
      "#Sigma(1385)^{#pm}",
      "#Omega^{-}",
   };


//10GeV
   const int N1 = 24;
   double y1[N1] = {6.6,3.2,0.9,0.91,0.20,0.03,0.024,0.35,0.30,0.27,0.29,0.044,0.09,0.253,0.080,0.023,0.0059,0.040,0.006,0.005,0.0106,0.0015,0.0007,0.008}; 
   double y12[N1] = {6.88,4.25,0.63,0.60,0.422,0.0339,0.0447,0.528,0.425,0.180,0.176,0.0648,0.0593,0.192,0.0571,0.0145,0.00266,0.0281,0.00644,0.00659,0.0130,
                   0.00104,0.000104,0.00398};
   double ey1[N1] = {0.2,0.3,0.04,0.05,0.04,0.01,0.006,0.04,0.08,0.03,0.03,0.003,0.02,0.016,0.007,0.008,0.0007,0.010,0.002,0.001,0.0020,0.0006,0.0004,0.002};
   double ex1[N1] = {0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4,0.4}; 
   double x1[N1] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};  

   const char* labels1[N1] = {
      "#pi^{+}",
      "#pi^{0}",
      "K^{+}",
      "K^{0}",
      "#eta",
      "#eta'",
      "f_{0}",
      "#rho^{0}",
      "#omega",
      "K*^{#pm}",
      "K*^{0}",
      "#phi",
      "f_{2}",
      "p",
      "#Lambda",
      "#Sigma^{0}",
      "#Xi^{-}",
      "#Delta^{++}",
      "#Sigma(1385)^{-}",
      "#Sigma(1385)^{+}",
      "#Sigma(1385)^{#pm}",
      "#Xi(1530)^{0}",
      "#Omega^{-}",
      "#Lambda(1520)"
   };



//sigma stuff
   const int N22 = 58;

//10GeV
   double sigmax1[N1] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
   double sigmax12[N22] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
   double line1[N22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   double sigmay1[N1];
   for(int i = 0; i < N1; i++){
      sigmay1[i] = (y1[i]-y12[i])/ey1[i];
   }

//29-35GeV
   double sigmax2[N2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
   double sigmax22[N22] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
   double line2[N22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   double sigmay2[N2];
   for(int i = 0; i < N2; i++){
      sigmay2[i] = (y2[i]-y22[i])/ey2[i];
   }

//91GeV

   double sigmax3[N3] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33};
   double sigmax32[N22] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
   double line3[N22] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   double sigmay3[N3];
   for(int i = 0; i < N3; i++){
      sigmay3[i] = (y3[i]-y32[i])/ey3[i];
   }

//130-200GeV
   double sigmax4[N4] = {1, 2, 3, 4, 5};
   double sigmax42[N22] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   double line4[N22] = {0, 0, 0, 0, 0};
   double sigmay4[N4];
   for(int i = 0; i < N4; i++){
      sigmay4[i] = (y4[i]-y42[i])/ey4[i];
   }

   TCanvas *c1 = new TCanvas("c1","sigma",100,500,1400,900);
   gStyle->SetOptStat(0); 
   c1->SetGridx();
   TGraphErrors *sigma1 = new TGraph(N1,sigmax1,sigmay1);   
   TGraphErrors *sigma2 = new TGraph(N2,sigmax2,sigmay2); 
   TGraphErrors *sigma3 = new TGraph(N3,sigmax3,sigmay3); 
   TGraphErrors *sigma4 = new TGraph(N4,sigmax4,sigmay4); 
   TGraph *line12 = new TGraph(N22,sigmax12,line1);  
   TGraph *line22 = new TGraph(N22,sigmax22,line2);  
   TGraph *line32 = new TGraph(N22,sigmax32,line3);  
   TGraph *line42 = new TGraph(N22,sigmax42,line4);  

   sigma1->SetMarkerStyle(20);
   sigma1->SetMarkerColor(1);
   sigma1->SetMarkerSize(1);
   sigma1->SetTitle("");
   line12->SetLineWidth(2);

   sigma1->SetMaximum(11);
   sigma1->SetMinimum(-11);

   sigma1->GetYaxis()->SetTitle("#Delta(#sigma)");
   sigma1->GetYaxis()->CenterTitle();
   sigma1->GetXaxis()->SetLabelSize(0);
   sigma1->GetYaxis()->SetTitleSize(0.12);
   sigma1->GetYaxis()->SetTitleOffset(0.2);
   sigma1->GetYaxis()->SetLabelSize(0.09);
   sigma1->GetYaxis()->SetNdivisions(510); 
   sigma2->SetMarkerStyle(20);
   sigma2->SetMarkerColor(1);
   sigma2->SetMarkerSize(1);
   sigma2->SetTitle("");
   line22->SetLineWidth(2);

   sigma2->SetMaximum(7);
   sigma2->SetMinimum(-7);

   sigma2->GetYaxis()->SetTitle("#Delta(#sigma)");
   sigma2->GetYaxis()->CenterTitle();
   sigma2->GetXaxis()->SetLabelSize(2);
   sigma2->GetYaxis()->SetTitleSize(0.12);
   sigma2->GetYaxis()->SetTitleOffset(0.2);
   sigma2->GetYaxis()->SetLabelSize(0.09);
   sigma2->GetYaxis()->SetNdivisions(506); 
   sigma3->SetMarkerStyle(20);
   sigma3->SetMarkerColor(1);
   sigma3->SetMarkerSize(1);
   sigma3->SetTitle("");
   line32->SetLineWidth(2);

   sigma3->SetMaximum(13);
   sigma3->SetMinimum(-13);

   sigma3->GetYaxis()->SetTitle("#Delta(#sigma)");
   sigma3->GetYaxis()->CenterTitle();
   sigma3->GetXaxis()->SetLabelSize(2);
   sigma3->GetYaxis()->SetTitleSize(0.12);
   sigma3->GetYaxis()->SetTitleOffset(0.2);
   sigma3->GetYaxis()->SetLabelSize(0.09);
   sigma3->GetYaxis()->SetNdivisions(510); 
   sigma4->SetMarkerStyle(20);
   sigma4->SetMarkerColor(1);
   sigma4->SetMarkerSize(1);
   sigma4->SetTitle("");
   line42->SetLineWidth(2);

   sigma4->SetMaximum(4.2);
   sigma4->SetMinimum(-4.2);

   sigma4->GetYaxis()->SetTitle("#Delta(#sigma)");
   sigma4->GetXaxis()->SetTitle("Particle species");
   sigma4->GetXaxis()->SetTitleSize(0.11);
   sigma4->GetXaxis()->SetTitleOffset(0.3);
   sigma4->GetYaxis()->CenterTitle();
   sigma4->GetXaxis()->CenterTitle();
   sigma4->GetXaxis()->SetLabelSize(2);
   sigma4->GetYaxis()->SetTitleSize(0.12);
   sigma4->GetYaxis()->SetTitleOffset(0.2);
   sigma4->GetYaxis()->SetLabelSize(0.09);
   sigma4->GetYaxis()->SetNdivisions(505); 
 
   TLatex l1;
   l1.SetTextSize(0.10);
   l1.SetTextAngle(90);
   l1.SetTextAlign(12); 

   TText *t1 = new TText(24,-9,"10GeV"); 
   t1->SetTextSize(0.1); 

   TLatex l2;
   l2.SetTextSize(0.10);
   l2.SetTextAngle(90);
   l2.SetTextAlign(12); 

   TText *t2 = new TText(20.5,-6.4,"29-35GeV"); 
   t2->SetTextSize(0.1); 
 
   TLatex l3;
   l3.SetTextSize(0.10);
   l3.SetTextAngle(90);
   l3.SetTextAlign(12);

   TText *t3 = new TText(33,-9,"91GeV"); 
   t3->SetTextSize(0.1); 

   TLatex l4;
   l4.SetTextSize(0.10);
   l4.SetTextAngle(90);
   l4.SetTextAlign(12);

   TText *t4 = new TText(4.8,-3.2,"130-200GeV"); 
   t4->SetTextSize(0.1); 

   c1->Divide(1,4,0.00002);

   c1_1->cd();
   gPad->SetTickx(1); 
   c1_1->SetGridy();
   sigma1->Draw("AP");
   t1->Draw("same"); 
   for (int i=0; i<N1; i++){
      l1.DrawLatex(sigmax1[i],sigmay1[i]+0.5,labels1[i]);
   }
   line12->Draw("same");
   c1_2->cd();
   c1_2->SetGridy();
   sigma2->Draw("AP");
   t2->Draw("same"); 
   for (int i=0; i<N2; i++){
      l2.DrawLatex(sigmax2[i],sigmay2[i]+0.5,labels2[i]);
   }
   line12->Draw("same");
   c1_3->cd();
   c1_3->SetGridy();
   sigma3->Draw("AP");
   t3->Draw("same"); 
   for (int i=0; i<N3; i++){
      l3.DrawLatex(sigmax3[i],sigmay3[i]+0.5,labels3[i]);
   } 
   line12->Draw("same");
   c1_4->cd();
   c1_4->SetGridy();
   sigma4->Draw("AP");
   for (int i=0; i<N4; i++){
      l4.DrawLatex(sigmax4[i],sigmay4[i]+0.3,labels4[i]);
   } 
   t4->Draw("same");
   line12->Draw("same");
   //c1->Close();

}
