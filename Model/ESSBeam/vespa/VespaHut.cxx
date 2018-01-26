/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essModel/VespaHut.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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

#include "VespaHut.h"

namespace essSystem
{

VespaHut::VespaHut(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  hutIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VespaHut::~VespaHut() 
  /*!
    Destructor
  */
{}

void
VespaHut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VespaHut","populate");
  
  FixedOffsetGroup::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  
  feFront=Control.EvalVar<double>(keyName+"FeFront");
  feLeftWall=Control.EvalVar<double>(keyName+"FeLeftWall");
  feRightWall=Control.EvalVar<double>(keyName+"FeRightWall");
  feRoof=Control.EvalVar<double>(keyName+"FeRoof");
  feBack=Control.EvalVar<double>(keyName+"FeBack");

  concFront=Control.EvalVar<double>(keyName+"ConcFront");
  concLeftWall=Control.EvalVar<double>(keyName+"ConcLeftWall");
  concRightWall=Control.EvalVar<double>(keyName+"ConcRightWall");
  concRoof=Control.EvalVar<double>(keyName+"ConcRoof");
  concBack=Control.EvalVar<double>(keyName+"ConcBack");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  return;
}

void
VespaHut::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VespaHut","createUnitVector");

  // add nosecone + half centre
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
 
void
VespaHut::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VespaHut","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,hutIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,hutIndex+6,Origin+Z*voidHeight,Z);  

  // FE WALLS:
  ModelSupport::buildPlane(SMap,hutIndex+11,
			   Origin-Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+13,
			   Origin-X*(feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+14,
			   Origin+X*(feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+15,
			   Origin-Z*(feFloor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,hutIndex+16,
			   Origin+Z*(feRoof+voidHeight),Z);  


  // CONC WALLS:
  ModelSupport::buildPlane(SMap,hutIndex+21,
			   Origin-Y*(concFront+feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+22,
			   Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,hutIndex+23,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,hutIndex+24,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,hutIndex+25,Origin-Z*(concFloor+feFloor+voidDepth),Z);  
  ModelSupport::buildPlane
    (SMap,hutIndex+26,Origin+Z*(concRoof+feRoof+voidHeight),Z);  


  return;
}

void
VespaHut::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VespaHut","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,hutIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "1 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  setCell("Iron",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,
		 "1 -22 23 -24 25 -26 (12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  setCell("Conc",cellIndex-1);

  // Front wall:
  Out=ModelSupport::getComposite(SMap,hutIndex,"11 -1 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  setCell("FrontWall",cellIndex-1);
  setCell("IronFront",cellIndex-1);

  // Ring of concrete
  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "11 -1 23 -24 25 -26 (-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));

  // Front concrete face
  Out=ModelSupport::getComposite(SMap,hutIndex,"21 -11 23 -24 25 -26 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  addCell("FrontWall",cellIndex-1);
  setCell("ConcFront",cellIndex-1);
  
  // Exclude:
  Out=ModelSupport::getComposite
    (SMap,hutIndex," 21 -22 23 -24  25 -26 ");
  addOuterSurf(Out);      

  return;
}

void
VespaHut::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VespaHut","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  // INNER VOID
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

  
  // MID Fe Layer
  midFC.setConnect(0,Origin-Y*(feFront+voidLength/2.0),-Y);
  midFC.setConnect(1,Origin+Y*(feBack+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(feLeftWall+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(feRightWall+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(feFloor+voidDepth),-Z);
  midFC.setConnect(5,Origin+Z*(feRoof+voidHeight),Z);  

  midFC.setLinkSurf(0,-SMap.realSurf(hutIndex+11));
  midFC.setLinkSurf(1,SMap.realSurf(hutIndex+12));
  midFC.setLinkSurf(2,-SMap.realSurf(hutIndex+13));
  midFC.setLinkSurf(3,SMap.realSurf(hutIndex+14));
  midFC.setLinkSurf(4,-SMap.realSurf(hutIndex+15));
  midFC.setLinkSurf(5,SMap.realSurf(hutIndex+16));

  
    // OUTER VOID
  outerFC.setConnect(0,Origin-Y*(feFront+concFront+voidLength/2.0),-Y);
  outerFC.setConnect(1,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concFloor+feFloor+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concRoof+feRoof+voidHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(hutIndex+21));
  outerFC.setLinkSurf(1,SMap.realSurf(hutIndex+22));
  outerFC.setLinkSurf(2,-SMap.realSurf(hutIndex+23));
  outerFC.setLinkSurf(3,SMap.realSurf(hutIndex+24));
  outerFC.setLinkSurf(4,-SMap.realSurf(hutIndex+25));
  outerFC.setLinkSurf(5,SMap.realSurf(hutIndex+26));

  
  return;
}

void
VespaHut::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VespaHut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
