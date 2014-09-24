#if !defined (__CINT__) || defined (__MAKECINT__)
#include "TObject.h"
#endif

using namespace std ;

class FillHistograms : public TObject {

public:

  FillHistograms();

  ~FillHistograms();

  // Book and fill 1D histo
  void FillHisto( double var, const string& hname, 
 		  int nbins, float xmin, float xmax ) ;

  // Book and fill 1D histo with Title
  void FillHisto( double var, const string& hname, const string& htit, 
 		  int nbins, float xmin, float xmax ) ;

  // Book and fill 2D histo
  void FillHisto( double var1, double var2, const string& hname, 
		  int nbinsx, float xmin, float xmax, 
		  int nbinsy, float ymin, float ymax ) ;

  // Book and fill 2D histo with Title
  void FillHisto( double var1, double var2, const string& hname, const string& htit,
		  int nbinsx, float xmin, float xmax, 
		  int nbinsy, float ymin, float ymax ) ; 
  
  // Book and fill a profile histo
  void FillProf( double var1, double var2, const string& hname, 
		 int nbins, float xmin, float xmax ,float ymin, float ymax ) ;

  // Book and filla a profile histo with Title
  void FillProf( double var1, double var2, const string& hname, const string& htit,
		 int nbins, float xmin, float xmax ,float ymin, float ymax ) ;

};
