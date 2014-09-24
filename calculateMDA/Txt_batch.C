void Txt_batch()
{
	 TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("Txt_batch.C","");
   dir.ReplaceAll("/./","/");
    TString pathFile;
    pathFile = dir + "U_Target.TXT";
    ifstream in;
    in.open(pathFile.Data());

    int nlines=0;
    int nlist=0;
    int list_flg=0;
    int iflg=0;
    int page=0;
    int flg_r=0;

    string str_tmp;
    vector<string> vec_str;

    while(getline(in,str_tmp)) {
        //if(!in.good()) break;
        //getline(in,str_tmp);
        nlines++;
        if(iflg) {
            nlist++;
            list_flg++;
            if(list_flg<5) {
                if(str_tmp.find("CURIES")!=string::npos) {
                    flg_r=1;
                    //printf("%d\n",list_flg);
                    page++;
                }
                if(page==1)
                if(list_flg==4)
                    vec_str.push_back(str_tmp);
                if(list_flg==2){
                    if(!flg_r)
                    {
                    list_flg=0;
                    iflg=0;
                    flg_r=0;
                    }
                }
                continue;
            }
            if(flg_r) {
                if(list_flg>61 || str_tmp.find("TOTAL")!=string::npos) {
                        if(str_tmp.find("TOTAL")!=string::npos)
                        {
                            vec_str.push_back(str_tmp);
                            break;
                        }
                    list_flg=0;
                    iflg=0;
                    flg_r=0;
                    continue;
                }
                vec_str.push_back(str_tmp);
            }
        }
        if(str_tmp.find("+")!=string::npos && str_tmp.find("FISSION PRODUCTS")!=string::npos) {
            iflg=1;
            //printf("%d\n",nlines);
        }

    }
    printf("ok\n page=%d\nnlines=%d\n",page,nlines);
	TString outFile;
	outFile = dir +"outdata4.txt";
    ofstream out;
    out.open(outFile.Data());
    for(int ii=0; ii<vec_str.size(); ii++)
        out<<vec_str[ii]<<endl;
    out.close();
    in.close();
}
