/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/CollUnit.cxx
 *
 * Copyright (c) 2004-2021 Stuart Ansell
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
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
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
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"

#include "CollUnit.h"

namespace xraySystem
{

CollUnit::CollUnit(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


CollUnit::~CollUnit()
  /*!
    Destructor
  */
{}

void
CollUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CollUnit","populate");

  FixedRotate::populate(Control);

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeWall=Control.EvalVar<double>(keyName+"TubeWall");
  tubeYStep=Control.EvalVar<double>(keyName+"TubeYStep");
  tubeTopGap=Control.EvalVar<double>(keyName+"TubeTopGap");
  tubeMainGap=Control.EvalVar<double>(keyName+"TubeMainGap");
  tubeTopLength=Control.EvalVar<double>(keyName+"TubeTopLength");
  tubeDownLength=Control.EvalVar<double>(keyName+"TubeDownLength");


  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");
  plateWidth=Control.EvalVar<double>(keyName+"PlateWidth");


  nHoles=Control.EvalVar<size_t>(keyName+"NHoles");
  holeGap=Control.EvalVar<double>(keyName+"HoleGap");
  holeRadius=Control.EvalVar<double>(keyName+"HoleRadius");
  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}

void
CollUnit::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CollUnit","createSurfaces");

  if (!isActive("Flange"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+100,Origin+Z*(3*plateLength),Z);
      ExternalCut::setCutSurf("Flange",-SMap.realSurf(buildIndex+100));
    }
  // main plate
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(plateThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(plateThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(plateWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(plateWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+5,Origin-Z*(plateLength/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+6,Origin+Z*(plateLength/2.0),Z);

  // Holes
  int BI(buildIndex+100);
  Geometry::Vec3D HCent(Origin-Z*(holeGap*static_cast<double>((nHoles-1)/2)));
  for(size_t i=0;i<nHoles;i++)
    {
      ModelSupport::buildCylinder(SMap,BI+7,HCent,Y,holeRadius);
      HCent+=Z*holeGap;
      BI+=10;
    }

  // TUBING:

  // ordered list of the points in the tube
  // -note the top tubes are make a random point above the layer
  // also the step is 45 deg to the out step so x and z distance same
  const Geometry::Vec3D TCent(Origin+Y*tubeYStep);
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   TCent+Y*(tubeRadius+2.0*tubeWall),Y);
  
  const double xzStep(tubeMainGap-tubeTopGap);  
  const std::vector<Geometry::Vec3D> chain
    ({
      TCent-X*tubeTopGap+Z*(tubeTopLength+plateLength),
      TCent-X*tubeTopGap+Z*tubeTopLength,
      TCent-X*tubeMainGap+Z*(tubeTopLength-xzStep),
      TCent-X*tubeMainGap-Z*tubeDownLength,
      TCent-X*tubeTopGap-Z*(tubeDownLength+xzStep),
      TCent+X*tubeTopGap-Z*(tubeDownLength+xzStep),
      TCent+X*tubeMainGap-Z*tubeDownLength,
      TCent+X*tubeMainGap+Z*(tubeTopLength-xzStep),
      TCent+X*tubeTopGap+Z*tubeTopLength,
      TCent+X*tubeTopGap+Z*(tubeTopLength+plateLength)
    });

  ModelSupport::buildCylinder(SMap,buildIndex+2007,chain[0],Z,tubeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+2008,chain[0],Z,
			      tubeRadius+tubeWall);
  BI=buildIndex+2010;
  for(size_t i=1;i<chain.size()-1;i++)
    {
      const Geometry::Vec3D aAxis=(chain[i]-chain[i-1]).unit();
      const Geometry::Vec3D bAxis=(chain[i+1]-chain[i]).unit();
      // divide centre axis
      const Geometry::Vec3D dAxis=(bAxis+aAxis)/2.0;

      ModelSupport::buildPlane(SMap,BI+1,chain[i],dAxis);
      if (i+1!=chain.size())
	{
	  ModelSupport::buildCylinder(SMap,BI+7,chain[i],bAxis,tubeRadius);
	  ModelSupport::buildCylinder
	    (SMap,BI+8,chain[i],bAxis,tubeRadius+tubeWall);
	}
      BI+=10;
    }    
  return;
}

void
CollUnit::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CollUnit","createObjects");

  const HeadRule flangeHR=getRule("Flange");
  HeadRule HR;
  HeadRule HRHole;
  
  // linear pneumatics feedthrough
  int BI(buildIndex+100);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  for(size_t i=0;i<nHoles;i++)
    {
      std::string hName("Hole"+std::to_string(i));
      HRHole=ModelSupport::getHeadRule(SMap,buildIndex,BI,"1 -2 -7M");
      makeCell(hName,System,cellIndex++,voidMat,0.0,HRHole);
      HRHole=ModelSupport::getHeadRule(SMap,BI,"7");
      HR*=HRHole;
      BI+=10;
    }
  makeCell("Plate",System,cellIndex++,plateMat,0.0,HR);
  // void above
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 6");  
  makeCell("PlateVoid",System,cellIndex++,plateMat,0.0,HR*flangeHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2011 -2007");
  makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR*flangeHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2011 -2008 2007");
  makeCell("supportPipe",System,cellIndex++,pipeMat,0.0,HR*flangeHR);

  std::vector<HeadRule> pipes;
  BI=buildIndex+2010;
  for(size_t i=0;i<7;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11 -7");
      makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11 7 -8");
      makeCell("supplyPipe",System,cellIndex++,pipeMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11 -8");
      pipes.push_back(HR.complement());
      BI+=10;
    }

  HR=ModelSupport::getHeadRule(SMap,BI,"1 -7");
  makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR*flangeHR);
  HR=ModelSupport::getHeadRule(SMap,BI,"1 -8 7");
  makeCell("supportPipe",System,cellIndex++,pipeMat,0.0,HR*flangeHR);


  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2 -1002 3 -4 5 -6");

  // maybe need to divide this:
  for(const HeadRule& pHR : pipes)
    HR*=pHR;
  makeCell("tubeVoid",System,cellIndex++,voidMat,0.0,HR);



  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -1002 3 -4 5");  
  addOuterSurf(HR*flangeHR);
  ELog::EM<<"F == "<<flangeHR<<ELog::endDiag;
  
  return;
}


void
CollUnit::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("CollUnit","createLinks");

  return;
}


void
CollUnit::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CollUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
