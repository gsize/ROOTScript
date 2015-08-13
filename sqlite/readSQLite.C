void readSQLite()
{
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("readSQLite.C","");
	dir.ReplaceAll("/./","/");

	TString dbname(Form("sqlite://%s/../../NuclearDatabase/nucleidata.sqlite",dir.Data()));

	TSQLiteServer * f = new TSQLiteServer(dbname.Data());
//	cout<<f->ServerInfo()<<endl;

	TSQLiteRow *row;
	TSQLiteResult *res;
	int nfield;

	res = (TSQLiteResult*) f->GetTables("");
	nfield = res->GetFieldCount();
	while((row = (TSQLiteRow *) res->Next()))
	{
		TString str(row->GetField(nfield-1));
		printf("%s\t",str.Data());
		delete row;
	}
	printf("\n");
	delete res;

	char *sql = "select * from rayInfor where ENDFCode=922350 and rayType = \"G\" ";
	res = (TSQLiteResult*) f->Query(sql);
	nfield = res->GetFieldCount();
	printf(" nfield:%d\n",nfield);
	for(int i =0;i<nfield;i++)
		printf("%12s",res->GetFieldName(i));
	printf("\n");

	while((row = (TSQLiteRow *)( res->Next())))
	{
		printf("%12s%12s%12s%12s%12s\n",row->GetField(0),row->GetField(1),row->GetField(2),row->GetField(3),row->GetField(4));
		delete row;
	}
	delete res;

	f->Close();
}
