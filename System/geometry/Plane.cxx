/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Plane.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "RegMethod.h"
#include "MemStack.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Plane& A)
  /*!
    Output stream
    \param OX :: output stream
    \param A :: Plane to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}

Plane::Plane() : Quadratic(),
  NormV(1.0,0.0,0.0),Dist(0.0)
  /*!
    Constructor: sets plane in y-z plane and throught origin
  */
{
  setBaseEqn();
}

Plane::Plane(const int N,const int T) : Quadratic(N,T),
  NormV(1.0,0.0,0.0),Dist(0.0)
  /*!
    Constructor: sets plane in y-z plane and throught origin
    \param N :: Name
    \param T :: Transform number
  */
{
  setBaseEqn();
}

Plane::Plane(const int N,const int T,
	     const Geometry::Vec3D& P,
	     const Geometry::Vec3D& NVec) : Quadratic(N,T)
  /*!
    Constructor: sets plane at a point and direction
    \param N :: Name
    \param T :: Transform number
    \param P :: Point for plane to pass thought
    \param NVec :: Normal for the plane
  */
{
  setPlane(P,NVec);
}

Plane::Plane(const Plane& A) : Quadratic(A),
   NormV(A.NormV),Dist(A.Dist)
  /*!
    Copy Constructor
    \param A :: Plane to copy
  */
{}

Plane*
Plane::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return new(this)
  */
{
  return new Plane(*this);
}

Plane&
Plane::operator=(const Plane& A) 
  /*!
    Assignment operator
    \param A :: Plane to copy
    \return *this
  */
{
  if (&A!=this)
    {
      this->Quadratic::operator=(A);
      NormV=A.NormV;
      Dist=A.Dist;
    }
  return *this;
}

Plane::~Plane()
  /*!
    Destructor
  */
{}

bool
Plane::operator==(const Plane& A) const
  /*!
    Determine if two plane are equal (within tolerance).
    Note : planes can have opposite signed Normals and D
    BUT that is not tested here
    \param A :: Plane to compare
    \return A==This
  */
{
  if (&A==this) return 1;

  if (fabs(A.Dist-Dist)<=Geometry::zeroTol)
    return (A.NormV!=NormV) ? 0 : 1;

  return 0;
}

bool
Plane::operator!=(const Plane& A) const
  /*!
    Determine if two plane are not equal (within tolerance).
    \param A :: Plane to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}

int
Plane::isEqual(const Plane& A) const
  /*!
    Determine if two plane are equal (within tolerance).
    Note : planes can have opposite signed Normals and D
    \param A :: Plane to compare
    \retval 1 :: Normals equal    
    \retval 0 :: not equal
    \retval -1 :: Normals opposite
  */
{
  if (&A==this) return 1;


  if (fabs(A.Dist-Dist)<=Geometry::zeroTol)
    {
      if (A.NormV==NormV) return 1;
      if (fabs(A.Dist)<=Geometry::zeroTol &&
	  A.NormV== -NormV)
	return -1;
      return 0;
    }
  // Normal opposite  
  if (fabs(A.Dist+Dist)<=Geometry::zeroTol)
    return (A.NormV != -NormV) ? 0 : -1;

  return 0;
}

