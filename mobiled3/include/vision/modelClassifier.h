//
//
//


#ifndef _MODELCLASSIFIER_H_
#define _MODELCLASSIFIER_H_

#include "model.h"

// a class supposed to hold the pool of all models and provide means to
// classify an unknown model

class CMODELCLASSIFIER
{
private:

  // not even implemented
  CMODELCLASSIFIER (const CMODELCLASSIFIER &);
  CMODELCLASSIFIER &operator= (const CMODELCLASSIFIER &);

protected:

  int _numModels;
  CMODEL **_model;

public:

  CMODELCLASSIFIER (void);
  ~CMODELCLASSIFIER (void);

  // 

  int AddModel (CMODEL *model);

  // this one compares two models and computes a `top 10' list of evaluations
  // of possible permutated region assignments. it returns the number of
  // solutions found and evaluated, even if this may be larger than 10.

  typedef struct
  {
    double angle;
    double dx, dy;
    double err;
  } RESULT;

  // Classify() classifies a given model by walking thru the list of known
  // models. it returns either the (positive) index of the model or something
  // negative if no match could be found. in any case the values from the best
  // match (possible garbage) are returned in `res' if res!=0.

  int Classify (const CMODEL &model, RESULT *res = 0x0) const;

  // Confirm() classifies a given model by walking thru the list of known
  // models. if the best model found has the name as given in `name' its
  // (positive) index is returned, otherwise something negative. in any case
  // the values from the best match (possible garbage) are returned in `res' if
  // res!=0.

  int Confirm (const CMODEL &model, const char *name, RESULT *res = 0x0) const;

  // Analyze() can be used once the model has been verified and only
  // positioning data of one model is needed. it returns either the (positive)
  // number of values returned in `res' (not more than 10) or something
  // negative if no match could be found.

  int Analyze (const CMODEL &model, const int idx, RESULT *res) const;

#if 1

  int GetNumModels (void) const;

  CMODEL *GetModel (const int idx) const;

#else

  int GetNumModels (void) const
    {
      return _numModels;
    }

  CMODEL *GetModel (const int idx) const
  {
    return _model[idx];
  }

#endif

  void DumpModels (void) const;
};


#endif
