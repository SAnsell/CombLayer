/***********************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   trex/TrexHut.cxx
 *
 * Copyright (c) 2004-2017 by Tsitohaina Randiamalala & Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "TrexHut.h"

namespace essSystem
{
TrexHut::TrexHut(const std::string& Key) :
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  hutIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutIndex+1)
{}

TrexHut::~TrexHut()
{}

void TrexHut::populate(const FuncDataBase& Control)
{
  ELog::RegMethod RegA("TrexHut","populate");

  FixedOffsetGroup::populate(Control);

  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");

  L1Front=Control.EvalVar<double>(keyName+"L1Front");
  L1LeftWall=Control.EvalVar<double>(keyName+"L1LeftWall");
  L1RightWall=Control.EvalVar<double>(keyName+"L1RightWall");
  L1Roof=Control.EvalVar<double>(keyName+"L1Roof");
  L1Back=Control.EvalVar<double>(keyName+"L1Back");
  
  L2Front=Control.EvalVar<double>(keyName+"L2Front");
  L2LeftWall=Control.EvalVar<double>(keyName+"L2LeftWall");
  L2RightWall=Control.EvalVar<double>(keyName+"L2RightWall");
  L2Roof=Control.EvalVar<double>(keyName+"L2Roof");
  L2Back=Control.EvalVar<double>(keyName+"L2Back");

  L3Front=Control.EvalVar<double>(keyName+"L3Front");
  L3LeftWall=Control.EvalVar<double>(keyName+"L3LeftWall");
  L3RightWall=Control.EvalVar<double>(keyName+"L3RightWall");
  L3Roof=Control.EvalVar<double>(keyName+"L3Roof");
  L3Back=Control.EvalVar<double>(keyName+"L3Back");

  L1Mat=ModelSupport::EvalMat<int>(Control,keyName+"L1Mat");
  L2Mat=ModelSupport::EvalMat<int>(Control,keyName+"L2Mat");
  L3Mat=ModelSupport::EvalMat<int>(Control,keyName+"L3Mat");

  return;
}
  
void 
TrexHut::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
{
  ELog::RegMethod RegA("TrexHut","createUnitVector");
  
  yStep+=0.0;//voidLength/2.0;
  attachSystem::FixedComp& Outer=getKey("Outer");
  attachSystem::FixedComp& Mid=getKey("Mid");
  attachSystem::FixedComp& Inner=getKey("Inner");
  
  Outer.createUnitVector(FC,sideIndex);
  Mid.createUnitVector(FC,sideIndex);
  Inner.createUnitVector(FC,sideIndex);
  applyOffset();
  setDefault("Inner");
  return;
}

void 
TrexHut::createSurfaces()
{
  ELog::RegMethod RegA("TrexHut","createSurfaces");

  ModelSupport::buildPlane(SMap,hutIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,hutIndex+6,Origin+Z*voidHeight,Z); 


  // L1 WALLS:
  ModelSupport::buildPlane(SMap,hutIndex+11,
			   Origin-Y*(L1Front+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+12,
			   Origin+Y*(L1Back+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+13,
			   Origin-X*(L1LeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+14,
			   Origin+X*(L1RightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+15,
			   Origin-Z*(L1Floor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,hutIndex+16,
			   Origin+Z*(L1Roof+voidHeight),Z);

  // L2 WALLS:
  ModelSupport::buildPlane(SMap,hutIndex+21,
			   Origin-Y*(L2Front+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+22,
			   Origin+Y*(L2Back+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+23,
			   Origin-X*(L2LeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+24,
			   Origin+X*(L2RightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+25,
			   Origin-Z*(L2Floor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,hutIndex+26,
			   Origin+Z*(L2Roof+voidHeight),Z);

  // L3 WALLS:
   ModelSupport::buildPlane(SMap,hutIndex+31,
			    Origin-Y*(L3Front+voidLength/2.0),Y);
   ModelSupport::buildPlane(SMap,hutIndex+32,
			    Origin+Y*(L3Back+voidLength/2.0),Y);
   ModelSupport::buildPlane(SMap,hutIndex+33,
			    Origin-X*(L3LeftWall+voidWidth/2.0),X);
   ModelSupport::buildPlane(SMap,hutIndex+34,
			    Origin+X*(L3RightWall+voidWidth/2.0),X);
   ModelSupport::buildPlane(SMap,hutIndex+35,
			    Origin-Z*(L3Floor+voidDepth),Z);  
   ModelSupport::buildPlane(SMap,hutIndex+36,
			    Origin+Z*(L3Roof+voidHeight),Z);
}

void TrexHut::createObjects(Simulation& System)
{
  ELog::RegMethod RegA("TrexHut","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,hutIndex,"1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "1 -12 13 -14 15 -16 (2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,L1Mat,0.0,Out));
  setCell("L1",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex," -1 11 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,L1Mat,0.0,Out));
  setCell("FrontWall",cellIndex-1);
  setCell("L1Front",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,
		 "11 -22 23 -24 25 -26 (12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,L2Mat,0.0,Out));
  setCell("L2",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,"-11 21 23 -24 25 -26 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,L2Mat,0.0,Out));
  addCell("FrontWall",cellIndex-1);
  setCell("L2Front",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,
		 "21 -32 33 -34 35 -36 (22:-23:24:-25:26) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,L3Mat,0.0,Out));
  setCell("L3",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,"-21 31 33 -34 35 -36 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,L3Mat,0.0,Out));
  addCell("FrontWall",cellIndex-1);
  setCell("L3Front",cellIndex-1); 

  // Exclude:
  Out=ModelSupport::getComposite(SMap,hutIndex,
			 " 31 -32 33 -34  35 -36 ");
  addOuterSurf(Out);      

  return;
}

void TrexHut::createLinks()
{
  ELog::RegMethod RegA("TrexHut","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  innerFC.setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(0,-SMap.realSurf(hutIndex+1));
  innerFC.setLinkSurf(1,SMap.realSurf(hutIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(hutIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(hutIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(hutIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(hutIndex+6));

  
  midFC.setConnect(0,Origin-Y*(L1Front+L2Front+voidLength/2.0),-Y);
  midFC.setConnect(1,Origin+Y*(L1Back+L2Back+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(L1LeftWall+L2LeftWall+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(L1RightWall+L2RightWall+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(L1Floor+L2Floor+voidDepth),-Z);
  midFC.setConnect(5,Origin+Z*(L1Roof+L2Roof+voidHeight),Z);  

  midFC.setLinkSurf(0,-SMap.realSurf(hutIndex+21));
  midFC.setLinkSurf(1,SMap.realSurf(hutIndex+22));
  midFC.setLinkSurf(2,-SMap.realSurf(hutIndex+23));
  midFC.setLinkSurf(3,SMap.realSurf(hutIndex+24));
  midFC.setLinkSurf(4,-SMap.realSurf(hutIndex+25));
  midFC.setLinkSurf(5,SMap.realSurf(hutIndex+26));

  outerFC.setConnect(0,Origin-Y*(L1Front+L2Front+L3Front+voidLength/2.0),-Y);
  outerFC.setConnect(1,Origin+Y*(L1Back+L2Back+L3Back+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(L1LeftWall+L2LeftWall+L3LeftWall+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(L1RightWall+L2RightWall+L3RightWall+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(L1Floor+L2Floor+L3Floor+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(L1Roof+L2Roof+L3Floor+voidHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(hutIndex+31));
  outerFC.setLinkSurf(1,SMap.realSurf(hutIndex+32));
  outerFC.setLinkSurf(2,-SMap.realSurf(hutIndex+33));
  outerFC.setLinkSurf(3,SMap.realSurf(hutIndex+34));
  outerFC.setLinkSurf(4,-SMap.realSurf(hutIndex+35));
  outerFC.setLinkSurf(5,SMap.realSurf(hutIndex+36));

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

}//End namespace
