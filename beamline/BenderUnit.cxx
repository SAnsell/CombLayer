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
{}
  
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
BenderUnit::calcAxis()
   /*!
     Set axis and endpoints using a rotation 
    */
{
  ELog::RegMethod RegA("BenderUnit","calcAxis");


  const double rotAng=length/radius;
    
  BXVec=X;
  BYVec=Y;
  BZVec=Z;
  
  const double RSign(rotSide ? 1.0 : -1.0);
  RCent=Origin+X*(RSign*radius);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRot(-,Z);
  

  // calc angle and rotation:
  const double theta = Length/Radius;
  endPt+=RPlane*(2.0*Radius*pow(sin(theta/2.0),2.0))+AYVec*(Radius*sin(theta));

  Qxy.rotate(BXVec);
  Qxy.rotate(BYVec);
  Qxy.rotate(BZVec);

  return;
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
  
  aHeight=Control.EvalTail<double>(keyName,"AHeight","Height");
  aWidth=Control.EvalTail<double>(keyName,"AWidth","Width");
  bHeight=Control.EvalTail<double>(keyName,"BHeight","Height");
  bWidth=Control.EvalTail<double>(keyName,"BWidth","Width");

  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  rotAng=Control.EvalDefVar<double>(keyName+"RotAngle",-1.0);

  if (radius>Geometry::zeroTol && rotAng>Geometry::zeroTol)
    length=rotAng*radius;
  else if (radius>Geometry::zeroTol)
    rotAng=length/radius
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

  const double y=radius*sin(rotAng);
  const double x=rotSide*radius*(1-cos(rotAng));

  const Geometry::Vec3D endPt(X*x+Y*y);
  const Geometry::Vec3D exitAxis=
    X*rotSide*sin(rotAng)+Y*cos(rotAng);

  
  ModelSupport::buildPlane(SMap,buildIndex+2,endPt,exitAxis);
			   

  const double maxThick=layerThick.back()+(aWidth+bWidth);
  
  ModelSupport::buildPlane(SMap,buildIndex+100,
			   Origin+X*(rotSide*maxThick),
			   Origin+X*(rotSide*maxThick)+Z,
			   exitPt+X*(rotSide*maxThick),
			   -X*rotside);
  
  int SN(buildIndex);
  for(size_t i=0;i<layerThick.size();i++)
    {
      ModelSupport::buildPlane(SMap,SN+5,
			       Origin-Z*(aHeight/2.0+layerThick[i]),Z);
      ModelSupport::buildPlane(SMap,SN+6,
			       Origin+Z*(aHeight/2.0+layerThick[i]),Z);

      ModelSupport::buildCylinder(SMap,SN+7,
				  RCent+X*(rotSide*(layerThick[i]+aWidth/2.0)),
				  Z,radius);
      ModelSupport::buildCylinder(SMap,SN+8,
				  RCent-X*(rotSide*(layerThick[i]+aWidth/2.0)),
				  Z,radius);
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
      SN+=20;
      innerHR=HR.complement();
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
  ELog::RegMethod RegA("BenderUnit","addSideLinks");

  setLinkSurf(2,SMap.realSurf(buildIndex+5));
  setLinkSurf(3,SMap.realSurf(buildIndex+6));
  setLinkSurf(4,SMap.realSurf(buildIndex+7));
  setLinkSurf(5,SMap.realSurf(buildIndex+8));


  setConnect(2,Origin-aHeight,-Z);
  setConnect(3,RCent*Radius+RAxis*((aHeight+bHeight)/4.0),RAxis);
  setConnect(4,RCent*(Radius-aWidth/2.0),-RPlane);
  setConnect(5,RCent*(Radius+aWidth/2.0),RPlane);

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
