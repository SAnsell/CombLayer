/***********************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   trex/TrexHut.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "TrexHut.h"

namespace essSystem
{
TrexHut::TrexHut(const std::string& Key) :
  attachSystem::FixedRotate(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
{}

TrexHut::~TrexHut()
{}

void TrexHut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
   */
{
  ELog::RegMethod RegA("TrexHut","populate");

  FixedRotate::populate(Control);

  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");

  L1Front=Control.EvalVar<double>(keyName+"L1Front");
  L1LeftWall=Control.EvalVar<double>(keyName+"L1LeftWall");
  L1RightWall=Control.EvalVar<double>(keyName+"L1RightWall");
  L1Roof=Control.EvalVar<double>(keyName+"L1Roof");
  L1Floor=Control.EvalVar<double>(keyName+"L1Floor");
  L1Back=Control.EvalVar<double>(keyName+"L1Back");
  
  L2Front=Control.EvalVar<double>(keyName+"L2Front");
  L2LeftWall=Control.EvalVar<double>(keyName+"L2LeftWall");
  L2RightWall=Control.EvalVar<double>(keyName+"L2RightWall");
  L2Roof=Control.EvalVar<double>(keyName+"L2Roof");
  L2Floor=Control.EvalVar<double>(keyName+"L2Floor");
  L2Back=Control.EvalVar<double>(keyName+"L2Back");

  L3Front=Control.EvalVar<double>(keyName+"L3Front");
  L3LeftWall=Control.EvalVar<double>(keyName+"L3LeftWall");
  L3RightWall=Control.EvalVar<double>(keyName+"L3RightWall");
  L3Roof=Control.EvalVar<double>(keyName+"L3Roof");
  L3Floor=Control.EvalVar<double>(keyName+"L3Floor");
  L3Back=Control.EvalVar<double>(keyName+"L3Back");

  L1Mat=ModelSupport::EvalMat<int>(Control,keyName+"L1Mat");
  L2Mat=ModelSupport::EvalMat<int>(Control,keyName+"L2Mat");
  L3Mat=ModelSupport::EvalMat<int>(Control,keyName+"L3Mat");

  return;
}
  
void 
TrexHut::createSurfaces()
  /*!
    construct surfaces 
    */
{
  ELog::RegMethod RegA("TrexHut","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z); 


  // L1 WALLS:
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(L1Front+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(L1Back+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(L1LeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(L1RightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(L1Floor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(L1Roof+voidHeight),Z);

  // L2 WALLS:
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*(L2Front+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(L2Back+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(L2LeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(L2RightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(L2Floor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(L2Roof+voidHeight),Z);

  // L3 WALLS:
   ModelSupport::buildPlane(SMap,buildIndex+31,
			    Origin-Y*(L3Front+voidLength/2.0),Y);
   ModelSupport::buildPlane(SMap,buildIndex+32,
			    Origin+Y*(L3Back+voidLength/2.0),Y);
   ModelSupport::buildPlane(SMap,buildIndex+33,
			    Origin-X*(L3LeftWall+voidWidth/2.0),X);
   ModelSupport::buildPlane(SMap,buildIndex+34,
			    Origin+X*(L3RightWall+voidWidth/2.0),X);
   ModelSupport::buildPlane(SMap,buildIndex+35,
			    Origin-Z*(L3Floor+voidDepth),Z);  
   ModelSupport::buildPlane(SMap,buildIndex+36,
			    Origin+Z*(L3Roof+voidHeight),Z);
}

void TrexHut::createObjects(Simulation& System)
  /*!
    Constructor of object
   */
{
  ELog::RegMethod RegA("TrexHut","createObjects");

  HeadRule HR;
  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "1 -12 13 -14 15 -16 (2:-3:4:-5:6) ");
  makeCell("L1",System,cellIndex++,L1Mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 11 13 -14 15 -16");
  makeCell("L1Front",System,cellIndex++,L1Mat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,
		 "11 -22 23 -24 25 -26 (12:-13:14:-15:16)");
  makeCell("L2",System,cellIndex++,L2Mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 21 23 -24 25 -26");
  makeCell("L2Front",System,cellIndex++,L2Mat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,
		 "21 -32 33 -34 35 -36 (22:-23:24:-25:26)");
  makeCell("L3",System,cellIndex++,L3Mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-21 31 33 -34 35 -36");
  makeCell("L3Front",System,cellIndex++,L3Mat,0.0,HR);

  // Exclude:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 33 -34  35 -36");
  addOuterSurf(HR);      

  return;
}

void TrexHut::createLinks()
{
  ELog::RegMethod RegA("TrexHut","createLinks");

  double D[6]=
    {voidLength/2.0,voidLength/2.0,
     voidWidth/2.0,voidWidth/2.0,
     voidDepth,voidHeight};
  
  const double L1[]=
    { L1Front+L2Front,
      L1Back+L2Back,
      L1LeftWall+L2LeftWall,
      L1RightWall+L2RightWall,
      L1Floor+L2Floor,
      L1Roof+L2Roof };
  
  const double L3[]=
    { L3Front,L3Back,
      L3LeftWall,L3RightWall,
      L3Floor,L3Roof};

  const double* LPtr=L1;
  int BI(buildIndex);
  
  for(size_t index=0;index<18;index+=6)
    {
      setConnect(index,Origin-Y*D[0],-Y);
      setConnect(index+1,Origin+Y*D[1],Y);
      setConnect(index+2,Origin-X*D[2],-X);
      setConnect(index+3,Origin+X*D[3],X);
      setConnect(index+4,Origin-Z*D[4],-Z);
      setConnect(index+5,Origin+Z*D[5],Z);

      setLinkSurf(index,-SMap.realSurf(BI+1));
      setLinkSurf(index+1,SMap.realSurf(BI+2));
      setLinkSurf(index+2,-SMap.realSurf(BI+3));
      setLinkSurf(index+3,SMap.realSurf(BI+4));
      setLinkSurf(index+4,-SMap.realSurf(BI+5));
      setLinkSurf(index+5,SMap.realSurf(BI+6));

      for(size_t i=0;i<6;i++)
	D[i]+=LPtr[i];
      LPtr=L3;
      BI+=(index) ? 10 : 20;
    }
  FixedComp::nameSideIndex(0,"innerFront");
  FixedComp::nameSideIndex(6,"midFront");
  FixedComp::nameSideIndex(12,"outerFront");

  return;
}

void TrexHut::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int FIndex)
{
  ELog::RegMethod RegA("TrexHut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

} // NAMESPACE  essSystem
