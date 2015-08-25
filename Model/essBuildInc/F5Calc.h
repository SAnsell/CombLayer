#ifndef _tallySystem_F5Calc_
#define _tallySystem_F5Calc_

#include<iostream>
#include<cmath>

using namespace std;

namespace tallySystem {

  struct point
  {
    double x,y,z;
  };

  class F5Calc
  {
    point B,C,B2; ///< Moderator corners

    point F5; ///< F5 tally coordinates

    double af; //Distance of the middle of the collimator entrance from the point detector
    point A; //Coordinates of point A, the start of the collimator on the M->F5 line

    point M; //Middle of the moderator
    
    void CalculateNormalVector(point P1,point P2,point P3, point &P4);
    double CalculateLength(point P1);
    void CalculateProjectionOntoPlane(point NV, point M, point P, point &P2);
    double CalculateDistance(point P1, point P2);

  public:
    void SetTally(double,double,double); //Set coordinates for the F5 tally
    void SetPoints(point,point,point); //Set coordinates for the moderator
    void SetLength(double); //Set position of the entrance of the collimator

    void CalculateMiddleOfModerator();
    double GetZAngle();
    double GetXYAngle();
    double GetHalfSizeX(); //Get horizontal dimension of the collimator
    double GetHalfSizeZ(); //Get vertical dimension of the collimator
    double GetOmega() { return GetHalfSizeX()*GetHalfSizeZ()*4.0/af/af; } // returns the collimator solid angle
  };
}

#endif
