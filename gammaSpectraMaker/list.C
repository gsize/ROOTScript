
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
	ofstream out("nuclearListCSV1.txt");
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
	char  s1[15],s2[15],s3[15],s4[15];
	TString str22,str33;

	while(getline(in,str_tmp)) 
	{
		TString str(str_tmp);
		if(str.Contains((TPRegexp)re_start1))
		{
			//char s1[10],s2[10];
			sscanf(str.Data(),"%s %s %d",s1,s2,&d1);
			TString strt(s1);
			d1 = elemNR2ENDFCode(strt);
			//out<<d1<<" "<<str<<endl;
		}
		else 
		{
			sscanf(str.Data(),"%s %s %s %s",s1,s2,s3,s4);
			if(str22.Contains(s2) && str33.Contains(s3))
			{
				continue;
			}
			str22 = s2;
			str33 = s3;
			out<<"\""<<d1<<"\",\""<<s1<<"\",\""<<s2<<"\",\""<<s3<<"\",\""<<s4<<"\""<<endl;
		}
	}
	in.close();
	out.close();
}
