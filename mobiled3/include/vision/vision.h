//
// vision.h
// (C) 01/2002 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include "image/imageClient.h"
#include "vision/modelClassifier.h"
#include "infobase/infobase.h"


class CVISION
{
  friend class CROBOTD;

private:

  // not even implemented
  CVISION (void);
  CVISION (const CVISION &);
  CVISION operator= (const CVISION &);

protected:

  unsigned int _flags;

  CINFOBASE &_infoBase;
  CIMAGECLIENT *_imageClient;
  CMODELCLASSIFIER _classifier;

  //
  // Grab() -- ideally a private routine, but for debugging purposes used by
  // CROBOTD. note: does not obey `_flags'!
  //

  void Grab (CIMAGE &image, const bool verbose = true) const;

public:  

  CVISION (CINFOBASE &infoBase, const unsigned int flags);
  virtual ~CVISION (void);

  //
  //
  //

  int VisionGetRegionCenter (const double color,
			     double *dx,
			     double *dy) const;

  //
  // return the number of a model
  //

  int FindModel (const char *devName) const;

  //
  // confirm that an object is of the given model and compute its
  // displacement. if the best matching model does not fit this function stays
  // in an endless loop until it does.
  //

  int ConfirmModel (const char *devName,
		    double *dx, double *dy, double *angle) const;

  //
  // find model by name/index and compute displacement.
  //

  int AnalyzeModel (const char *devName,
		    double *dx, double *dy, double *angle,
		    double *error = 0x0) const;

  int AnalyzeModel (const int idx,
		    double *dx, double *dy, double *angle,
		    double *error = 0x0) const;

  //
  // ++itschere20031125: used for debugging only, returns values unscaled and
  // is not very insisting.
  //

  int AnalyzeModelRaw (const int idx,
		       double &dx, double &dy, double &da,
		       double &error,
		       const bool verbose) const;
};
