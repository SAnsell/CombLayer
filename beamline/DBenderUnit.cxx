/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/DBenderUnit.cxx 
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "polySupport.h"
#include "surfRegister.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "GuideUnit.h"

#include "DBenderUnit.h"

namespace beamlineSystem
{

DBenderUnit::DBenderUnit(const std::string& key)  :
  GuideUnit(key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: Key Name
  */
{}

DBenderUnit::DBenderUnit(const DBenderUnit& A) : 
  GuideUnit(A),
  RCentA(A.RCentA),RCentB(A.RCentB),RAxisA(A.RAxisA),
  RAxisB(A.RAxisB),RPlaneFrontA(A.RPlaneFrontA),
  RPlaneFrontB(A.RPlaneFrontB),RPlaneBackA(A.RPlaneBackA),
  RPlaneBackB(A.RPlaneBackB),endPtA(A.endPtA),endPtB(A.endPtB),
  RadiusA(A.RadiusA),RadiusB(A.RadiusB),aHeight(A.aHeight),
  bHeight(A.bHeight),aWidth(A.aWidth),bWidth(A.bWidth),
  Length(A.Length),rotAng(A.rotAng),sndAng(A.sndAng),
  AXVec(A.AXVec),AYVec(A.AYVec),AZVec(A.AZVec),
  BXVec(A.BXVec),BYVec(A.BYVec),BZVec(A.BZVec)
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
      GuideUnit::operator=(A);
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
      if (std::abs<double>(OutValues.first.imag())<Geometry::zeroTol &&
          OutValues.first.real()>0.0)
        return midPt+LDir*OutValues.first.real();
      
      if (std::abs<double>(OutValues.second.imag())<Geometry::zeroTol &&
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
DBenderUnit::createSurfaces()
  /*!
    Build the surfaces for the track
   */
{
  ELog::RegMethod RegA("DBenderUnit","createSurfaces");

  // plus/minus points
  const Geometry::Vec3D PWCentre= calcWidthCent(1);
  const Geometry::Vec3D MWCentre= calcWidthCent(0);

  const Geometry::Vec3D PHCentre= calcHeightCent(1);
  const Geometry::Vec3D MHCentre= calcHeightCent(0);

  // Make divider plane width required
  const double maxWidth=layerThick.back()+(aWidth+bWidth);
  ModelSupport::buildPlane(SMap,buildIndex+1001,
			   begPt+RPlaneFrontA*maxWidth,
			   endPt+RPlaneFrontA*maxWidth,
			   endPt+RPlaneFrontA*maxWidth+RAxisA,
			   -RPlaneFrontA);
  // Make divider plane heightx required
  const double maxHeight=layerThick.back()+(aHeight+bHeight);
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   begPt+RPlaneFrontB*maxHeight,
			   endPt+RPlaneFrontB*maxHeight,
			   endPt+RPlaneFrontB*maxHeight+RAxisB,
			   -RPlaneFrontB);

  // Widths
  int SN(buildIndex);
  for(size_t j=0;j<layerThick.size();j++)
    {
      ModelSupport::buildCylinder(SMap,SN+5,MHCentre,
				  RAxisB,RadiusB-(layerThick[j]+aHeight/2.0));
      ModelSupport::buildCylinder(SMap,SN+6,PHCentre,
				  RAxisB,RadiusB+(layerThick[j]+aHeight/2.0));

      ModelSupport::buildCylinder(SMap,SN+7,MWCentre,
				  RAxisA,RadiusB-(layerThick[j]+aWidth/2.0));
      ModelSupport::buildCylinder(SMap,SN+8,PWCentre,
				  RAxisA,RadiusB+(layerThick[j]+aWidth/2.0));
      SN+=20;
    }

  return;
}

void
DBenderUnit::createObjects(Simulation& System)
  /*!
    Build all the objects
    \param System :: Simulation
  */
{

  HeadRule HR;

  HeadRule innerHR;
  const HeadRule divHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"1001 1002")*
    getFrontRule()*getBackRule();

  int SN(buildIndex);
  for(size_t i=0;i<layerThick.size();i++)
    {
      HR=ModelSupport::getHeadRule(SMap,SN,"5 -6 7 -8");
      makeCell("Layer"+std::to_string(i),System,cellIndex++,layerMat[i],0.0,
	       HR*innerHR*divHR);
      SN+=20;
      innerHR=HR.complement();
    }
  addOuterSurf(HR*divHR);
  return ;
}
    
void
DBenderUnit::createLinks()
  /*!
    Add link points to the guide unit
  */
{
  ELog::RegMethod RegA("BenderUnit","addSideLinks");

  setLinkSurf(2,SMap.realSurf(buildIndex+5));
  setLinkSurf(3,SMap.realSurf(buildIndex+6));
  setLinkSurf(4,SMap.realSurf(buildIndex+7));
  setLinkSurf(5,SMap.realSurf(buildIndex+8));

  const Geometry::Vec3D MCentre= calcWidthCent(0);
  setConnect(2,MCentre+RCentA*RadiusA+RAxisA*((aWidth+bWidth)/4.0),-RAxisA);
  setConnect(3,MCentre+RCentA*RadiusA+RAxisA*((aWidth+bWidth)/4.0),RAxisA);
  setConnect(4,MCentre+RCentB*RadiusB+RAxisB*((aHeight+bHeight)/4.0),-RAxisB);
  setConnect(5,MCentre+RCentB*RadiusB+RAxisB*((aHeight+bHeight)/4.0),RAxisB);

  return;
}

void
DBenderUnit::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Construct a DBender unit
    \param System :: Simulation to use
    \param FC :: FixedComp to use for basis set
    \param sideIndex :: side link point
   */
{
  ELog::RegMethod RegA("DBenderUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE beamlineSystem
