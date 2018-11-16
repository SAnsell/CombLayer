/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   loki/LokiHut.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
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

#include "LokiHut.h"

namespace essSystem
{

LokiHut::LokiHut(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

LokiHut::LokiHut(const LokiHut& A) : 
  attachSystem::FixedOffsetGroup(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),  
  voidHeight(A.voidHeight),
  voidWidth(A.voidWidth),voidDepth(A.voidDepth),
  voidLength(A.voidLength),feFront(A.feFront),feLeftWall(A.feLeftWall),
  feRightWall(A.feRightWall),feRoof(A.feRoof),feFloor(A.feFloor),
  feBack(A.feBack),concFront(A.concFront),concLeftWall(A.concLeftWall),
  concRightWall(A.concRightWall),concRoof(A.concRoof),
  concFloor(A.concFloor),concBack(A.concBack),feMat(A.feMat),
  concMat(A.concMat)
  /*!
    Copy constructor
    \param A :: LokiHut to copy
  */
{}

LokiHut&
LokiHut::operator=(const LokiHut& A)
  /*!
    Assignment operator
    \param A :: LokiHut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      voidHeight=A.voidHeight;
      voidWidth=A.voidWidth;
      voidDepth=A.voidDepth;
      voidLength=A.voidLength;
      feFront=A.feFront;
      feLeftWall=A.feLeftWall;
      feRightWall=A.feRightWall;
      feRoof=A.feRoof;
      feFloor=A.feFloor;
      feBack=A.feBack;
      concFront=A.concFront;
      concLeftWall=A.concLeftWall;
      concRightWall=A.concRightWall;
      concRoof=A.concRoof;
      concFloor=A.concFloor;
      concBack=A.concBack;
      feMat=A.feMat;
      concMat=A.concMat;
    }
  return *this;
}

  
LokiHut::~LokiHut() 
  /*!
    Destructor
  */
{}

void
LokiHut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("LokiHut","populate");
  
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
LokiHut::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("LokiHut","createUnitVector");

  // add nosecone + half centre
  yStep+=voidLength/2.0;
  attachSystem::FixedComp& Outer=getKey("Outer");
  attachSystem::FixedComp& Inner=getKey("Inner");

  Outer.createUnitVector(FC,sideIndex);
  Inner.createUnitVector(FC,sideIndex);
  applyOffset();
  setDefault("Inner");
  return;
}

void
LokiHut::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("LokiHut","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  


  // FE WALLS:
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(feFloor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(feRoof+voidHeight),Z);  


  // CONC WALLS:
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*(concFront+feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+23,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+24,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+25,Origin-Z*(concFloor+feFloor+voidDepth),Z);  
  ModelSupport::buildPlane
    (SMap,buildIndex+26,Origin+Z*(concRoof+feRoof+voidHeight),Z);  


  return;
}

void
LokiHut::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("LokiHut","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Object(cellIndex++,feMat,0.0,Out));
  setCell("Iron",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
		 "1 -22 23 -24 25 -26 (12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  setCell("Conc",cellIndex-1);

  // Front wall:
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -1 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,feMat,0.0,Out));
  setCell("FrontWall",cellIndex-1);

  // Ring of concrete
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -1 23 -24 25 -26 (-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));

  // Front concrete face
  Out=ModelSupport::getComposite(SMap,buildIndex,"21 -11 23 -24 25 -26 ");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  addCell("FrontWall",cellIndex-1);
  
  // Exclude:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 21 -22 23 -24  25 -26 ");
  addOuterSurf(Out);      

  return;
}

void
LokiHut::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("LokiHut","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  // INNER VOID
  innerFC.setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  innerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));

  
    // OUTER VOID
  outerFC.setConnect(0,Origin-Y*(feFront+concFront+voidLength/2.0),-Y);
  outerFC.setConnect(1,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concFloor+feFloor+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concRoof+feRoof+voidHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+21));
  outerFC.setLinkSurf(1,SMap.realSurf(buildIndex+22));
  outerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+23));
  outerFC.setLinkSurf(3,SMap.realSurf(buildIndex+24));
  outerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+25));
  outerFC.setLinkSurf(5,SMap.realSurf(buildIndex+26));

  
  return;
}

void
LokiHut::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("LokiHut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
