
void list()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("list.C","");
	dir.ReplaceAll("/./","/");

	ifstream in;
	in.open(Form("%s/../../NuclearDatabase/icrp-07.rad",dir.Data()));
	if(!(in.is_open()))
	{
		cout<< "open in file failed!"<<endl;
		return ;
	}

	string str_tmp;
	vector<string> vec_str;
    int i=1;
	TPMERegexp re_start1("^[a-zA-Z]+");
	while(getline(in,str_tmp)) 
	{
		TString str(str_tmp);
		//vec_str.push_back(str_tmp);

        //TString str(vec_str[ii]);
		if(str.Contains((TPRegexp)re_start1))
		{
			cout<<i<<"\t"<<str<<endl;
				i++;

		}
	}
	in.close();
}
