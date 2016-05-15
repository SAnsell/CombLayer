/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/DBenderUnit.cxx 
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "DBenderUnit.h"

namespace beamlineSystem
{

DBenderUnit::DBenderUnit(const int ON,const int LS)  :
  ShapeUnit(ON,LS)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}


DBenderUnit::~DBenderUnit() 
  /*!
    Destructor
   */
{}

DBenderUnit*
DBenderUnit::clone() const 
  /*!
    Clone funciton
    \return *this
   */
{
  return new DBenderUnit(*this);
}

void
DBenderUnit::setApperture(const double VA,const double VB,
			  const double HA,const double HB)
   /*!
     Set axis and endpoints using a rotation 
     \param VA :: vertial first apperature
     \param VB :: vertial second apperature
     \param HA :: horrizontal first apperature
     \param HB :: horrizontal second apperature
   */
{
  aHeight=VA;
  bHeight=VB;
  aWidth=HA;
  bWidth=HB;
  return;
}

void
DBenderUnit::setRadii(const double RA,const double RB)
   /*!
     Set radii
     \param RA :: Primary 
     \param RB :: 
   */
{
  RadiusA=RA;
  RadiusB=RB;
  return;
}
  
void
DBenderUnit::setLength(const double L)
  /*!
    Set the length 
    \param L :: length 
  */
{
  Length=L;
  return;
}

  
void
DBenderUnit::setOriginAxis(const Geometry::Vec3D& O,
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
  ELog::RegMethod RegA("DBenderUnit","setOriginAxis");

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

  Geometry::Vec3D RAxisA=ZAxis;
  Geometry::Vec3D RAxisB=ZAxis;
  const Geometry::Quaternion Qa=
    Geometry::Quaternion::calcQRotDeg(rotAng,YAxis);
  Qa.rotate(RAxisA);


  const Geometry::Quaternion Qb=
    Geometry::Quaternion::calcQRotDeg(sndAng,YAxis);
  Qb.rotate(RAxisB);


  Geometry::Vec3D RPlaneA=YAxis*RAxisA;
  Geometry::Vec3D RPlaneB=YAxis*RAxisB;

  Geometry::Vec3D RCentA=begPt+RPlaneA*RadiusA;
  Geometry::Vec3D RCentB=begPt+RPlaneA*RadiusB;  

  
  // calc angle and rotation:
  const double thetaA = asin(Length/RadiusA);
  const double thetaB = asin(Length/RadiusB);
  endPt+=YAxis*Length+RPlaneA*Length/tan(thetaA)+
    RPlaneB*Length/tan(thetaB);
  
  Qxy.rotate(BXVec);
  Qxy.rotate(BYVec);
  Qxy.rotate(BZVec);

  return;
}

Geometry::Vec3D
DBenderUnit::calcWidthCent(const bool plusSide) const
  /*!
    Calculate the shifted centre based on the difference in
    widths. Keeps the original width point correct (symmetric round
    origin point) -- then track the exit track + / - round the centre line
    bend.
    \param plusSide :: to use the positive / negative side
    \return new centre
  */
{
  ELog::RegMethod RegA("DBenderUnit","calcWidthCent");

  
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
  */  
  return Geometry::Vec3D(0,0,0);
    //    returnRCent;
}

Geometry::Vec3D
DBenderUnit::calcHeightCent(const bool plusSide) const
  /*!
    Calculate the shifted centre based on the difference in
    widths. Keeps the original width point correct (symmetric round
    origin point) -- then track the exit track + / - round the centre line
    bend.
    \param plusSide :: to use the positive / negative side
    \return new centre
  */
{
  ELog::RegMethod RegA("DBenderUnit","calcHeightCent");

  const double DH=(plusSide) ? -(bHeight-aHeight)/2.0 :
    (bHeight-aHeight)/2.0;

  begPt+=(RadiusA+DH)*
  
  
  const double DH=(bHeight-aHeight)/2.0;
  const double pSign=(plusSide) ? -1.0 : 1.0;
  
  const Geometry::Vec3D nEndPt=endPt+BZVec*(pSign*DW);
  const Geometry::Vec3D midPt=(nEndPt+begPt)/2.0;
  const Geometry::Vec3D LDir=((nEndPt-begPt)*RAxis).unit();
    
  const Geometry::Vec3D AMid=(nEndPt-begPt)/2.0;
  std::pair<std::complex<double>,
	    std::complex<double> > OutValues;
  const size_t NAns=solveQuadratic(1.0,2.0*AMid.dotProd(LDir),
				   AMid.dotProd(AMid)-RadiusB*RadiusB,
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
  */  
  return Geometry::Vec3D(0,0,0);
    //    returnRCent;
}


void
DBenderUnit::createSurfaces(ModelSupport::surfRegister& SMap,
			  const std::vector<double>& Thick)
  /*!
    Build the surfaces for the track
    \param SMap :: SMap to use
    \param indexOffset :: Index offset
    \param Thick :: Thickness for each layer
   */
{
  ELog::RegMethod RegA("DBenderUnit","createSurfaces");
  /*
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
  */
  return;
}

std::string
DBenderUnit::getString(const ModelSupport::surfRegister& SMap,
		      const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("DBenderUnit","getString");

  const int SN(static_cast<int>(layerN)*layerSep);
  return ModelSupport::getComposite
    (SMap,shapeIndex+SN,shapeIndex," 1M 5 -6 7 -8 ");
}

std::string
DBenderUnit::getExclude(const ModelSupport::surfRegister& SMap,
		       const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("DBenderUnit","getExclude");
  
  std::ostringstream cx;

  const int SN(static_cast<int>(layerN)*layerSep);
  return ModelSupport::getComposite(SMap,shapeIndex+SN," (-5:6:-7:8) ");
}

  
}  // NAMESPACE beamlineSystem
