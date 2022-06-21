/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/StriplineBPM.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "StriplineBPM.h"

namespace tdcSystem
{

StriplineBPM::StriplineBPM(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


StriplineBPM::~StriplineBPM()
  /*!
    Destructor
  */
{}

void
StriplineBPM::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("StriplineBPM","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");

  innerAngle=Control.EvalVar<double>(keyName+"InnerAngle");
  innerAngleOffset=Control.EvalVar<double>(keyName+"InnerAngleOffset");

  flangeARadius=Control.EvalHead<double>
    (keyName,"FlangeARadius","FlangeRadius");
  flangeALength=Control.EvalHead<double>
    (keyName,"FlangeALength","FlangeLength");

  flangeBRadius=Control.EvalHead<double>
    (keyName,"FlangeBRadius","FlangeRadius");
  flangeBLength=Control.EvalHead<double>
    (keyName,"FlangeBLength","FlangeLength");

  striplineRadius=Control.EvalVar<double>(keyName+"StriplineRadius");
  striplineThick=Control.EvalVar<double>(keyName+"StriplineThick");
  striplineEnd=Control.EvalVar<double>(keyName+"StriplineEnd");
  striplineYStep=Control.EvalVar<double>(keyName+"StriplineYStep");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  striplineMat=ModelSupport::EvalMat<int>(Control,keyName+"StriplineMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}


void
StriplineBPM::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("StriplineBPM","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // main pipe and thicness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+outerThick);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(length/2.0-flangeALength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(length/2.0-flangeBLength),Y);

  // Field shaping [300]
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,innerRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,Y,innerRadius+innerThick);

  const double ang(M_PI*innerAngle/(2.0*180.0));
  double midAngle(innerAngleOffset*M_PI/180.0);
  int BI(buildIndex+350);
  for(size_t i=0;i<4;i++)
    {
      const Geometry::Vec3D lowAxis(X*cos(midAngle-ang)+Z*sin(midAngle-ang));
      const Geometry::Vec3D highAxis(X*cos(midAngle+ang)+Z*sin(midAngle+ang));
      ModelSupport::buildPlane(SMap,BI+1,Origin,lowAxis);
      ModelSupport::buildPlane(SMap,BI+2,Origin,highAxis);
      midAngle+=M_PI/2.0;
      BI+=2;
    }

  // end point on electrons (solid)
  ModelSupport::buildPlane(SMap,buildIndex+302,
			   Origin+Y*(length/2.0-striplineEnd),Y);


  // Stripline holder
  const double angleStep(M_PI/4.0);

  BI=buildIndex+411;
  for(size_t i=0;i<8;i++)
    {
      const double angle(angleStep*static_cast<double>(i));
      const Geometry::Vec3D axis(X*cos(angle)+Z*sin(angle));
      ModelSupport::buildPlane(SMap,BI,Origin+axis*striplineRadius,axis);
      BI++;
    }

  const Geometry::Vec3D eCent(Origin+Y*(striplineYStep-length/2.0));
  ModelSupport::buildPlane
    (SMap,buildIndex+401,eCent-Y*(striplineThick/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+402,eCent+Y*(striplineThick/2.0),Y);

  return;
}

void
StriplineBPM::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("StriplineBPM","createObjects");

  HeadRule HR;
  
  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  // inner void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-307");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  // front void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 307 -401");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

  // main walll
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -202 7 -17");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,HR);

  // front flange
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 7 -107");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,HR*frontHR);

  // back flange
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"202 7 -207");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"17 401 -402 -411 -412 -413 -414 -415 -416 -417 -418");
  makeCell("ElectronPlate",System,cellIndex++,striplineMat,0.0,HR);


  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 17 -401 -411 -412 -413 -414 -415 -416 -417 -418");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-202 17 402 -411 -412 -413 -414 -415 -416 -417 -418");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);

  if (striplineRadius>flangeARadius)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-101 107 -411 -412 -413 -414 -415 -416 -417 -418");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
    }
  else
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"101 -202 -107 (411:412:413:414:415:416:417:418)");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
    }

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"202 207 -411 -412 -413 -414 -415 -416 -417 -418");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  int BI(0);
  const HeadRule IHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"401 -302  307 -317");
  for(size_t i=0;i<4;i++)
    {

      HR=ModelSupport::getHeadRule(SMap,buildIndex+BI,"351 -352");
      ELog::EM<<"OuterX == "<<cellIndex
	      <<" "<<HR<<ELog::endDiag;

      makeCell("Stripline",System,cellIndex++,striplineMat,0.0,IHR*HR);
      HR=ModelSupport::getRangeHeadRule
	(SMap,351,358,BI,buildIndex,"352R -353R");
      makeCell("StriplineGap",System,cellIndex++,voidMat,0.0,IHR*HR);
      BI+=2;
    }
  // edge electrod void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"401 -302 317 -7");
  makeCell("EdgeVoid",System,cellIndex++,voidMat,0.0,HR);

  // edge electrod void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"302  307 -7");
  makeCell("StriplineEnd",System,cellIndex++,striplineMat,0.0,HR*backHR);

  if (striplineRadius>flangeARadius)
    HR=ModelSupport::getHeadRule
      (SMap,buildIndex,"-411 -412 -413 -414 -415 -416 -417 -418");
  else
    HR=ModelSupport::getHeadRule
      (SMap,buildIndex,"-107");
  addOuterSurf(HR*frontHR*backHR);

  return;
}

void
StriplineBPM::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("StriplineBPM","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}

void
StriplineBPM::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("StriplineBPM","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
