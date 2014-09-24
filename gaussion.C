void gaussion()
{
	double dlt=0.1;
	double ccc=0.;
	double cc=0.;
	double rd[61];
	for(int j=0; j<=60; j++)
	{
		ccc=j-30.45;
		rd[j]=0.0;
		for (int i=0; i<=9; i++)
		{
			cc=-ccc*ccc/32.0;
			rd[j]+= exp(cc);
			ccc+=dlt;
		}
	}
	TH1D *d4 = new TH1D("d4","",62,0.,62.);
	for (int i = 0; i < 61; i++) d4->SetBinContent(i + 1,rd[i]);
	d4->Draw();
}
