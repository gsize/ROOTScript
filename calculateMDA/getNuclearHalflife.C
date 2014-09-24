void getNuclearHalflife()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("getNuclearHalflife.C","");
	dir.ReplaceAll("/./","/");
	TString pathFile;
	pathFile = dir + "nuclear.txt";
	ifstream in;
	in.open(pathFile);
	if(!(in.is_open()))
	{
		printf("open file failed!");
		return;
	}
	TGeoManager *geom = new TGeoManager("","");
	TGeoElementTable *table = gGeoManager->GetElementTable();

	string str_tmp;
	vector<string> elemStr_list;
	vector<string> elemStrRN_list;
	vector<int> elemStrRN_A;
	vector<double> elemHalflife_list;

	int flag_list=0;
	while(getline(in,str_tmp)) {
		flag_list++;
		if(flag_list>58){
			char str1[8],str2[10],str3[8];
			int elemA;
			int iso=0;
			cout<<str_tmp<<endl;
			sscanf(str_tmp.c_str(),"%s%s%s",str1,str3,str2);
			elemStr_list.push_back(str1);
			elemStrRN_list.push_back(str2);
			string strt(str3);
			if(strt.find("M")!=string::npos)
			{
				iso=1;
			}
			elemA=atoi(str3);
			TGeoElement *element = table->FindElement(str1);
			TGeoElementRN *elemStrRN = table->GetElementRN(elemA,element->Z(),iso);
			if(elemStrRN->IsRadioNuclide())
			{
				double halflife;
				halflife=elemStrRN->HalfLife();
				//printf("elemetRN:%s\thalflife:%lf\n",elemStrRN->GetName(),halflife);
				elemHalflife_list.push_back(halflife);
			}
		}
	}
	TString outFile;
	outFile = dir + "outdata_halflife.txt";
	ofstream out;
	out.open(outFile.Data());
	for(int ii=0; ii<elemStrRN_list.size(); ii++)
		out<<elemStrRN_list[ii]<<"  "<<elemHalflife_list[ii]<<endl;
	out.close();

	in.close();
}
