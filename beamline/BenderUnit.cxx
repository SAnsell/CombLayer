/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/BenderUnit.cxx 
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "polySupport.h"
#include "surfRegister.h"
#include "Surface.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "ShapeUnit.h"
#include "BenderUnit.h"

namespace beamlineSystem
{


BenderUnit::BenderUnit(const int ON,const int LS)  :
  ShapeUnit(ON,LS)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}

BenderUnit::BenderUnit(const BenderUnit& A) : 
  ShapeUnit(A),
  RCent(A.RCent),RAxis(A.RAxis),RPlane(A.RPlane),Radius(A.Radius),
  aHeight(A.aHeight),bHeight(A.bHeight),aWidth(A.aWidth),
  bWidth(A.bWidth),Length(A.Length),rotAng(A.rotAng),
  AXVec(A.AXVec),AYVec(A.AYVec),AZVec(A.AZVec),BXVec(A.BXVec),
  BYVec(A.BYVec),BZVec(A.BZVec)
  /*!
    Copy constructor
    \param A :: BenderUnit to copy
  */
{}

BenderUnit&
BenderUnit::operator=(const BenderUnit& A)
  /*!
    Assignment operator
    \param A :: BenderUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ShapeUnit::operator=(A);
      RCent=A.RCent;
      RAxis=A.RAxis;
      RPlane=A.RPlane;
      Radius=A.Radius;
      aHeight=A.aHeight;
      bHeight=A.bHeight;
      aWidth=A.aWidth;
      bWidth=A.bWidth;
      Length=A.Length;
      rotAng=A.rotAng;
      AXVec=A.AXVec;
      AYVec=A.AYVec;
      AZVec=A.AZVec;
      BXVec=A.BXVec;
      BYVec=A.BYVec;
      BZVec=A.BZVec;
    }
  return *this;
}

BenderUnit::~BenderUnit() 
  /*!
    Destructor
   */
{}

BenderUnit*
BenderUnit::clone() const 
  /*!
    Clone funciton
    \return *this
   */
{
  return new BenderUnit(*this);
}

void
BenderUnit::setValues(const double H,const double W,
		      const double L,const double Rad,
		      const double BA)
  /*!
    Quick setting of values
    \param H :: Height
    \param W :: Width
    \param L :: Length
    \param Rad :: Rad
    \param BA :: Bend angle relative to Z axis [deg]
   */
{
  ELog::RegMethod RegA("BenderUnit","setValues");
  
  Radius=Rad;
  aHeight=H;
  bHeight=H;
  aWidth=W;
  bWidth=W;
  Length=L;
  rotAng=BA;
  return;
}

void
BenderUnit::setValues(const double HA,const double HB,
		      const double WA,const double WB,
		      const double L,const double Rad,
		      const double BA)
  /*!
    Quick setting of values
    \param HA :: Height [start]
    \param HB :: Height [End]
    \param WA :: Width [start]
    \param WB :: Width [end]
    \param L :: Length
    \param Rad :: Rad
    \param BA :: Bend angle relative to Z axis [deg]
   */
{
  ELog::RegMethod RegA("BenderUnit","setValues");
  
  Radius=Rad;
  aHeight=HA;
  bHeight=HB;
  aWidth=WA;
  bWidth=WB;
  Length=L;
  rotAng=BA;
  return;
}

void
BenderUnit::setOriginAxis(const Geometry::Vec3D& O,
			  const Geometry::Vec3D& XAxis,
			  const Geometry::Vec3D& YAxis,
			  const Geometry::Vec3D& ZAxis)
   /*!
     Set axis and endpoints using a rotation 
     \param O :: Origin
     \param XAxis :: Input X Axis
     \param YAxis :: Input Y Axis
     \param ZAxis :: Input Z Axis
    */
{
  ELog::RegMethod RegA("BenderUnit","setOriginAxis");

  begPt=O;
  endPt=O;
  AXVec=XAxis;
  AYVec=YAxis;
  AZVec=ZAxis;
  // Now calc. exit point

  BXVec=XAxis;
  BYVec=YAxis;
  BZVec=ZAxis;

  // Now calculate RAxis:
  RAxis=ZAxis;
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(rotAng,YAxis);
  Qz.rotate(RAxis);

  
  RPlane=YAxis*RAxis;
  RCent=begPt+RPlane*Radius;
  
  // calc angle and rotation:
  const double theta = Length/Radius;
  endPt+=RPlane*(2*Radius*pow(sin(theta/2.0),2.0))+AYVec*(Radius*sin(theta));
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRot(-theta,RAxis);

  Qxy.rotate(BXVec);
  Qxy.rotate(BYVec);
  Qxy.rotate(BZVec);
  return;
}

