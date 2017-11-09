/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Cone.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Line.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cone.h"

namespace Geometry
{

Cone::Cone() : Quadratic(),
  Centre(), Normal(1,0,0), alpha(0.0), cangle(1.0),
    cutFlag(0)	       
  /*!
    Constructor with centre line along X axis 
    and centre on origin
  */
{
  setBaseEqn();
}

Cone::Cone(const int N,const int T) : Quadratic(N,T),
  Centre(),Normal(1,0,0),alpha(0.0),cangle(1.0),cutFlag(0)
  /*!
    Constructor with centre line along X axis 
    and centre on origin
    \param N :: name
    \param T :: Transform number 
  */
{
  setBaseEqn();
}

Cone::Cone(const Cone& A) :Quadratic(A),
  Centre(A.Centre), Normal(A.Normal), 
  alpha(A.alpha), cangle(A.cangle),
  cutFlag(A.cutFlag)
  /*!
    Standard Copy Constructor
    \param A :: Cone to copy
  */
{}

Cone*
Cone::clone() const
  /*! 
    Makes a clone (implicit virtual copy constructor) 
    \return new(*this)
  */
{
  return new Cone(*this);
}

Cone&
Cone::operator=(const Cone& A)
  /*!
    Assignment operator
    \param A :: Cone to copy
    \return *this
  */
{
  if(this!=&A)
    {
      Quadratic::operator=(A);
      Centre=A.Centre;
      Normal=A.Normal;
      alpha=A.alpha;
      cangle=A.cangle;
      cutFlag=A.cutFlag;
    }
  return *this;
}

Cone::~Cone()
  /*!
    Destructor
  */
{} 

bool
Cone::operator==(const Cone& A) const
  /*!
    Equality operator. Checks angle,centre and 
    normal separately
    \param A :: Cone to compare
    \return A==this within CTolerance
  */
{
  if(this==&A)
    return 1;
  if (fabs(cangle-A.cangle)>Geometry::zeroTol)
    return 0;
  if (Centre.Distance(A.Centre)>Geometry::zeroTol)
    return 0;
  if (Normal.Distance(A.Normal)>Geometry::zeroTol)
    return 0;
  if (A.cutFlag!=cutFlag)
    return 0;

  return 1;
}

bool
Cone::operator!=(const Cone& A) const
  /*!
    Determine if two Cones are not equal (within tolerance).
    \param A :: Cone to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}


int
Cone::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX cone string    
    Recall that cones can only be specified on an axis
     Valid input is: 
     - k/x cen_y cen_z tan(angle)
     - kx tan(angle)
    \return : 0 on success, neg of failure 
  */
{
  ELog::RegMethod RegA("Cone","setSurface");

  std::string Line=this->stripID(Pstr);

  std::string item;
  if (!StrFunc::section(Line,item) || 
      tolower(item[0])!='k' || item.length()<2 || 
      item.length()>3)
    return -1;

  // Cones on X/Y/Z axis
  const size_t itemPt((item[1]=='/' && item.length()==3) ? 2 : 1);
  const int ptype=static_cast<int>(tolower(item[itemPt])-'x');
  if (ptype<0 || ptype>=3)
    return -2;
  double norm[3]={0.0,0.0,0.0};
  double cent[3]={0.0,0.0,0.0};
  norm[ptype]=1.0;

  if (itemPt==1)        // kx type cone
    {
      if (!StrFunc::section(Line,cent[ptype]))
	return -3;
    }
  else
    {
      int index;
      for(index=0;index<3 && StrFunc::section(Line,cent[index]);index++) ;
      if (index!=3)
	return -4;
    }
  // The user must enter t^2 which is tan^2(angle) for MCNPX
  double tanAng;
  if (!StrFunc::section(Line,tanAng))
    return -5;


  cutFlag=0;
  StrFunc::section(Line,cutFlag);      // doesn't set on failure    
  if (cutFlag)
    ELog::EM<<"WARNING == CUTFLAG DEPRECIATED"<<ELog::endWarn;
  
  Centre=Geometry::Vec3D(cent);
  Normal=Geometry::Vec3D(norm);
  setTanAngle(sqrt(tanAng));
  setBaseEqn();
  return 0;
} 

void
Cone::setBaseEqn()
  /*!
    Sets an equation of type 
    \f[ Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0 \f]
  */
{
  const double c2(cangle*cangle);
  const double CdotN(Centre.dotProd(Normal));
  BaseEqn[0]=c2-Normal[0]*Normal[0];     // A x^2
  BaseEqn[1]=c2-Normal[1]*Normal[1];     // B y^2
  BaseEqn[2]=c2-Normal[2]*Normal[2];     // C z^2 
  BaseEqn[3]= -2*Normal[0]*Normal[1];     // D xy
  BaseEqn[4]= -2*Normal[0]*Normal[2];     // E xz
  BaseEqn[5]= -2*Normal[1]*Normal[2];     // F yz
  BaseEqn[6]= 2.0*(Normal[0]*CdotN-Centre[0]*c2) ;     // G x
  BaseEqn[7]= 2.0*(Normal[1]*CdotN-Centre[1]*c2) ;     // H y
  BaseEqn[8]= 2.0*(Normal[2]*CdotN-Centre[2]*c2) ;     // J z
  BaseEqn[9]= c2*Centre.dotProd(Centre)-CdotN*CdotN;   // K const
  return;
}

void
Cone::rotate(const Geometry::Matrix<double>& R)
  /*!
    Rotate both the centre and the normal direction 
    \param R :: Matrix for rotation. 
  */
{
  Centre.rotate(R);
  Normal.rotate(R);
  setBaseEqn();
  return;
}

void
Cone::rotate(const Geometry::Quaternion& QA) 
  /*!
    Rotate both the centre and the normal direction 
    \param QA :: rotate usig Quaterion
  */
{
  QA.rotate(Centre);
  QA.rotate(Normal);
  Normal.makeUnit();
  setBaseEqn();
  return;
}

void 
Cone::displace(const Geometry::Vec3D& A)
  /*!
    Displace the centre
    Only need to update the centre position 
    \param A :: Geometry::Vec3D to add
  */
{
  Centre+=A;
  setBaseEqn();
  return;
}

void
Cone::setCone(const Geometry::Vec3D& C,const Geometry::Vec3D& A,
	      const double angle)
  /*!
    Set the cone
  */
{
  Centre=C;
  Normal=A.unit();
  // Set angle calls: baseEqn
  setAngle(angle);
  return;
}

void 
Cone::setCentre(const Geometry::Vec3D& A)
  /*!
    Sets the central point and the Base Equation
    \param A :: New Centre point
  */
{
  Centre=A;
  setBaseEqn();
  return;
}

void 
Cone::setNormal(const Geometry::Vec3D& A)
  /*!
    Sets the Normal and the Base Equation
    \param A :: New Normal direction
  */
{
  if (A.abs()>Geometry::zeroTol)
    {
      Normal=A;
      Normal.makeUnit();
      setBaseEqn();
    }
  return;
}

void
Cone::setAngle(const double A) 
  /*!
    Set the angle of the cone.
    \param A :: Angle in degrees.
    Resets the base equation
  */
{
  alpha=A;
  cangle=cos(M_PI*alpha/180.0);
  setBaseEqn();
  return;
}

void
Cone::setTanAngle(const double A) 
  /*! 
    Set the cone angle
    Resets the base equation 
    \param A :: Tan of the angle  (for MCNPX)
  */
{
  cangle=1.0/sqrt(A*A+1.0);        // convert tan(theta) to cos(theta)
  alpha=acos(cangle)*180.0/M_PI;
  setBaseEqn();
  return;
}

double
Cone::distance(const Geometry::Vec3D& Pt) const
  /*!
    Calculates the distance from the point to the Cone
    does not calculate the point on the cone that is closest
    \param Pt :: Point to calcuate from

    - normalise to a cone vertex at the origin
    - calculate the angle between the axis and the Point
    - Calculate the distance to P
    \return distance to Pt
  */
{
  const Geometry::Vec3D Px=Pt-Centre;
  // test is the centre to point distance is zero
  if(Px.abs()<Geometry::parallelTol)
    return Px.abs();
  double Pangle=Px.dotProd(Normal)/Px.abs();
  if (Pangle<0.0)
    Pangle=acos(-Pangle);
  else
    Pangle=acos(Pangle);
  
  Pangle-=M_PI*alpha/180.0;
  return Px.abs()*sin(Pangle);
}

Geometry::Vec3D
Cone::surfaceNormal(const Geometry::Vec3D& Pt) const
  /*!
    Get surface normal from point
    \param Pt :: Point 
    \return Normal [pointing outwards]
   */
{
  ELog::RegMethod RegA("Cone","surfaceNormal");

  return Quadratic::surfaceNormal(Pt);
}

int
Cone::side(const Geometry::Vec3D& Pt) const
  /*!
    Calculate if the point R is within the cone 
    \param Pt :: Point to determine if in/out of cone
    \retval -1  :: within the cone
    \retval 0 :: on teh cone
    \retval 1 :: outside the cone
  */
{
  const Geometry::Vec3D cR = (Pt-Centre).unit();
  double rptAngle=Normal.dotProd(cR);
  // exit if on wrong side:
  
  if (cutFlag>0 && rptAngle< -Geometry::zeroTol )
    return 1;
  if (cutFlag<0 && rptAngle>Geometry::zeroTol )
    return 1;
  if (rptAngle<0.0) 
    rptAngle*=-1.0;
  // touch test
  if (fabs(rptAngle-cangle)<Geometry::zeroTol) return 0;
  return (rptAngle>cangle) ? -1 : 1;  
}

int
Cone::onSurface(const Geometry::Vec3D& R) const
  /*! 
     Calculate if the point R is on
     the cone (Note: have to be careful here
     since angle calcuation calcuates an angle.
     We need a distance for tolerance!)
     \param R :: Point to check
     \retval 1 if on the surface 
     \retval 0 if off the surface 
  */
{
  return (side(R)==0) ? 1 : 0;
}


  
void
Cone::write(std::ostream& OX) const
  /*!
    Write out the cone class in an mcnpx
    format.
    \param OX :: Output Stream (required for multiple std::endl)
  */
{
  //               -3 -2 -1 0 1 2 3        
  const char Tailends[]="zyx xyz";
  const int Ndir=Normal.masterDir(Geometry::zeroTol);
  if (Ndir==0)
    {
      Quadratic::write(OX);
      return;
    }
  std::ostringstream cx;
  Quadratic::writeHeader(cx);
  
  const int Cdir=Centre.masterDir(Geometry::zeroTol);
  cx.precision(Geometry::Nprecision);
  // Name and transform 
   
  if (Cdir || Centre.nullVector(Geometry::zeroTol))
    {
      cx<<"k";
      cx<< Tailends[Ndir+3]<<" ";          // set x,y,z based on Ndir
      cx<< ((Cdir>0) ? Centre[Cdir-1] : Centre[-Cdir-1]);
      cx<<" ";
    }
  else
    {
      cx<<" k/";
      cx<< Tailends[Ndir+3]<<" ";          // set x,y,z based on Ndir
      for(int i=0;i<3;i++)
	cx << Centre[i] << " ";
    }
  const double TA=tan((M_PI*alpha)/180.0);    // tan^2(angle)
  cx<<TA*TA;
  if (cutFlag)
    cx<<" "<<((Ndir<0) ? -cutFlag : cutFlag);
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

}  // NAMESPACE Geometry
