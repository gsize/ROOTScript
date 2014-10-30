void read_kk_file() {
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("read_kk_file.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;
	pathName = dir + "kk.txt";

	ifstream in;
	in.open(pathName.Data());
	if(!(in.is_open()))
	{
		cout<<"can't open file!";
		return ;
	}

	vector <double> xList;
	vector <double> yList;
	string str_tmp;
	while(getline(in,str_tmp)) {
		double x,y,z; 
		sscanf(str_tmp.c_str(),"%lf %lf %lf",&x, &y,&z);
		xList.push_back(x);
		yList.push_back(z);
		cout <<x <<"\t"<<z <<endl;

	}
	in.close();

	TGraph *gr=new TGraph((Int_t)xList.size(),&xList[0],&yList[0]);
	gr->Draw("A*");

}