int
Plane::setSurface(const std::string& Pstr)
  /*! 
     processes a standard MCNPX plane string:
     There are three types : 
     - (A) px Distance
     - (B) p A B C D (equation Ax+By+Cz=D)
     - (C) p Vec3D Vec3D Vec3D
     \param Pstr :: String to make into a plane of type p{xyz} or p 
     \return 0 on success, -ve of failure
  */
{
  // Two types of plane string p[x-z]  and p
  std::string Line=Pstr;
  std::string item;
  
  if (!StrFunc::section(Line,item) || tolower(item[0])!='p')
    return -1;
  // Only 3 need to be declared
  double surf[9]={0.0,0,0,0,0};
      
  if (item.size()==1)  // PROCESS BASIC PLANE:
    {
      int cnt;
      for(cnt=0;cnt<9 && StrFunc::section(Line,surf[cnt]);cnt++) ;
      if (cnt!=4 && cnt!=9)
	return -3;
      if (cnt==9)          // Vec3D type
        {
	  Geometry::Vec3D A=Geometry::Vec3D(surf[0],surf[1],surf[2]);
	  Geometry::Vec3D B=Geometry::Vec3D(surf[3],surf[4],surf[5]);
	  Geometry::Vec3D C=Geometry::Vec3D(surf[6],surf[7],surf[8]);
	  B-=A;
	  C-=A;
	  NormV = B*C;
	  NormV.makeUnit();
	  Dist=A.dotProd(NormV);
	}
      else        // Norm Equation:
        { 
	  NormV=Geometry::Vec3D(surf[0],surf[1],surf[2]);
	  const double ll=NormV.makeUnit();
	  if (ll<Geometry::zeroTol)   // avoid 
	    return -4;
	  Dist= surf[3]/ll;
	}
    }
  else if (item.size()==2)  //  PROCESS px type PLANE
    {
      const int ptype=static_cast<int>(tolower(item[1])-'x');
      if (ptype<0 || ptype>2)         // Not x,y,z
	return -5;
      surf[ptype]=1.0;
      if (!StrFunc::convert(Line,Dist))
	return -6;                      //Too short or no number
      NormV=Geometry::Vec3D(surf[0],surf[1],surf[2]);
    }
  else
    return -3;       // WRONG NAME

  setBaseEqn();
  return 0;
}

int
Plane::setPlane(const Geometry::Vec3D& P,const Geometry::Vec3D& N) 
  /*!
    Given a point and a normal direction set the plane
    \param P :: Point for plane to pass thought
    \param N :: Normal for the plane
    \retval 0 :: success
  */
{
  NormV=N;
  NormV.makeUnit();
  Dist=P.dotProd(NormV);
  setBaseEqn();
  return 0;
}

int
Plane::setPlane(const Geometry::Vec3D& PA,const Geometry::Vec3D& PB,
		const Geometry::Vec3D& PC) 
  /*!
    Given three points make the plane pass via the points
    \param PA :: Point for plane to pass through
    \param PB :: Point for plane to pass through
    \param PC :: Point for plane to pass through
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("Plane","setPlane(3 Vec3d)");
  const Geometry::Vec3D LA=PC-PA;
  const Geometry::Vec3D LB=PB-PA;
  NormV=LA*LB;
  NormV.makeUnit();
  Dist=PA.dotProd(NormV);
  setBaseEqn();
  return 0;
}

int
Plane::setPlane(const Geometry::Vec3D& PA,const Geometry::Vec3D& PB,
		const Geometry::Vec3D& PC,const Geometry::Vec3D& N) 
  /*!
    Given three points make the plane pass via the points
    \param PA :: Point for plane to pass through
    \param PB :: Point for plane to pass through
    \param PC :: Point for plane to pass through
    \param N :: Approximate normal direction 
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("Plane","setPlane(4 Vec3d)");
  const Geometry::Vec3D LA=PC-PA;
  const Geometry::Vec3D LB=PB-PA;
  NormV=LA*LB;
  const double DP=NormV.dotProd(N);
  if (DP<-Geometry::zeroTol)
    NormV*=1.0;
  NormV.makeUnit();
  Dist=PA.dotProd(NormV);
  setBaseEqn();
  return 0;
}

int
Plane::setPlane(const Plane& A)
  /*!
    Set a plane based on another plane
    \param A :: Plane to copy
    \retval 0 :: success
  */
{
  if (this!=&A)
    {
      NormV=A.NormV;
      Dist=A.Dist;
      setBaseEqn();
    }
  return 0;
}

int
Plane::setPlane(const Geometry::Vec3D& N,const double D) 
  /*!
    Given a point and a normal direction set the plane
    \param N :: Normal for the plane
    \param D :: Distance from origin
    \retval 0 :: success
  */
{
  NormV=N;
  NormV.makeUnit();
  Dist=D;
  setBaseEqn();
  return 0;
}

void
Plane::setNormal(const Vec3D& Pt)
  /*!
    Set the normal to Pt
    \param Pt :: Normal vector
   */
{
  if (Pt.abs()>Geometry::zeroTol)
    {
      NormV=Pt;
      NormV.makeUnit();
      setBaseEqn();
    }
  return;
}

void
Plane::setDistance(const double D)
  /*!
    Set the distance in the plane
    \param D :: Distance to set
   */
{
  Dist=D;
  setBaseEqn();
  return;
}

