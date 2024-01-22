/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1ElectronShield.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include "Exception.h"
#include "surfRegister.h"
#include "BaseVisit.h"
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
#include "SurInter.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "Importance.h"
#include "Object.h"

#include "M1ElectronShield.h"

namespace xraySystem
{

M1ElectronShield::M1ElectronShield(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Main","Extra"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1ElectronShield::~M1ElectronShield()
  /*!
    Destructor
   */
{}

void
M1ElectronShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1ElectronShield","populate");

  FixedRotate::populate(Control);

  elecXOut=Control.EvalVar<double>(keyName+"ElecXOut");
  elecLength=Control.EvalVar<double>(keyName+"ElecLength");
  elecFrontLength=Control.EvalVar<double>(keyName+"ElecFrontLength");
  elecHeight=Control.EvalVar<double>(keyName+"ElecHeight");
  elecThick=Control.EvalVar<double>(keyName+"ElecThick");
  elecEdge=Control.EvalVar<double>(keyName+"ElecEdge");
  elecHoleRadius=Control.EvalVar<double>(keyName+"ElecHoleRadius");

  connectLength=Control.EvalVar<double>(keyName+"ConnectLength");
  connectGap=Control.EvalVar<double>(keyName+"ConnectGap");
  connectThick=Control.EvalVar<double>(keyName+"ConnectThick");

  nCut=Control.EvalVar<size_t>(keyName+"NCut");
  cutRadius=Control.EvalVar<double>(keyName+"CutRadius");
  cutGap=Control.EvalVar<double>(keyName+"CutGap");
  
  blockOffset=Control.EvalVar<double>(keyName+"BlockOffset");
  blockWidth=Control.EvalVar<double>(keyName+"BlockWidth");

  plateOffset=Control.EvalVar<double>(keyName+"PlateOffset");
  plateHeight=Control.EvalVar<double>(keyName+"PlateHeight");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  pipeRadius=Control.EvalVar<double>(keyName+"PipeRadius");
  pipeThick=Control.EvalVar<double>(keyName+"PipeThick");
  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  electronMat=ModelSupport::EvalMat<int>(Control,keyName+"ElectronMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}

void
M1ElectronShield::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1ElectronShield","createSurfaces");
  
  // ELECTRON shield
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(elecLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(elecLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+3,Origin+X*(elecXOut-elecThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*elecXOut,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(elecHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(elecHeight/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,X,elecHoleRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin+X*(elecXOut-elecEdge),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(elecHeight/2.0-elecThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(elecHeight/2.0-elecThick),Z);


  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(elecFrontLength+elecLength/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin-Y*(elecFrontLength+elecLength/2.0),
			      X,plateHeight/2.0);
    
  // low heat join
  ModelSupport::buildPlane
    (SMap,buildIndex+105,Origin-Z*(connectGap+connectThick/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+106,Origin-Z*(connectGap-connectThick/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+205,Origin+Z*(connectGap-connectThick/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin+Z*(connectGap+connectThick/2.0),Z);

  const Geometry::Vec3D pOrg=Origin+X*elecXOut;
  // mid inner bend layer
  ModelSupport::buildPlane
    (SMap,buildIndex+314,pOrg+X*connectLength,X);
  // layer to end 
  ModelSupport::buildPlane
    (SMap,buildIndex+303,pOrg+X*blockOffset,X);
  
  // plate
  ModelSupport::buildPlane
    (SMap,buildIndex+403,pOrg+X*plateOffset,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+404,pOrg+X*(plateThick+plateOffset),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+405,pOrg-Z*(plateHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+406,pOrg+Z*(plateHeight/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+415,pOrg-Z*(plateHeight/2.0-plateThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+416,pOrg+Z*(plateHeight/2.0-plateThick),Z);

  Geometry::Vec3D cMid=pOrg-Y*(static_cast<double>(nCut/2)*cutGap);
  int BI(buildIndex+400);
  for(size_t i=0;i<nCut;i++)
    {
      ModelSupport::buildCylinder(SMap,BI+7,cMid,X,cutRadius);
      cMid+=Y*cutGap;
      BI+=10;
    }
  
  ModelSupport::buildPlane
    (SMap,buildIndex+503,pOrg+X*(blockOffset+blockWidth),X);

  // mid way split
  const Geometry::Vec3D pipeOrgA
    (pOrg+X*(blockOffset+blockWidth)-
     Z*((plateHeight/2.0-plateThick+connectGap+connectThick/2.0)/2.0));
  const Geometry::Vec3D pipeOrgB
    (pOrg+X*(blockOffset+blockWidth)+
     Z*((plateHeight/2.0-plateThick+connectGap+connectThick/2.0)/2.0));
  
  ModelSupport::buildCylinder(SMap,buildIndex+1007,pipeOrgA,Y,pipeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1017,pipeOrgA,
			     Y,pipeRadius+pipeThick);
  ModelSupport::buildCylinder(SMap,buildIndex+1107,pipeOrgB,Y,pipeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+1117,pipeOrgB,
			     Y,pipeRadius+pipeThick);
  
  return;
}
  
void
M1ElectronShield::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1ElectronShield","createObjects");

  const HeadRule mirrorHR=getRule("Mirror");
  const HeadRule tubeHR=getRule("TubeRadius");

  HeadRule HR;
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -7");
  makeCell("EPlateHole",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 13 -4 5 -6 (-15:16:3) 7");
  makeCell("EPlate",System,cellIndex++,electronMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 13 -3 15 -16");
  makeCell("EPlate",System,cellIndex++,voidMat,0.0,HR*mirrorHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 4 -314 105 -106");
  makeCell("EHeat",System,cellIndex++,electronMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 314 -403 105 -106");
  makeCell("EHeatVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 4 -314 205 -206");
  makeCell("EHeat",System,cellIndex++,electronMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 314 -403 205 -206");
  makeCell("EHeatVoid",System,cellIndex++,voidMat,0.0,HR);  

  const HeadRule mainCutHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"403 -404");
  HeadRule cutHR;
  int BI(buildIndex+400);
  for(size_t i=0;i<nCut;i++)
    {
      HR=mainCutHR*HeadRule(SMap,BI,-7);
      makeCell("PlateHole",System,cellIndex++,voidMat,0.0,HR);  
      cutHR*=HeadRule(SMap,BI,7);
      BI+=10;
    }
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-17:11) -2 403 -404 405 -406");
  makeCell("Plate",System,cellIndex++,electronMat,0.0,HR*cutHR);  

  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 303 -403 405 -415");
  makeCell("PlateEnd",System,cellIndex++,electronMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 303 -403 416 -406");
  makeCell("PlateEnd",System,cellIndex++,electronMat,0.0,HR);  

  // mid space
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 106 -205 -403 4");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,HR);  
  
  // Block + pipe

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -105 415 303 -503 1017");
  makeCell("BlockBase",System,cellIndex++,electronMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -1007");
  makeCell("Pipe",System,cellIndex++,waterMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -1017 1007");
  makeCell("Pipe",System,cellIndex++,electronMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -105 415 -403 503 1017");
  makeCell("BlockBase",System,cellIndex++,voidMat,0.0,HR);  
  // top
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 206 -416 303 -503 1117");
  makeCell("BlockTop",System,cellIndex++,electronMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -1107");
  makeCell("Pipe",System,cellIndex++,waterMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -1117 1107");
  makeCell("Pipe",System,cellIndex++,electronMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 206 -416 -403 503 1117");
  makeCell("BlockTop",System,cellIndex++,voidMat,0.0,HR);  

  
  // exterior voids:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 4 -303 5 -105");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 4 -303 206 -6");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 5 -6");
  addOuterSurf("Main",HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 403 -404 405 -406 (-17:11)");
  addOuterSurf("Extra",HR);

  return;
}



void
M1ElectronShield::createVoidObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1ElectronShield","createObjects");

  const HeadRule tubeHR=getRule("TubeRadius");
  const HeadRule mirrorHR=getRule("Mirror");
  const HeadRule tubeF=getRule("TubeFront");
  const HeadRule tubeB=getRule("TubeBack");

  const HeadRule ACylHR=getRule("ringACyl");
  const HeadRule AFrontHR=getRule("ringAFront");
  const HeadRule ABackHR=getRule("ringABack");

  const HeadRule BCylHR=getRule("ringBCyl");
  const HeadRule BFrontHR=getRule("ringBFront");
  const HeadRule BBackHR=getRule("ringBBack");

  const HeadRule mainBackHR(SMap,buildIndex,-2);
  const HeadRule mainFrontHR(SMap,buildIndex,1);

  const HeadRule outerBackHR(SMap,buildIndex,-2);
  const HeadRule outerFrontHR(SMap,buildIndex,1);

  const HeadRule midUnitHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"404 405 -406");
  const HeadRule rightUnitHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"303 5 -405");
  const HeadRule leftUnitHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"303 406 -6");
  const HeadRule topUnitHR=HeadRule(SMap,buildIndex,6)*mirrorHR;
  const HeadRule baseUnitHR=HeadRule(SMap,buildIndex,-5)*mirrorHR;


  HeadRule HR;
  // Can we test stuff??
  if (!ACylHR.isEmpty() && !BCylHR.isEmpty())
    {
      HR=mainBackHR*tubeHR*ABackHR;
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*midUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*leftUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*rightUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*topUnitHR);      
      makeCell("RingVoidBase",System,cellIndex++,voidMat,0.0,HR*baseUnitHR);

      HR=ACylHR*ABackHR.complement()*AFrontHR.complement();

      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*midUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*leftUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*rightUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*topUnitHR);
      makeCell("RingVoidBase",System,cellIndex++,voidMat,0.0,HR*baseUnitHR);


      HR=tubeHR*BFrontHR*AFrontHR;

	    
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*midUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*leftUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*rightUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*topUnitHR);
      makeCell("RingVoidBase",System,cellIndex++,voidMat,0.0,HR*baseUnitHR);

      HR=BCylHR*BFrontHR.complement()*BBackHR.complement();
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*midUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*leftUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*rightUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*topUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*baseUnitHR);

      HR=tubeHR*mainFrontHR*BBackHR;
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*midUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*leftUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*rightUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*topUnitHR);
      makeCell("RingVoid",System,cellIndex++,voidMat,0.0,HR*baseUnitHR);
      
    }

  // Special for electron shield:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-403:404:-405:406");
  HR*=mirrorHR*tubeHR*mainFrontHR.complement()*tubeF;
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR);  
  HR=mirrorHR*tubeHR*mainBackHR.complement()*tubeB;
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR);  
  
  return;
}

void
M1ElectronShield::addExternal(Simulation& System)
  /*!
    Inserts the blades into the close crystal voids
    \parma System :: Simulation to use
  */
{
  ELog::RegMethod Rega("M1ElectronShield","addExternal");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 13 -4  5 -15");  
  insertComponent(System,"BaseEndVoid",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 13 -4 -6 16");  
  insertComponent(System,"TopEndVoid",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 13 -4 5 -15");  
  insertComponent(System,"BaseVoid",HR.complement());

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 13 -4 -6 16");  
  insertComponent(System,"TopVoid",HR.complement());
  return;
}
  
void
M1ElectronShield::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1ElectronShield","createLinks");


  // link points are defined in the end of createSurfaces

  return;
}

void
M1ElectronShield::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1ElectronShield","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  createVoidObjects(System);
  addExternal(System);
  
  return;
}

}  // NAMESPACE xraySystem
