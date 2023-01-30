/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/BenderUnit.cxx 
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

#include "BenderUnit.h"

namespace beamlineSystem
{

BenderUnit::BenderUnit(const std::string& key)  :
  GuideUnit(key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: keyName
  */
{
  resetYRotation=1;
}

BenderUnit::BenderUnit(const BenderUnit& A) : 
  GuideUnit(A),
  aHeight(A.aHeight),bHeight(A.bHeight),aWidth(A.aWidth),
  bWidth(A.bWidth),radius(A.radius),rotAng(A.rotAng),
  rCent(A.rCent),bX(A.bX),bY(A.bY)
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
      GuideUnit::operator=(A);
      aHeight=A.aHeight;
      bHeight=A.bHeight;
      aWidth=A.aWidth;
      bWidth=A.bWidth;
      radius=A.radius;
      rotAng=A.rotAng;
      rCent=A.rCent;
      bX=A.bX;
      bY=A.bY;
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
BenderUnit::calcConstValues()
  /*!
    Basic value constant values
   */
{
  ELog::RegMethod RegA("BenderUnit","calcConstValues");
  
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRot(rotAng,-Z);  

  rCent=Origin+X*radius;
  endPt=rCent-X*(radius*cos(rotAng))+Y*(radius*sin(rotAng));
  bY=Qxy.makeRotate(Y);
  bX=Qxy.makeRotate(X);
  return;
}
  
Geometry::Vec3D
BenderUnit::calcCentre(const double aLen,const double bLen) const
  /*!
    Calculate the shifted centre based on the difference in
    widths. Keeps the original width point correct (symmetric round
    origin point) -- then track the exit track + / - round the centre line
    bend.
    The method constructs the mid point of the line A-B
    and then determine the distance to the centre. It also
    constructs the normal to the line 
    \return new centre
  */
{
  ELog::RegMethod RegA("BenderUnit","calcMidTrack");

  return calcMidTrack(aLen,bLen).first;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
BenderUnit::calcMidTrack(const double aLen,const double bLen) const
  /*!
    Calculate the shifted centre based on the difference in
    widths. Keeps the original width point correct (symmetric round
    origin point) -- then track the exit track + / - round the centre line
    bend.
    The method constructs the mid point of the line A-B
    and then determine the distance to the centre. It also
    constructs the normal to the line 
    \return new centre / normal pointing from centre
  */
{
  ELog::RegMethod RegA("BenderUnit","calcMidTrack");

  const Geometry::Vec3D APt=Origin+X*aLen;
  const Geometry::Vec3D BPt=endPt+bX*bLen;
  const Geometry::Vec3D midPt((APt+BPt)/2.0);
  const double L=APt.Distance(BPt)/2.0;  // lenght of half line
  const double b=sqrt(radius*radius-L*L);
  Geometry::Vec3D norm=Z*(BPt-APt).unit();

  if (X.dotProd(norm)<0.0)
    norm*=-1.0;
  return std::pair<Geometry::Vec3D,Geometry::Vec3D>
    (midPt+norm*b,-norm);
}

void
BenderUnit::populate(const FuncDataBase& Control)
  /*!
    Sets the appropiate APts/BPtrs based on the type of
    guide needed
    \param Control :: DataBase of varaibels
   */
{
  ELog::RegMethod RegA("PlateUnit","populate");

  GuideUnit::populate(Control);

  aHeight=Control.EvalHead<double>(keyName,"AHeight","Height");
  aWidth=Control.EvalHead<double>(keyName,"AWidth","Width");
  bHeight=Control.EvalHead<double>(keyName,"BHeight","Height");
  bWidth=Control.EvalHead<double>(keyName,"BWidth","Width");

  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  rotAng=Control.EvalDefVar<double>(keyName+"RotAngle",-1.0);

  if (radius>Geometry::zeroTol && rotAng>Geometry::zeroTol)
    length=rotAng*radius;
  else if (radius>Geometry::zeroTol)
    rotAng=length/radius;
  else if (rotAng>Geometry::zeroTol)
    radius=length/rotAng;

  return;
}

void
BenderUnit::createSurfaces()
  /*!
    Build the surfaces for the track
   */
{
  ELog::RegMethod RegA("BenderUnit","createSurfaces");

  calcConstValues();

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,endPt,bY);
      setBack(-SMap.realSurf(buildIndex+2));
    }
  

  const double maxThick=layerThick.back()+(aWidth+bWidth);

  Geometry::Vec3D C,norm;
  std::tie(C,norm)=calcMidTrack(maxThick,maxThick);
      
  ModelSupport::buildPlane(SMap,buildIndex+100,C,norm);
  
  int SN(buildIndex);

  double T(0.0);
  for(size_t i=0;i<layerThick.size();i++)
    {
      ModelSupport::buildPlane(SMap,SN+5,Origin-Z*(aHeight/2.0+T),Z);
      ModelSupport::buildPlane(SMap,SN+6,Origin+Z*(aHeight/2.0+T),Z);

      C=calcCentre(T+aWidth/2.0,T+bWidth/2.0);
      ModelSupport::buildCylinder(SMap,SN+7,C,Z,radius);
      C=calcCentre(-T-aWidth/2.0,-T-bWidth/2.0);
      ModelSupport::buildCylinder(SMap,SN+8,C,Z,radius);

      T+=layerThick[i];
      SN+=20;
    }
  return;
}

void
BenderUnit::createObjects(Simulation& System)
  /*
    Write string for layer number
    \param System :: Simulation
    \return inward string
  */
{
  ELog::RegMethod RegA("BenderUnit","ceateObjects");
  
  const HeadRule fbHR=HeadRule(SMap,buildIndex,100)*
    getFrontRule()*getBackRule();

  HeadRule HR;

  HeadRule innerHR;
  int SN(buildIndex);
  for(size_t i=0;i<layerThick.size();i++)
    {
      HR=ModelSupport::getHeadRule(SMap,SN,"5 -6 7 -8");
      makeCell("Layer"+std::to_string(i),System,cellIndex++,layerMat[i],0.0,
	       HR*innerHR*fbHR);
      innerHR=HR.complement();
      SN+=20;
    }

  addOuterSurf(HR*fbHR);
  return ;
}

void
BenderUnit::createLinks()
  /*!
    Add link points to the guide unit
    \param SMap :: Surface Map 
    \param FC :: FixedComp to use
   */
{
  ELog::RegMethod RegA("BenderUnit","createLinks");

  if (resetYRotation)
    applyAngleRotate(0,-yAngle,0);

  ExternalCut::createLink("front",*this,0,Origin,-Y);
  ExternalCut::createLink("back",*this,1,endPt,bY);

  setLinkSurf(2,SMap.realSurf(buildIndex+5));
  setLinkSurf(3,SMap.realSurf(buildIndex+6));
  setLinkSurf(4,SMap.realSurf(buildIndex+7));
  setLinkSurf(5,SMap.realSurf(buildIndex+8));

  return;
}

void
BenderUnit::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
/*!
    Construct a Bender unit
    \param System :: Simulation to use
    \param FC :: FixedComp to use for basis set
    \param sideIndex :: side link point
   */
{
  ELog::RegMethod RegA("BenderUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE beamlineSystem
