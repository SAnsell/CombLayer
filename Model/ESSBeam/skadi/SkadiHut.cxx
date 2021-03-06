/***********************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   skadi/SkadiHut.cxx
 *
 * Copyright (c) 2004-2018 by Tsitohaina Randiamalala & Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "SkadiHut.h"

namespace essSystem
{
SkadiHut::SkadiHut(const std::string& Key) :
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap()
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

void 
SkadiHut::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create unit vector 
    \param FC :: FixedComp
    \param sideInde :: Link index
  */
{
  ELog::RegMethod RegA("SkadiHut","createUnitVector");
  
  yStep+=voidLength/2.0;
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

void SkadiHut::createSurfaces()
{
  ELog::RegMethod RegA("SkadiHut","createSurfaces");

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

   // Cylinder Pipe
   ModelSupport::buildCylinder(SMap,buildIndex+41,Origin,Y,pipeRadius);
   ModelSupport::buildCylinder(SMap,buildIndex+42,Origin,Y,
			       pipeRadius+pipeL1Thickness);
   ModelSupport::buildCylinder(SMap,buildIndex+43,Origin,Y,
			       pipeRadius+pipeL1Thickness+pipeL2Thickness);
   ModelSupport::buildPlane(SMap,buildIndex+51,Origin+Y*pipeLength,Y);

  return;
}

void SkadiHut::createObjects(Simulation& System)
{
  ELog::RegMethod RegA("SkadiHut","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "(1 -2 3 -4 5 -6) : (2 -41 -51) ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -42 41 -51 ");
  System.addCell(MonteCarlo::Object(cellIndex++,pipeL1Mat,0.0,Out));
  setCell("L1Pipe",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -43 42 -51 ");
  System.addCell(MonteCarlo::Object(cellIndex++,pipeL2Mat,0.0,Out));
  setCell("L2Pipe",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -12 13 -14 15 -16 43 (2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Object(cellIndex++,L1Mat,0.0,Out));
  setCell("L1",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,L1Mat,0.0,Out));
  setCell("FrontWall",cellIndex-1);
  setCell("L1Front",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
		 "11 -22 23 -24 25 -26 43 (12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object(cellIndex++,L2Mat,0.0,Out));
  setCell("L2",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-11 21 23 -24 25 -26 ");
  System.addCell(MonteCarlo::Object(cellIndex++,L2Mat,0.0,Out));
  addCell("FrontWall",cellIndex-1);
  setCell("L2Front",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
		 "21 -32 33 -34 35 -36 43 (22:-23:24:-25:26) ");
  System.addCell(MonteCarlo::Object(cellIndex++,L3Mat,0.0,Out));
  setCell("L3",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-21 31 33 -34 35 -36 ");
  System.addCell(MonteCarlo::Object(cellIndex++,L3Mat,0.0,Out));
  addCell("FrontWall",cellIndex-1);
  setCell("L3Front",cellIndex-1); 

  // Exclude:
  Out=ModelSupport::getComposite(SMap,buildIndex,
			 "( 31 -32 33 -34  35 -36 ) : (32 -43 -51) ");
  addOuterSurf(Out);      

  return;
}

void
SkadiHut::createLinks()
{
  ELog::RegMethod RegA("SkadiHut","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  innerFC.setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  innerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));

  
  midFC.setConnect(0,Origin-Y*(L1Front+voidLength/2.0),-Y);
  midFC.setConnect(1,Origin+Y*(L1Back+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(L1LeftWall+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(L1RightWall+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(L1Floor+voidDepth),-Z);
  midFC.setConnect(5,Origin+Z*(L1Roof+voidHeight),Z);  

  midFC.setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  midFC.setLinkSurf(1,SMap.realSurf(buildIndex+12));
  midFC.setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  midFC.setLinkSurf(3,SMap.realSurf(buildIndex+14));
  midFC.setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  midFC.setLinkSurf(5,SMap.realSurf(buildIndex+16));

  outerFC.setConnect(0,Origin-Y*(L1Front+L2Front+L3Front+voidLength/2.0),-Y);
  outerFC.setConnect(1,Origin+Y*(L1Back+L2Back+L3Back+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(L1LeftWall+L2LeftWall+L3LeftWall+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(L1RightWall+L2RightWall+L3RightWall+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(L1Floor+L2Floor+L3Floor+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(L1Roof+L2Roof+L3Floor+voidHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+31));
  outerFC.setLinkSurf(1,SMap.realSurf(buildIndex+32));
  outerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+33));
  outerFC.setLinkSurf(3,SMap.realSurf(buildIndex+34));
  outerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+35));
  outerFC.setLinkSurf(5,SMap.realSurf(buildIndex+36));

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
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

}//Emd namespace
