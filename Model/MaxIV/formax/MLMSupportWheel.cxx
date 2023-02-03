/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/MLMSupportWheel.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "surfRegister.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "MLMSupportWheel.h"


namespace xraySystem
{

MLMSupportWheel::MLMSupportWheel(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}



MLMSupportWheel::~MLMSupportWheel()
  /*!
    Destructor
   */
{}

void
MLMSupportWheel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMSupportWheel","populate");

  FixedRotate::populate(Control);

  wheelRadius=Control.EvalVar<double>(keyName+"WheelRadius");
  wheelOuterRadius=Control.EvalVar<double>(keyName+"WheelOuterRadius");
  wheelHubRadius=Control.EvalVar<double>(keyName+"WheelHubRadius");
  wheelHeight=Control.EvalVar<double>(keyName+"WheelHeight");

  nSpokes=Control.EvalVar<size_t>(keyName+"NSpokes");
  
  spokeThick=Control.EvalVar<double>(keyName+"SpokeThick");
  spokeCornerRadius=Control.EvalVar<double>(keyName+"SpokeCornerRadius");
  spokeCornerGap=Control.EvalVar<double>(keyName+"SpokeCornerGap");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat","Void");

  return;
}


void
MLMSupportWheel::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("MLMSupportWheel","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Y*(height/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Y*(height/2.0),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,wheelHubRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,wheelRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,wheelOuterRadius);

  const double angleStep=(2.0*M_PI)/static_cast<double>(nSpokes);
  double angle(0.0);
  int BI(buildIndex+100);
  for(size_t i=0;i<nSpokes;i++)
    {
      const Geometry::Vec3D PX=X*std::cos(angle)+Z*std::sin(angle);
      ModelSupport::buildPlane(SMap,BI+1,
			       Origin-PX*(spokeThick/2.0),PX);
      ModelSupport::buildPlane(SMap,BI+2,
			       Origin-PX*(spokeThick/2.0),PX);
      angle+=angleStep;
      BI+=10;
    }

  return; 
}

void
MLMSupportWheel::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMSupportWheel","createObjects");

  HeadRule HR;

  // slot:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(11:-17) (-12:-18) 13 -14 15 -6");  
  makeCell("TopSlot",System,cellIndex++,0,0.0,HR);

  // xstal
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6 ((-11 17):(12 18):-13:14:-15)");  
  makeCell("Xstal",System,cellIndex++,mirrorMat,0.0,HR);

  // base plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 -5 105");  
  makeCell("BaseFlat",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 154 -104 5 -126 (-201:202)");  
  makeCell("BaseBack",System,cellIndex++,baseMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"154 -104 5 -126 201 -202");  
  makeCell("BaseBackGap",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -153 103 5 -116");  
  makeCell("BaseFront",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -151 -154 153 5 -126");  
  makeCell("BaseBeam",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"152 -102 -154 153 5 -126 (-203:204)");  
  makeCell("BaseOut",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2 -102 153 5 -126 203 -204");  
  makeCell("BaseOutGap",System,cellIndex++,voidMat,0.0,HR);  
  
  // base voids:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"151 -2 -154 153 5 -126 (-1:-3:4)");  
  makeCell("CrysVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 -153 103 116 -126");  
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 103 -104 126 -6 (-1:2:-3:4)");  
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,HR);  

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 105 -6");
  addOuterSurf(HR);  
  return; 
}

void
MLMSupportWheel::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMSupportWheel","createLinks");
  
  return;
}

void
MLMSupportWheel::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMSupportWheel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
