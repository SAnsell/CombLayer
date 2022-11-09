/***********************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   skadi/SkadiHut.cxx
 *
 * Copyright (c) 2004-2022 by Tsitohaina Randiamalala & Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "BaseMap.h"
#include "CellMap.h"

#include "SkadiHut.h"

namespace essSystem
{
SkadiHut::SkadiHut(const std::string& Key) :
  attachSystem::FixedRotate(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: keynName
   */
{}

SkadiHut::~SkadiHut()
  /*!
    Destructor
   */
{}

void SkadiHut::populate(const FuncDataBase& Control)
{
  ELog::RegMethod RegA("SkadiHut","populate");

  FixedRotate::populate(Control);

  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");

  const size_t nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  for(size_t i=0;i<nLayer;i++)
    {
      layerOffset LO;
      const std::string LStr=keyName+"L"+std::to_string(i+1);
      LO.front=Control.EvalVar<double>(LStr+"Front");
      LO.leftWall=Control.EvalVar<double>(LStr+"LeftWall");
      LO.rightWall=Control.EvalVar<double>(LStr+"RightWall");
      LO.roof=Control.EvalVar<double>(LStr+"Roof");
      LO.back=Control.EvalVar<double>(LStr+"Back");
      LO.floor=Control.EvalVar<double>(LStr+"Floor");
      LO.mat=ModelSupport::EvalMat<int>(Control,LStr+"Mat");
      layerV.push_back(LO);
    }

  // Variable for the detector pipe
  pipeRadius=Control.EvalVar<double>(keyName+"pipeRadius");
  pipeLength=Control.EvalVar<double>(keyName+"pipeLength");
  pipeL1Thickness=Control.EvalVar<double>(keyName+"pipeL1Thick");
  pipeL2Thickness=Control.EvalVar<double>(keyName+"pipeL2Thick");
  
  pipeL1Mat=ModelSupport::EvalMat<int>(Control,keyName+"pipeL1Mat");
  pipeL2Mat=ModelSupport::EvalMat<int>(Control,keyName+"pipeL2Mat");
  //
  
  return;
}

void SkadiHut::createSurfaces()
  /*!
    Construct all the surfaces
  */
{
  ELog::RegMethod RegA("SkadiHut","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z); 

  int BI(buildIndex+10);
  for(const layerOffset& LO : layerV)
    {
      // L1 WALLS:
      ModelSupport::buildPlane
	(SMap,BI+1,Origin-Y*(LO.front+voidLength/2.0),Y);
      ModelSupport::buildPlane
	(SMap,BI+2,Origin+Y*(LO.back+voidLength/2.0),Y);
      ModelSupport::buildPlane
	(SMap,BI+3,Origin-X*(LO.leftWall+voidWidth/2.0),X);
      ModelSupport::buildPlane
	(SMap,BI+4,Origin+X*(LO.rightWall+voidWidth/2.0),X);
      ModelSupport::buildPlane
	(SMap,BI+5,Origin-Z*(LO.floor+voidDepth),Z);  
      ModelSupport::buildPlane
	(SMap,BI+6,Origin+Z*(LO.roof+voidHeight),Z);
      BI+=10;
    }

  /*
  // Cylinder Pipe
   ModelSupport::buildCylinder(SMap,buildIndex+147,Origin,Y,pipeRadius);
   ModelSupport::buildCylinder(SMap,buildIndex+148,Origin,Y,
			       pipeRadius+pipeL1Thickness);
   ModelSupport::buildCylinder(SMap,buildIndex+143,Origin,Y,
			       pipeRadius+pipeL1Thickness+pipeL2Thickness);
   ModelSupport::buildPlane(SMap,buildIndex+151,Origin+Y*pipeLength,Y);
  */
  return;
}

void SkadiHut::createObjects(Simulation& System)
  /*!
    Construct all the objects
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("SkadiHut","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  int index(0);
  int BI(buildIndex);
  for(const layerOffset& LO : layerV)
    {
      index++;
      const std::string LayerStr="L"+std::to_string(index);
      HR=ModelSupport::getHeadRule
	(SMap,BI,"1 -12 13 -14 15 -16 (2:-3:4:-5:6) ");
      makeCell(LayerStr,System,cellIndex++,LO.mat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,BI,"-1 11 13 -14 15 -16");
      makeCell(LayerStr+"Front",System,cellIndex++,LO.mat,0.0,HR);
      addCell("FrontWall",cellIndex-1);
      BI+=10;
    }

  /*  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -42 41 -51 ");
  makeCell("L1Pipe",SsytemcellIndex++,pipeL1Mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -43 42 -51 ");
  makeCell(,SsytemcellIndex++,pipeL2Mat,0.0,HR);
  setCell("L2Pipe",cellIndex-1);
  */

  // Exclude:
  HR=ModelSupport::getHeadRule(SMap,BI,"1 -2 3 -4 5 -6 ");
  addOuterSurf(HR);      

  return;
}

void
SkadiHut::createLinks()
  /*!
    Construct and name all the links
  */
{
  ELog::RegMethod RegA("SkadiHut","createLinks");

  FixedComp::setNConnect(6*(layerV.size()+1));
  double D[6]=
    {voidLength/2.0,voidLength/2.0,
     voidWidth/2.0,voidWidth/2.0,
     voidDepth,voidHeight};

  FixedComp::setConnect(0,Origin-Y*D[0],-Y);
  FixedComp::setConnect(1,Origin+Y*D[1],Y);
  FixedComp::setConnect(2,Origin-X*D[2],-X);
  FixedComp::setConnect(3,Origin+X*D[3],X);
  FixedComp::setConnect(4,Origin-Y*D[4],-Z);
  FixedComp::setConnect(5,Origin-Y*D[5],Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  FixedComp::nameSideIndex(0,"innerFront");
  FixedComp::nameSideIndex(1,"innerBack");
  int BI(buildIndex+10);
  size_t index=6;
  for(const layerOffset& LO : layerV)
    {
      setConnect(index,Origin-Y*(D[0]+LO.front),-Y);
      setConnect(index+1,Origin+Y*(D[1]+LO.back),Y);
      setConnect(index+2,Origin-X*(D[2]+LO.leftWall),-X);
      setConnect(index+3,Origin+X*(D[3]+LO.rightWall),X);
      setConnect(index+4,Origin-Z*(D[4]+LO.roof),-Z);
      setConnect(index+5,Origin+Z*(D[5]+LO.floor),Z);

      setLinkSurf(index+0,-SMap.realSurf(BI+1));
      setLinkSurf(index+1,SMap.realSurf(BI+2));
      setLinkSurf(index+2,-SMap.realSurf(BI+3));
      setLinkSurf(index+3,SMap.realSurf(BI+4));
      setLinkSurf(index+4,-SMap.realSurf(BI+5));
      setLinkSurf(index+5,SMap.realSurf(BI+6));

      const std::string lStr="Layer"+std::to_string(index/6);
      FixedComp::nameSideIndex(index,lStr+"Front");
      FixedComp::nameSideIndex(index+1,lStr+"Back");

      index+=6;
      BI+=10;
    }

  return;
}

void
SkadiHut::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("SkadiHut","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,voidLength/2.0);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

}//Emd namespace
