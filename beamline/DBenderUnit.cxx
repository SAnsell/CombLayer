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

DBenderUnit::DBenderUnit(const DBenderUnit& A) : 
  ShapeUnit(A),
  RCentA(A.RCentA),RCentB(A.RCentB),RAxisA(A.RAxisA),
  RAxisB(A.RAxisB),RPlaneFrontA(A.RPlaneFrontA),
  RPlaneFrontB(A.RPlaneFrontB),RPlaneBackA(A.RPlaneBackA),
  RPlaneBackB(A.RPlaneBackB),endPtA(A.endPtA),
  endPtB(A.endPtB),RadiusA(A.RadiusA),RadiusB(A.RadiusB),
  aHeight(A.aHeight),bHeight(A.bHeight),aWidth(A.aWidth),
  bWidth(A.bWidth),Length(A.Length),rotAng(A.rotAng),
  sndAng(A.sndAng),AXVec(A.AXVec),AYVec(A.AYVec),
  AZVec(A.AZVec),BXVec(A.BXVec),BYVec(A.BYVec),BZVec(A.BZVec)
  /*!
    Copy constructor
    \param A :: DBenderUnit to copy
  */
{}

DBenderUnit&
DBenderUnit::operator=(const DBenderUnit& A)
  /*!
    Assignment operator
    \param A :: DBenderUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ShapeUnit::operator=(A);
      RCentA=A.RCentA;
      RCentB=A.RCentB;
      RAxisA=A.RAxisA;
      RAxisB=A.RAxisB;
      RPlaneFrontA=A.RPlaneFrontA;
      RPlaneFrontB=A.RPlaneFrontB;
      RPlaneBackA=A.RPlaneBackA;
      RPlaneBackB=A.RPlaneBackB;
      endPtA=A.endPtA;
      endPtB=A.endPtB;
      RadiusA=A.RadiusA;
      RadiusB=A.RadiusB;
      aHeight=A.aHeight;
      bHeight=A.bHeight;
      aWidth=A.aWidth;
      bWidth=A.bWidth;
      Length=A.Length;
      rotAng=A.rotAng;
      sndAng=A.sndAng;
      AXVec=A.AXVec;
      AYVec=A.AYVec;
      AZVec=A.AZVec;
      BXVec=A.BXVec;
      BYVec=A.BYVec;
      BZVec=A.BZVec;
    }
  return *this;
}

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
     \param RA :: Primary radius
     \param RB :: Secondary raidus
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

  // Now calculate RAxis:

  RAxisA=ZAxis;
  RAxisB=ZAxis;
  const Geometry::Quaternion Qa=
    Geometry::Quaternion::calcQRotDeg(rotAng,YAxis);
  Qa.rotate(RAxisA);

  const Geometry::Quaternion Qb=
    Geometry::Quaternion::calcQRotDeg(sndAng,YAxis);
  Qb.rotate(RAxisB);

  RPlaneFrontA=YAxis*RAxisA;
  RPlaneFrontB=YAxis*RAxisB;

  RCentA=begPt+RPlaneFrontA*RadiusA;
  RCentB=begPt+RPlaneFrontB*RadiusB;

  // calc angle and rotation:
  const double thetaA = asin(Length/RadiusA);
  const double thetaB = asin(Length/RadiusB);
  endPt+=YAxis*Length+RPlaneFrontA*(Length*tan(thetaA))+
    RPlaneFrontB*(Length*tan(thetaB));
  endPtA=begPt+YAxis*Length+RPlaneFrontA*(Length*tan(thetaA));
  endPtB=begPt+YAxis*Length+ RPlaneFrontB*(Length*tan(thetaB));

  // Cacluate the new direction [outgoing]
  const Geometry::Quaternion QaxisA=
    Geometry::Quaternion::calcQRotDeg(thetaA,RAxisA);

  const Geometry::Quaternion QaxisB=
    Geometry::Quaternion::calcQRotDeg(thetaB,RAxisB);
  RPlaneBackA=RPlaneFrontA;
  RPlaneBackB=RPlaneFrontB;
    
  QaxisA.rotate(BYVec);  
  QaxisB.rotate(BYVec);

  QaxisA.rotate(RPlaneBackA);  
  QaxisB.rotate(RPlaneBackB);
  
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
  
  const Geometry::Vec3D nEndPt=endPtA+RPlaneBackA*(pSign*DW);
  const Geometry::Vec3D midPt=(nEndPt+begPt)/2.0;
  const Geometry::Vec3D LDir=((nEndPt-begPt)*RAxisA).unit();
    
  const Geometry::Vec3D AMid=(nEndPt-begPt)/2.0;
  std::pair<std::complex<double>,
	    std::complex<double> > OutValues;
  const size_t NAns=solveQuadratic(1.0,2.0*AMid.dotProd(LDir),
				   AMid.dotProd(AMid)-RadiusA*RadiusA,
				   OutValues);
  
  if (NAns) 
    {
      if (fabs(OutValues.first.imag())<Geometry::zeroTol &&
          OutValues.first.real()>0.0)
        return midPt+LDir*OutValues.first.real();
      
      if (fabs(OutValues.second.imag())<Geometry::zeroTol &&
          OutValues.second.real()>0.0)
        return midPt+LDir*OutValues.second.real();
    }

  ELog::EM<<"Failed to find quadratic solution for bender"<<ELog::endErr;
  return RCentA;
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

  const double DW=(bHeight-aHeight)/2.0;
  const double pSign=(plusSide) ? -1.0 : 1.0;
  
  const Geometry::Vec3D nEndPt=endPtB+RPlaneBackB*(pSign*DW);
  const Geometry::Vec3D midPt=(nEndPt+begPt)/2.0;
  const Geometry::Vec3D LDir=((nEndPt-begPt)*RAxisB).unit();
    
  const Geometry::Vec3D AMid=(nEndPt-begPt)/2.0;
  std::pair<std::complex<double>,
	    std::complex<double> > OutValues;
  const size_t NAns=solveQuadratic(1.0,2.0*AMid.dotProd(LDir),
				   AMid.dotProd(AMid)-RadiusB*RadiusB,
				   OutValues);
  
  if (NAns) 
    {
      if (fabs(OutValues.first.imag())<Geometry::zeroTol &&
          OutValues.first.real()>0.0)
        return midPt+LDir*OutValues.first.real();
      
      if (fabs(OutValues.second.imag())<Geometry::zeroTol &&
          OutValues.second.real()>0.0)
        return midPt+LDir*OutValues.second.real();
    }

  ELog::EM<<"Failed to find quadratic solution for bender"<<ELog::endErr;
  return RCentB;
}

void
DBenderUnit::createSurfaces(ModelSupport::surfRegister& SMap,
			  const std::vector<double>& Thick)
  /*!
    Build the surfaces for the track
    \param SMap :: SMap to use
    \param Thick :: Thickness for each layer
   */
{
  ELog::RegMethod RegA("DBenderUnit","createSurfaces");

  // plus/minus points
  const Geometry::Vec3D PWCentre= calcWidthCent(1);
  const Geometry::Vec3D MWCentre= calcWidthCent(0);

  const Geometry::Vec3D PHCentre= calcHeightCent(1);
  const Geometry::Vec3D MHCentre= calcHeightCent(0);

  // Make divider plane width required
  const double maxWidth=Thick.back()+(aWidth+bWidth);
  ModelSupport::buildPlane(SMap,shapeIndex+1001,
			   begPt+RPlaneFrontA*maxWidth,
			   endPt+RPlaneFrontA*maxWidth,
			   endPt+RPlaneFrontA*maxWidth+RAxisA,
			   -RPlaneFrontA);
  // Make divider plane heightx required
  const double maxHeight=Thick.back()+(aHeight+bHeight);
  ModelSupport::buildPlane(SMap,shapeIndex+1002,
			   begPt+RPlaneFrontB*maxHeight,
			   endPt+RPlaneFrontB*maxHeight,
			   endPt+RPlaneFrontB*maxHeight+RAxisB,
			   -RPlaneFrontB);

  // Widths
  for(size_t j=0;j<Thick.size();j++)
    {
      const int SN(shapeIndex+static_cast<int>(j)*layerSep);

      ModelSupport::buildCylinder(SMap,SN+5,MHCentre,
				  RAxisB,RadiusB-(Thick[j]+aHeight/2.0));
      ModelSupport::buildCylinder(SMap,SN+6,PHCentre,
				  RAxisB,RadiusB+(Thick[j]+aHeight/2.0));

      ModelSupport::buildCylinder(SMap,SN+7,MWCentre,
				  RAxisA,RadiusB-(Thick[j]+aWidth/2.0));
      ModelSupport::buildCylinder(SMap,SN+8,PWCentre,
				  RAxisA,RadiusB+(Thick[j]+aWidth/2.0));
    }

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
    (SMap,shapeIndex+SN,shapeIndex," 1001M 1002M 5 -6 7 -8 ");
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
