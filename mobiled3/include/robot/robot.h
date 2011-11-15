//
// robot.h
// (C) 08/2000 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#ifndef _ROBOT_H
#define _ROBOT_H

#include "labdev/labdev.h"


class CROBOT : public CLABDEV
{
private:

  // not even implemented
  CROBOT (const CROBOT &);
  CROBOT &operator= (const CROBOT &);

protected:

public:

  CROBOT (void) : CLABDEV ("robot") {};
  virtual ~CROBOT (void) {}

  // basic functionality

#if 0
  int Init (void);
  int Park (void);
#endif

  int Hand (const int open);

  // this is just enough for the simplified scenario

  int PickTubeStorage (void);
  int HoldTubeSampler (void);
  int TakeTubeSampler (void);

  int PlaceTubeCentrifuge (void);   // ++itschere20010415
  int RunCentrifuge (void);   // ++itschere20010415
  int PickTubeCentrifuge (void);   // ++itschere20010415

  int HoldTubePipette (const int numShake, const double depth);
  int TakeTubePipette (void);
  int PlaceTubeWaste (void);

  int PlaceTubeFridge (void);   // ++itschere20010415

  int PlaceTubeStorage (void);   // ++itschere20010417
  int OpenCentrifuge (void);   // ++itschere20010417
  int CloseCentrifuge (void);   // ++itschere20010417
  int CloseCentrifugeReally (void);   // ++itschere20010716

  int ShakeTube (void);   // ++itschere20010710

  int HoldTubeScanner (void);   // ++itschere20010723
  int TakeTubeScanner (void);   // ++itschere20010723

  //
  // if you want a more optimized way of centrifuge handling, take these:
  //
  // `LoadAndRunCentrifuge()' is a time saving combination of the former (still
  // working) `PlaceTubeCentrifuge()', `CloseCentrifuge()',
  // `CloseCentrifugeReally()' and `RunCentrifuge()'.
  //
  // `StopAndUnloadCentrifuge()' is a time saving combination of the former
  // (still working) `OpenCentrifuge()' and `PickTubeCentrifuge()'. note that
  // it does not actually stop the centrifuge!
  //
  // time saving is achieved by ommitting the redundant fine positioning for
  // the second and later calls - they're not necessary if the mobile platform
  // hasn't moved in the meantime.
  //

  int LoadAndRunCentrifuge (void); // ++itschere20020122
  int StopAndUnloadCentrifuge (void);   // ++itschere20020122

  //
  //
  //

  int OpenFridge (void);   // ++itschere20020129
  int CloseFridge (void);   // ++itschere20020129

  int PlaceTubeAndCloseFridge (void);   // ++itschere20020131

  // ++itschere20021024

  int PickCedexStorage (void);
  int PlaceCedexCedex (void);
  int PickCedexCedex (void);
  int PlaceCedexWaste (void);

  int PickTubeStorageBarcode (void);   // ++itschere20021114
  int PlaceTubeStorageBarcode (void);   // ++itschere20021114

  int HoldCedexPipette (const double depth = 0.0);   // ++itschere20021121
  int TakeCedexPipette (void);   // ++itschere20021121

  int RotateTubeScanner (void);   // ++itschere2001122

  int ParkCharger (void);   // ++itschere20030317
  int UnparkCharger (void);   // ++itschere20030317

  //

  int HoldTubeSampler2 (void);   // ++itschere20030502
  int TakeTubeSampler2 (void);   // ++itschere20030502
};

#endif // _ROBOT_H