void
Plane::mirrorPt(Geometry::Vec3D& Pt) const
  /*!
    Apply a mirror symmetry to the point (not a reflection)
    \param Pt :: Point to move
    \return Mirror point
  */
{
  Pt+=NormV*(2.0*(Dist-Pt.dotProd(NormV)));
  return;
}

void
Plane::mirrorAxis(Geometry::Vec3D& Axis) const
  /*!
    Apply a mirror symmetry to the axis (not a reflection)
    \param Axis :: Axis to move
    \return Mirror axis
  */
{
  // First find where the origin moves.
  Geometry::Vec3D Origin(0,0,0);
  mirrorPt(Origin);
  // now the Axis point
  mirrorPt(Axis);
  Axis-=Origin;
  return;
}

void
Plane::mirror(const Geometry::Plane& MP) 
  /*!
    Apply a mirror plane to the Plane
    This involves moving the normal
    \param MP :: Mirror point
   */
{
  MP.mirrorAxis(NormV);
  setBaseEqn();
  return;
}


void
Plane::mirrorSelf()
  /*!
    Apply a mirror plane to the Plane
    on itself
    This involves reflecting the normal
    and the distance
   */
{
  NormV*=-1.0;
  Dist*=-1.0;
  setBaseEqn();
  return;
}

void
Plane::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Rotate the plane about the origin by MA 
    \param MA :: direct rotation matrix (3x3)
  */
{
  NormV.rotate(MA);
  NormV.makeUnit();
  Quadratic::rotate(MA);
  return;
}


void
Plane::rotate(const Geometry::Quaternion& QA) 
  /*!
    Rotate the plane about the origin by QA 
    \param QA :: Quaternion
  */
{
  QA.rotate(NormV);
  NormV.makeUnit();
  Quadratic::rotate(QA);
  return;
}

void
Plane::displace(const Geometry::Vec3D& Sp) 
  /*!
    Displace the plane by Point Sp.  
    i.e. r+sp now on the plane 
    \param Sp :: point value of displacement
  */
{
  Dist+=NormV.dotProd(Sp);
  Quadratic::displace(Sp);
  return;
}

double
Plane::distance(const Geometry::Vec3D& A) const
  /*!
    Determine the distance of point A from the plane 
    returns a value relative to the normal
    \param A :: point to get distance from 
    \returns signed distance from point
  */
{
  return A.dotProd(NormV)-Dist;
}

Geometry::Vec3D
Plane::closestPt(const Geometry::Vec3D& A) const
  /*!
    Determine the closet point on the plane from A
    \param A :: point to compare with the plane
    \returns distance tot he point 
  */
{
  return A-NormV*(A.dotProd(NormV)-Dist);
}

double
Plane::dotProd(const Plane& A) const
  /*!
    Calculates the dot product
    \param A :: plane to calculate the normal distance from x
    \returns the Normal.A.Normal dot product
  */
{
  return NormV.dotProd(A.NormV);
}

Geometry::Vec3D
Plane::crossProd(const Plane& A) const
  /*!
    Take the cross produce of the normals
    \param A :: plane to calculate the cross product from 
    \returns the Normal x A.Normal cross product 
  */
{
  return NormV*A.NormV;
}

int
Plane::side(const Geometry::Vec3D& A) const
  /*!
    Calcualates the side that the point is on
    \param A :: test point
    \retval +ve :: on the same side as the normal
    \retval -ve :: the  opposite side 
    \retval 0 :: A is on the plane itself (within tolerence) 
  */
{
  double Dp=NormV.dotProd(A);
  Dp-=Dist;
  if (Geometry::zeroTol<fabs(Dp))
    return (Dp>0) ? 1 : -1;
  return 0;
}

void
Plane::reversePtValid(const int sign,const Geometry::Vec3D& A)
  /*!
    Calcualates the side that the point is on
    and reverses the plane sense if the 
    \param sign :: Sign of the direction to use
    \param A :: test point
  */
{
  ELog::RegMethod RegA("Plane","reversePtValid");
  double Dp=NormV.dotProd(A);
  Dp-=Dist;
  if (Geometry::zeroTol>fabs(Dp)) return;
  if (Dp*sign>0) return;
  NormV*= -1.0;
  Dist*=-1;
  return;
}

