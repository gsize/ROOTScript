// origen2的输出文件格式化

void Origen2OutputFileFormat(TString fname="TAPE20d.OUT")
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("Origen2OutputFileFormat.C","");
	dir.ReplaceAll("/./","/");

	ifstream in;
	in.open(Form("%sdata/%s",dir.Data(),fname.Data()));
	if(!(in.is_open()))
	{
		cout<< "open in file failed!"<<endl;
		return ;
	}

	string str_tmp;
	vector<string> vec_str;
	TPMERegexp re_start("^\\+ +FISSION PRODUCTS +");
	TPMERegexp re_start1("^0 +[0-9a-zA-Z :,]+RADIOACTIVITY...CURIES +");
	TPMERegexp re_end("^1 +OUTPUT");
	TPMERegexp re4("^ +TOTAL ");
	TPMERegexp re(".{12,12}( [0-9]+(\\.[0-9]+)(E|e)(\\+|\\-)[0-9]{2,2}){10,10}");

	while(getline(in,str_tmp)) 
	{
		TString str(str_tmp);
		if(str.Contains((TPRegexp) re_start))
		{
			getline(in,str_tmp);
			getline(in,str_tmp);
			str=str_tmp;
			if(str.Contains((TPRegexp) re_start1))
			{
				getline(in,str_tmp);
				getline(in,str_tmp);
				while(getline(in,str_tmp)) {
					str=str_tmp;
					if(str.Contains((TPRegexp) re_end))
						break;
					vec_str.push_back(str_tmp);
				}
			}
		}
	}
	in.close();
	TString outFile;
	outFile = dir + fname;
	outFile += ".txt";
	ofstream out;
	out.open(outFile.Data());
	if(!(out.is_open()))
	{
		cout<< "open in file failed!"<<endl;
		return ;
	}
	for(int ii=0; ii<vec_str.size(); ii++)
		out<<vec_str[ii]<<endl;
	out.close();
}
