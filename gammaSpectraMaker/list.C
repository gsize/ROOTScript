
int elemNR2ENDFCode(TString elementName)
{
	int ENDFCode;

	TPRegexp re("^[A-Z][a-z]?-\\d+m?");
	TPRegexp re1("^[A-Z][a-z]?-\\d+");
	TPRegexp re2("^[A-Z][a-z]?-\\d+m$");

	TGeoElementTable *table = gGeoManager->GetElementTable();
	if(elementName.Contains(re))
	{
		int a=0,z=0,iso=0;
		char str[4];

		sscanf(elementName.Data(),"%[A-Za-z]-%d",str,&a);
		TGeoElement *elem = table->FindElement(str);
		z = elem->Z();

		if(elementName.Contains(re2))
			iso=1;
		else
			iso=0;

		ENDFCode = 10000*z+10*a+iso;
	}
	else
		ENDFCode=0;

	return ENDFCode;
}

void list()
{
	TGeoManager *geom = new TGeoManager("","");
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("list.C","");
	dir.ReplaceAll("/./","/");

	ifstream in;
	ofstream out("outfile.txt");
	in.open(Form("%s/../../NuclearDatabase/icrp-07.rad",dir.Data()));
	if(!(in.is_open()))
	{
		cout<< "open in file failed!"<<endl;
		return ;
	}

	string str_tmp;
	int i=1;
	TPMERegexp re_start1("^[a-zA-Z]+-\\d+");
	int d1=0;
	while(getline(in,str_tmp)) 
	{
		TString str(str_tmp);
		if(str.Contains((TPRegexp)re_start1))
		{
			char s1[10],s2[10];
			sscanf(str.Data(),"%s %s %d",s1,s2,&d1);
			TString strt(s1);
			d1 = elemNR2ENDFCode(strt);
		}
		else 
		{
			out<<d1<<" "<<str<<endl;
		}
	}
	in.close();
	out.close();
}
