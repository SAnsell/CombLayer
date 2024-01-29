/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1Mirror.cxx
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
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Surface.h"
#include "SurInter.h"
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
#include "M1Mirror.h"


namespace xraySystem
{

M1Mirror::M1Mirror(const std::string& Key) :
  attachSystem::FixedRotate(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


M1Mirror::~M1Mirror()
  /*!
    Destructor
   */
{}

void
M1Mirror::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1Mirror","populate");

  FixedRotate::populate(Control);

  theta=Control.EvalVar<double>(keyName+"Theta");  
  phi=Control.EvalVar<double>(keyName+"Phi");
  theta=0.0;
  zAngle=0.0;
  ELog::EM<<"Warnign theta/zangle set to zero" <<ELog::endDiag;
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  slotXStep=Control.EvalVar<double>(keyName+"SlotXStep");
  slotWidth=Control.EvalVar<double>(keyName+"SlotWidth");
  slotDepth=Control.EvalVar<double>(keyName+"SlotDepth");

  pipeXStep=Control.EvalVar<double>(keyName+"PipeXStep");
  pipeYStep=Control.EvalVar<double>(keyName+"PipeYStep");
  pipeZStep=Control.EvalVar<double>(keyName+"PipeZStep");
  pipeSideRadius=Control.EvalVar<double>(keyName+"PipeSideRadius");
  pipeWallThick=Control.EvalVar<double>(keyName+"PipeWallThick");
  pipeBaseLen=Control.EvalVar<double>(keyName+"PipeBaseLen");
  pipeBaseRadius=Control.EvalVar<double>(keyName+"PipeBaseRadius");
  pipeOuterLen=Control.EvalVar<double>(keyName+"PipeOuterLen");
  pipeOuterRadius=Control.EvalVar<double>(keyName+"PipeOuterRadius");

  nWaterChannel=Control.EvalVar<size_t>(keyName+"NWaterChannel");
  waterLength=Control.EvalVar<double>(keyName+"WaterLength");
  waterHeight=Control.EvalVar<double>(keyName+"WaterHeight");
  waterWidth=Control.EvalVar<double>(keyName+"WaterWidth");
  waterDrop=Control.EvalVar<double>(keyName+"WaterDrop");
  waterGap=Control.EvalVar<double>(keyName+"WaterGap");

  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  

  return;
}


void
M1Mirror::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1Mirror","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  makePlane("Divider",SMap,buildIndex+1000,Origin,Y);

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width,X);
  makePlane("RefSurf",SMap,buildIndex+4,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // support cuts:
  Geometry::Vec3D slotOrg=Origin-X*slotXStep;
  ModelSupport::buildPlane(SMap,buildIndex+13,slotOrg-X*(slotWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,slotOrg+X*(slotWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,slotOrg-Z*(height/2.0-slotDepth),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,slotOrg+Z*(height/2.0-slotDepth),Z);


  // pipes:
  const Geometry::Vec3D pBaseA=Origin-X*pipeXStep-Y*(length/2.0-pipeYStep);
  const Geometry::Vec3D pBaseB=Origin-X*pipeXStep+Y*(length/2.0-pipeYStep);

  const Geometry::Vec3D pMidA=Origin-Z*(height/2.0-pipeZStep)
    -Y*(length/2.0-pipeYStep);
  const Geometry::Vec3D pMidB=Origin-Z*(height/2.0-pipeZStep)
    +Y*(length/2.0-pipeYStep);


  ModelSupport::buildCylinder(SMap,buildIndex+107,pBaseA,Z,pipeBaseRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,pBaseB,Z,pipeBaseRadius);
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,pBaseA,Z,pipeBaseRadius+pipeWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,pBaseB,Z,pipeBaseRadius+pipeWallThick);

  ModelSupport::buildCylinder
    (SMap,buildIndex+127,pBaseA,Z,pipeOuterRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,pBaseB,Z,pipeOuterRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+157,pMidA,X,pipeSideRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+257,pMidB,X,pipeSideRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+167,pMidA,X,pipeSideRadius+pipeWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+267,pMidB,X,pipeSideRadius+pipeWallThick);


  // pipe plane separators
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Z*(height/2.0-pipeBaseLen),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+111,Origin-Z*(height/2.0-pipeOuterLen),Z);

  ModelSupport::buildPlane(SMap,buildIndex+154,pBaseA,X);


  // water channels:

  ModelSupport::buildPlane(SMap,buildIndex+1001,
			   Origin-Y*(waterLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1002,
			   Origin+Y*(waterLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1003,
			   Origin-X*(waterHeight+waterDrop),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,
			   Origin-X*waterDrop,X);

  Geometry::Vec3D WMidPt=Origin-Z*
    ((waterWidth+waterGap)*static_cast<double>(nWaterChannel)/2.0);

  int BI(buildIndex+1000);
  for(size_t i=0;i<nWaterChannel;i++)
    {

      ModelSupport::buildPlane(SMap,BI+5,
			       WMidPt-Z*(waterWidth/2.0),Z);
      ModelSupport::buildPlane(SMap,BI+6,
			       WMidPt+Z*(waterWidth/2.0),Z);
      WMidPt+=Z*(waterGap+waterWidth);
      BI+=10;
    }

  // Point for pipework connection
  ELog::EM<<"Point == "<<SMap.realSurf(buildIndex+5)<<ELog::endDiag;
  const Geometry::Vec3D pipePtA=
    SurInter::getLinePoint(pBaseA,-Z,SMap.realSurf(buildIndex+5));
  const Geometry::Vec3D pipePtB=
    SurInter::getLinePoint(pBaseB,-Z,SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(7,pipePtA,-Z);
  FixedComp::setConnect(8,pipePtB,-Z);
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(9,slotOrg-X*(slotWidth/2.0),-X);
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(10,slotOrg+X*(slotWidth/2.0),X);
  FixedComp::setLinkSurf(10,-SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(11,slotOrg-Z*(height/2.0-slotDepth),Z);
  FixedComp::setLinkSurf(11,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(12,slotOrg-Z*(height/2.0-slotDepth),Z);
  FixedComp::setLinkSurf(12,SMap.realSurf(buildIndex+16));

  // key normal orientation
  FixedComp::setConnect(13,pipePtA,Y);
  FixedComp::setConnect(14,pipePtB,Y);
  FixedComp::setLinkSurf(13,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(14,-SMap.realSurf(buildIndex+5));

  
  return;
}

void
M1Mirror::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1Mirror","createObjects");

  HeadRule HR;
  HeadRule waterCutHR;

  int BI(buildIndex+1000);
  if (nWaterChannel)
    {
      waterCutHR=ModelSupport::getHeadRule
	(SMap,buildIndex,"1001 -1002 1003 -1004 1005");
      waterCutHR*=HeadRule(SMap,BI+static_cast<int>(nWaterChannel-1)*10,-6);
      waterCutHR.makeComplement();
    }

  // xstal
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
     "1 -1000  3 -4 5 -6 (-13:14:(-16 15)) (101:117) (154:167) (111:127)");
  makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,HR*waterCutHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
     "1000 -2 3 -4 5 -6 (-13:14:(-16 15)) (101:217) (154:267) (111:227)");
  makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,HR*waterCutHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -14 -15 5");
  makeCell("Slot",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -14 16 -6");
  makeCell("Slot",System,cellIndex++,voidMat,0.0,HR);


  // pipes
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -107");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -117 107 (154:157)");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -157 107");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -167 157 117");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -111 -127 117");
  makeCell("pipe",System,cellIndex++,outerMat,0.0,HR);

  // opposite side
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -207");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -101 -217 207 (154:257)");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -257 207");
  makeCell("pipe",System,cellIndex++,waterMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -154 -267 257 217");
  makeCell("pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -111 -227 217");
  makeCell("pipe",System,cellIndex++,outerMat,0.0,HR);

  // skip first gap
  if (nWaterChannel)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,BI,"1001 -1002 1003 -1004 5M -6M");
      makeCell("SiWater",System,cellIndex++,waterMat,0.0,HR);
      
      for(size_t i=1;i<nWaterChannel;i++)
	{
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,BI,"1001 -1002 1003 -1004 6M -15M");
	  makeCell("SiWaterGap",System,cellIndex++,mirrorMat,0.0,HR);
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,BI,"1001 -1002 1003 -1004 15M -16M");
	  makeCell("SiWater",System,cellIndex++,waterMat,0.0,HR);
	  BI+=10;
	}
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(HR);

  return;
}

void
M1Mirror::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1Mirror","createLinks");

  // link points are defined in the end of createSurfaces

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*width,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(6,Origin-X*width,Y);
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+3));

  // note no surf for base widht:
  FixedComp::setConnect(15,Origin-X*(width/2.0),Y);

  nameSideIndex(2,"outSide");
  nameSideIndex(3,"beamSide");
  nameSideIndex(3,"mirrorSide");
  nameSideIndex(4,"base");
  nameSideIndex(5,"top");
  nameSideIndex(6,"backPlateOrg");
  nameSideIndex(7,"downPipeA");
  nameSideIndex(8,"downPipeB");
  nameSideIndex(9,"slotBase");
  nameSideIndex(10,"slotTop");
  nameSideIndex(11,"slotAMid");
  nameSideIndex(12,"slotBMid");
  nameSideIndex(13,"normPipeA");
  nameSideIndex(14,"normPipeB");
  nameSideIndex(15,"centreAxis");
  
  return;
}

void
M1Mirror::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1Mirror","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
