#include "F5Calc.h"
#include <cstdlib>

namespace tallySystem {

  void F5Calc::SetTally(double lx,double ly,double lz)
  {
    F5.x=lx; F5.y=ly; F5.z=lz;
  };

  void F5Calc::SetPoints(point lB,point lC, point lB2)
  {
    B = lB; C = lC; B2 = lB2;
  };

  void F5Calc::SetLength(double laf)
  {
    CalculateMiddleOfModerator();
    af=laf;

    double mf=sqrt(pow(F5.x-M.x,2.0)+pow(F5.y-M.y,2.0)+pow(F5.z-M.z,2.0));
    {
      A.x=F5.x+af/mf*(M.x-F5.x);
      A.y=F5.y+af/mf*(M.y-F5.y);
      A.z=F5.z+af/mf*(M.z-F5.z);
      
      // check if A belongs to MF5:
      double deltaXY = (A.x-M.x)*(F5.y-M.y) - (A.y-M.y)*(F5.x-M.x);
      double deltaXZ = (A.x-M.x)*(F5.z-M.z) - (A.z-M.z)*(F5.x-M.x);
      double epsilon = 1E-5;
      if ((deltaXY<epsilon) && (deltaXZ<epsilon)) {
      }
    }
  }

  void F5Calc::CalculateMiddleOfModerator()
  {
    M.x=C.x+(B.x-C.x)/2.0+(B2.x-B.x)/2.0;
    M.y=C.y+(B.y-C.y)/2.0+(B2.y-B.y)/2.0;
    M.z=C.z+(B.z-C.z)/2.0+(B2.z-B.z)/2.0;
  };

  double F5Calc::GetZAngle()
  {
    //The angle is calculated between the plane, which is normal to the moderator surface and includes point M, and the line M->F5
    //The angle is negative when the F5 tally is below the plane

    point lF5_Projected;
    point lNV; //Normalvector of the plane containing B,C and B2
    point lNV2; //Normalvector of the plane normal to the plane containing B,C and B2
    point lM;

    lM.x=M.x+(B.x-C.x)/2.0;
    lM.y=M.y+(B.y-C.y)/2.0;
    lM.z=M.z+(B.z-C.z)/2.0; 
    CalculateNormalVector(B,C,B2,lNV); 
    lNV2.x=lNV.y*(lM.z-M.z)-lNV.z*(lM.y-M.y);
    lNV2.y=lNV.z*(lM.x-M.x)-lNV.x*(lM.z-M.z);
    lNV2.z=lNV.x*(lM.y-M.y)-lNV.y*(lM.x-M.x); 
    double lLength=CalculateLength(lNV2);
    lNV2.x/=lLength; lNV2.y/=lLength; lNV2.z/=lLength; 
    CalculateProjectionOntoPlane(lNV2,M,F5,lF5_Projected); 

    double lAngle=fabs(atan( CalculateDistance(F5,lF5_Projected)/CalculateDistance(M,lF5_Projected) )/M_PI*180.0);
    if(lF5_Projected.z-F5.z >= 0.0) return -lAngle;
    else return lAngle;
  };

  double F5Calc::GetXYAngle()
  {
    /*The angle is calculated between
      1. the line consisting of
      a. M (middle of the moderator)
      b. lF5_Projected (the projection of the F5 tally onto the plane which is perpendicular to the moderator surface and includes M)
      2. the line which is perpendicular to the moderator surface and goes through M (middle of the moderator)
    */

    return 180.0+atan2(F5.y-M.y, F5.x-M.x)*180/M_PI;
  };

  double F5Calc::GetHalfSizeX()
  {
    double val = CalculateDistance(B,C)/2.0*af/CalculateDistance(F5,M);
    // Correction if BC is not perpendicular to M F5:
    double ab = (F5.x-M.x)*(B.x-C.x) + (F5.y-M.y)*(B.y-C.y);
    double len = sqrt(pow(F5.x-M.x, 2) + pow(F5.y-M.y, 2)) * sqrt(pow(B.x-C.x, 2) + pow(B.y-C.y, 2));
    double cosa = ab/len;
    val *= cos(M_PI/2-acos(cosa));
    return val;
  };

  double F5Calc::GetHalfSizeZ()
  {
    return CalculateDistance(B,B2)/2.0*af/CalculateDistance(F5,M);
  };

  void F5Calc::CalculateNormalVector(point P1,point P2,point P3,point &P4)
  {
    P4.x=(P1.y-P3.y)*(P1.z-P2.z)-(P1.z-P3.z)*(P1.y-P2.y);
    P4.y=(P1.z-P3.z)*(P1.x-P2.x)-(P1.x-P3.x)*(P1.z-P2.z);
    P4.z=(P1.x-P3.x)*(P1.y-P2.y)-(P1.y-P3.y)*(P1.x-P2.x);
    
    double lLength=sqrt(pow(P4.x,2.0)+pow(P4.y,2.0)+pow(P4.z,2.0));
    P4.x/=lLength;
    P4.y/=lLength;
    P4.z/=lLength;
  };

  double F5Calc::CalculateLength(point P1)
  {
    return sqrt(pow(P1.x,2.0)+pow(P1.y,2.0)+pow(P1.z,2.0));
  };

  void F5Calc::CalculateProjectionOntoPlane(point NV,point M,point P,point &P2)
  {
    //NV is the normal vector of the plane
    //M is a point on the plane
    //P is a point which projection (P2) is calculated on the plane

    double lDistance=fabs(NV.x*(M.x-P.x)+NV.y*(M.y-P.y)+NV.z*(M.z-P.z))/sqrt(pow(NV.x,2.0)+pow(NV.y,2.0)+pow(NV.z,2.0));

    //Calculate the possible projections
    point lProjection1,lProjection2;
    lProjection1.x=P.x+NV.x*lDistance;
    lProjection1.y=P.y+NV.y*lDistance;
    lProjection1.z=P.z+NV.z*lDistance;
    lProjection2.x=P.x-NV.x*lDistance;
    lProjection2.y=P.y-NV.y*lDistance;
    lProjection2.z=P.z-NV.z*lDistance;

    //The smaller distance is the correct projection
    if(CalculateDistance(M,lProjection1)<=CalculateDistance(M,lProjection2))
      {
	P2.x=lProjection1.x;
	P2.y=lProjection1.y;
	P2.z=lProjection1.z;
      }
    else
      {
	P2.x=lProjection2.x;
	P2.y=lProjection2.y;
	P2.z=lProjection2.z;
      }
  };


  double F5Calc::CalculateDistance(point P1, point P2)
  {
    return sqrt(pow(P1.x-P2.x,2.0)+pow(P1.y-P2.y,2.0)+pow(P1.z-P2.z,2.0));
  };

}
