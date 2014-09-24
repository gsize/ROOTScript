/////////////////////////////////////////////////////////////////////////////////////
//
// Purpose: Book and fill TH1D, TH2D, TProfile histograms in one go: 
// the user doesn't need to book histograms prior to filling them.
//
// What it does: fills a histogram. If the histogram doesn't 
// exist yet, books it first. 
//
// Example 1: Fill two TH1D histos:
//
// FillHistograms* histo ;
// < Begin event loop >
//    histo->FillHisto( variable1, "histo_name1", nbinsx1, xmin1, xmax1 ) ;
//    histo->FillHisto( variable2, "histo_name2", nbinsx2, xmin2, xmax2 ) ;
// < End event loop > 
//
// Example 2: Fill TH2D histo and give it a title:
//
// FillHistograms* histo ;
// string histo_name = "run_number" ;
// histo->FillHisto( run, histo_name, "Run II: run number", 1000, 0., 1000. ) ;
//
// Example 3: Fill a profile histo (assuming all the variable have
//            already been assigned definite values):
//
// FillHistograms* histo ;
// histo->FillProf( var1, var2, "var2_vs_var1", nbinsx, xmin, xmax, ymin, ymax ) ;
//
//
// Author: Daniel Goldin (Texas A&M University)
//
/////////////////////////////////////////////////////////////////////////////////////

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "path-to-header-file/FillHistograms.hh"


/////////////////////////////////////////////////////////////////////////////////////
// 
// Constructor
//
FillHistograms::FillHistograms()
{
}

/////////////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
FillHistograms::~FillHistograms() 
{
}

/////////////////////////////////////////////////////////////////////////////////////
//
// Book and fill 1D histo
//
void FillHistograms::FillHisto( double var, const string& hname, 
				int nbins, float xmin, float xmax ) 
{
  // If histo not already booked, book it
  if ( !gDirectory->FindObject( hname.c_str() ) ) {
    std::vector<TH1D*> vectHist ;
    vectHist.push_back( new TH1D( hname.c_str(), "", nbins, xmin, xmax ) )  ;
  }

  // Fill histo
  ((TH1D*)gDirectory->Get(hname.c_str()))->Fill( var ) ;

}

/////////////////////////////////////////////////////////////////////////////////////
// 
// Book and fill 1D histo with Title
//
void FillHistograms::FillHisto( double var, const string& hname, const string& htit, 
				int nbins, float xmin, float xmax ) 
{
  // If histo not already booked, book it
  if ( !gDirectory->FindObject( hname.c_str() ) ) {
    std::vector<TH1D*> vectHist ;
    vectHist.push_back( new TH1D( hname.c_str(), "", nbins, xmin, xmax ) )  ;
    vectHist[vectHist.size()-1]->SetXTitle( htit.c_str() ) ;
  }

  // Fill histo
  ((TH1D*)gDirectory->Get(hname.c_str()))->Fill( var ) ;

}


/////////////////////////////////////////////////////////////////////////////////////
//
// Book and fill 2D histo
//
void FillHistograms::FillHisto( double var1, double var2, const string& hname, 
				int nbinsx, float xmin, float xmax, 
				int nbinsy, float ymin, float ymax ) 
{
  // If histo not already booked, book it
  if ( !gDirectory->FindObject( hname.c_str() ) ) {
    std::vector<TH2D*> vectHist ;
    vectHist.push_back( new TH2D( hname.c_str(), "", nbinsx, xmin, xmax,
                                  nbinsy, ymin, ymax ) )  ;
  }

  // Fill histo
  ((TH2D*)gDirectory->Get(hname.c_str()))->Fill( var1, var2 ) ;

}

/////////////////////////////////////////////////////////////////////////////////////
//
// Book and fill 2D histo with Title
//
void FillHistograms::FillHisto( double var1, double var2, const string& hname, 
				const string& htit,
				int nbinsx, float xmin, float xmax, 
				int nbinsy, float ymin, float ymax ) 
{
  // If histo not already booked, book it
  if ( !gDirectory->FindObject( hname.c_str() ) ) {
    std::vector<TH2D*> vectHist ;
    vectHist.push_back( new TH2D( hname.c_str(), "", nbinsx, xmin, xmax,
                                  nbinsy, ymin, ymax ) )  ;
    vectHist[vectHist.size()-1]->SetXTitle( htit.c_str() ) ;
  }

  // Fill histo
  ((TH2D*)gDirectory->Get(hname.c_str()))->Fill( var1, var2 ) ;

}

/////////////////////////////////////////////////////////////////////////////////////
//
// Book and fill a profile histo
//
void FillHistograms::FillProf( double var1, double var2, const string& hname, 
			       int nbins, float xmin, float xmax, 
			       float ymin, float ymax ) 
{
  // If histo not already booked, book it
  if ( !gDirectory->FindObject( hname.c_str() ) ) {
    std::vector<TProfile*> vectHist ;
    vectHist.push_back( new TProfile( hname.c_str(), "", nbins, xmin, xmax, ymin, ymax )
                        )  ;
  }

  // Fill histo
  ((TProfile*)gDirectory->Get(hname.c_str()))->Fill( var1, var2, 1. ) ;

}

/////////////////////////////////////////////////////////////////////////////////////
//
// Book and fill a profile histo with Title
//
void FillHistograms::FillProf( double var1, double var2, const string& hname, 
			       const string& htit,
			       int nbins, float xmin, float xmax, 
			       float ymin, float ymax ) 
{
  // If histo not already booked, book it
  if ( !gDirectory->FindObject( hname.c_str() ) ) {
    std::vector<TProfile*> vectHist ;
    vectHist.push_back( new TProfile( hname.c_str(), "", nbins, xmin, xmax, ymin, ymax )
                        )  ;
    vectHist[vectHist.size()-1]->SetXTitle( htit.c_str() ) ;
  }

  // Fill histo
  ((TProfile*)gDirectory->Get(hname.c_str()))->Fill( var1, var2, 1. ) ;

}

