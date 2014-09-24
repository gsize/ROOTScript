
#include <vector>
#include <TH3F.h>
#include <TPolyMarker3D.h>
#include <TStyle.h>
#include <fstream>
#include <iostream>
#include <TCanvas.h>
/*
 Parse input string
 */
std::vector<string> splitString(const std::string& str,
						   const char* separator) {
    vector<string> result;
    string::size_type prev_pos=0;
    string::size_type pos=0;
    while ((pos = str.find_first_of(separator, prev_pos)) != string::npos) {
		if (prev_pos < pos) {
			result.push_back(str.substr(prev_pos, pos-prev_pos));
		}
		prev_pos = pos + 1;
    }
    if (prev_pos < str.size()) {
		result.push_back(str.substr(prev_pos));
    }
    return result;
}



void plotBad(string infile)
{
	
	TCanvas *c1 = new TCanvas("Bad", "Bad", 0, 0, 500, 500);
	c1->cd();

	
	gStyle->SetOptStat(0);
	
	TH3F *frame3d = new TH3F("Bad","Bad",10,-1,4,10,-1,4,10,-1,4);
	frame3d->Draw();
	
	vector<TPolyMarker3D*> markers;
	
	ifstream in(infile.c_str());
	if(!in.is_open()){
		cerr<<"couldn't open "<<infile<<endl;
		return;
	}
	string str;
	
	int kk=0;
	while(getline(in, str)){
		vector<string> splits = splitString(str, " ");

		float x = atof(splits[0].c_str());
		float y = atof(splits[1].c_str());
		float z = atof(splits[2].c_str());
		
		float* vals = new float[3];
		vals[0]=x;
		vals[1]=y;
		vals[2]=z;

		markers.push_back(new TPolyMarker3D(3, vals));
		markers.back()->SetMarkerStyle(20);
		markers.back()->SetMarkerColor(2);
		markers.back()->Draw();
		kk++;
		printf("%d\t%f\t%f\t%f\n",kk,vals[0],vals[1],vals[2]);
	}
}

void plotGood(string infile)
{
	gStyle->SetOptStat(0);
	
	TCanvas *c1 = new TCanvas("demo1", "Good", 500, 0, 500, 500);
	c1->cd();

	TH3F *frame3d = new TH3F("Good","Good",10,-1,4,10,-1,4,10,-1,4);
	frame3d->Draw();
	
	std::vector<TPolyMarker3D * > markers;
	
	ifstream in(infile.c_str());
	if(!in.is_open()){
		cerr<<"couldn't open "<<infile<<endl;
		return;
	}
	string str;
	
	while(getline(in, str)){
		vector<string> splits = splitString(str, " ");

		float x = atof(splits[0].c_str());
		float y = atof(splits[1].c_str());
		float z = atof(splits[2].c_str());

		float* vals = new float[3];
		vals[0]=x;
		vals[1]=y;
		vals[2]=z;

		markers.push_back(new TPolyMarker3D(3));
		markers.back()->SetPoint(0, x, y, z);
		markers.back()->SetMarkerStyle(20);
		markers.back()->SetMarkerColor(2);
		markers.back()->Draw();
	}
}

void demo(string infile = "data.dat"){
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("demo.C","");
	dir.ReplaceAll("/./","/");
	TString pathName;
	pathName = dir + "data.dat";
	infile = pathName;
	cout << infile<< endl;
	plotGood(infile);
	plotBad(infile);
}