Geometry::Vec3D
BenderUnit::calcWidthCent(const bool plusSide) const
  /*!
    Calculate the shifted centre based on the difference in
    widths. Keeps the original width point correct (symmetric round
    origin point) -- then track the exit track + / - round the centre line
    bend.
    \param plusSide :: to use the positive / negative side
    \return new centre
  */
{
  ELog::RegMethod RegA("BenderUnit","calcWidthCent");

  const double DW=(bWidth-aWidth)/2.0;
  const double pSign=(plusSide) ? -1.0 : 1.0;

  const Geometry::Vec3D nEndPt=endPt+BXVec*(pSign*DW);
  const Geometry::Vec3D midPt=(nEndPt+begPt)/2.0;
  const Geometry::Vec3D LDir=((nEndPt-begPt)*RAxis).unit();
    
  const Geometry::Vec3D AMid=(nEndPt-begPt)/2.0;
  std::pair<std::complex<double>,
	    std::complex<double> > OutValues;
  const size_t NAns=solveQuadratic(1.0,2.0*AMid.dotProd(LDir),
				   AMid.dotProd(AMid)-Radius*Radius,
				   OutValues);
  if (!NAns) 
    {
      ELog::EM<<"Failed to find quadratic solution for bender"<<ELog::endErr;
      return RCent;
    }

  if (fabs(OutValues.first.imag())<Geometry::zeroTol &&
      OutValues.first.real()>0.0)
    return midPt+LDir*OutValues.first.real();
  if (fabs(OutValues.second.imag())<Geometry::zeroTol &&
      OutValues.second.real()>0.0)
    return midPt+LDir*OutValues.second.real();
  
  return RCent;
}


void
BenderUnit::createSurfaces(ModelSupport::surfRegister& SMap,
			  const std::vector<double>& Thick)
  /*!
    Build the surfaces for the track
    \param SMap :: SMap to use
    \param indexOffset :: Index offset
    \param Thick :: Thickness for each layer
   */
{
  ELog::RegMethod RegA("BenderUnit","createSurfaces");
  Geometry::Vec3D PCentre= calcWidthCent(1);
  Geometry::Vec3D MCentre= calcWidthCent(0);
  // Make divider plane +ve required
  const double maxThick=Thick.back()+(aWidth+bWidth);


  ModelSupport::buildPlane(SMap,shapeIndex+1,
			   begPt+RPlane*maxThick,
			   endPt+RPlane*maxThick,
			   endPt+RPlane*maxThick+RAxis,
			   -RPlane);
  for(size_t j=0;j<Thick.size();j++)
    {
      const int SN(shapeIndex+static_cast<int>(j)*layerSep);
      ModelSupport::buildPlane(SMap,SN+5,
			       begPt-RAxis*(aHeight/2.0+Thick[j]),
			       begPt-RAxis*(aHeight/2.0+Thick[j])+RPlane,
			       endPt-RAxis*(bHeight/2.0+Thick[j]),
			       RAxis);
      ModelSupport::buildPlane(SMap,SN+6,
			       begPt+RAxis*(aHeight/2.0+Thick[j]),
			       begPt+RAxis*(aHeight/2.0+Thick[j])+RPlane,
			       endPt+RAxis*(bHeight/2.0+Thick[j]),
			       RAxis);

      ModelSupport::buildCylinder(SMap,SN+7,MCentre,
				  RAxis,Radius-(Thick[j]+aWidth/2.0));
      ModelSupport::buildCylinder(SMap,SN+8,PCentre,
				  RAxis,Radius+(Thick[j]+aWidth/2.0));
    }
  return;
}

std::string
BenderUnit::getString(const ModelSupport::surfRegister& SMap,
		      const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("BenderUnit","getString");

  const int SN(static_cast<int>(layerN)*layerSep);
  return ModelSupport::getComposite
    (SMap,shapeIndex+SN,shapeIndex," 1M 5 -6 7 -8 ");
}

std::string
BenderUnit::getExclude(const ModelSupport::surfRegister& SMap,
		       const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("BenderUnit","getExclude");
  
  std::ostringstream cx;

  const int SN(static_cast<int>(layerN)*layerSep);
  return ModelSupport::getComposite(SMap,shapeIndex+SN," (-5:6:-7:8) ");
}

  
}  // NAMESPACE beamlineSystem