int
Plane::onSurface(const Geometry::Vec3D& A) const
  /*! 
     Calcuate the side that the point is on
     and returns success if it is on the surface.
     - Uses zeroTol to determine the closeness
     \param A :: Point to check
     \retval 1 if on the surface 
     \retval 0 if off the surface 
     
  */
{
  return (side(A)!=0) ? 0 : 1;
}

void 
Plane::print() const
  /*!
    Prints out the surface info and
    the Plane info.
  */
{
  Quadratic::print();
  return;
}

int
Plane::reversedPlane() const
  /*!
    Special funtion to determin if a px/py/pz plane
    would be appropiate BUT the plane is negative in normal
    \return 1-3 if negative normal / 0 otherwize
  */
{
  for(int i=0;i<3;i++)
    if (-NormV[i]>(1.0-Geometry::zeroTol))
      return i+1;
  return 0;
}

int
Plane::planeType() const
  /*! 
    Find if the normal vector allows it to be a special
    type of plane (x,y,z direction) 
    (Assumes NormV is a unit vector)
    \retval 1-3 :: on the x,y,z axis
    \retval 0 :: general plane
  */
{
  for(int i=0;i<3;i++)
    if (NormV[i]>(1.0-Geometry::zeroTol))
      return i+1;
  return 0;
}

void 
Plane::setBaseEqn()
  /*!
    Sets the general equation for a plane
  */
{
  BaseEqn[0]=0.0;     // A x^2
  BaseEqn[1]=0.0;     // B y^2
  BaseEqn[2]=0.0;     // C z^2 
  BaseEqn[3]=0.0;     // D xy
  BaseEqn[4]=0.0;     // E xz
  BaseEqn[5]=0.0;     // F yz
  BaseEqn[6]=NormV[0];     // G x
  BaseEqn[7]=NormV[1];     // H y
  BaseEqn[8]=NormV[2];     // J z
  BaseEqn[9]= -Dist;        // K const
  return;
}
  
void 
Plane::writeFLUKA(std::ostream& OX) const
  /*! 
    Object of write is to output a Fluka file [free format]
    \param OX :: Output stream (required for multiple std::endl)  
  */
{
  ELog::RegMethod RegA("Plane","writeFLUKA");
  
  masterWrite& MW=masterWrite::Instance();

  std::ostringstream cx;
  const int ptype=planeType();
  if (!ptype)
    {
      cx<<"PLA s"<<getName()<<" "
	<<MW.Num(NormV)<<" "
	<<MW.Num(NormV*Dist);
    }
  else
    {
      // NormV[] is -1.0 or 1.0
      const double D=NormV[ptype-1]*Dist;
      const std::string PNMX[3]={"YZP","XZP","XYP"};
      cx<<PNMX[ptype-1]<<" s"<<getName()<<" "<<MW.Num(D);
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

  
void 
Plane::writePOVRay(std::ostream& OX) const
  /*! 
    Object of write is to output a POV-Ray file
    \param OX :: Output stream (required for multiple std::endl)  
  */
{
  ELog::RegMethod RegA("Plane","writePOVRay");
  
  masterWrite& MW=masterWrite::Instance();

  OX<<"#declare s"<<getName()
    << " = plane { ";
  const int ptype=planeType();
  if (!ptype)
    {
      OX<<"<"<<MW.NumComma(NormV)<<">,"
	<<MW.Num(Dist)<<"}"<<std::endl;
    }
  else
    {
      // NormV[] is -1.0 or 1.0
      const double D=NormV[ptype-1]*Dist;
      const std::string PNMX[3]={"x","y","z"};
      OX<<"xyz"[ptype-1]<< ", " << MW.Num(D)<<"}"<<std::endl;
    }
  return;
}

void 
Plane::write(std::ostream& OX) const
  /*! 
    Object of write is to output a MCNPX plane info 
    \param OX :: Output stream (required for multiple std::endl)  
  */
{
  masterWrite& MW=masterWrite::Instance();

  std::ostringstream cx;
  Surface::writeHeader(cx);
  const int ptype=planeType();
  if (!ptype)
      cx<<"p "<<MW.Num(NormV)<<" "<<MW.Num(Dist);
  else if(NormV[ptype-1]<0)
    cx<<"p"<<"xyz"[ptype-1]<<" "<<MW.Num(-Dist);
  else 
    cx<<"p"<<"xyz"[ptype-1]<<" "<<MW.Num(Dist);

  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

} // NAMESPACE Geometry
