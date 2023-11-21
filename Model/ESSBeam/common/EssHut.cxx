/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   common/EssHut.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "EssHut.h"

namespace essSystem
{

EssHut::EssHut(const std::string& Key) : 
  attachSystem::FixedRotate(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

EssHut::EssHut(const EssHut& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  voidHeight(A.voidHeight),voidWidth(A.voidWidth),
  voidDepth(A.voidDepth),voidLength(A.voidLength),
  feFront(A.feFront),feLeftWall(A.feLeftWall),
  feRightWall(A.feRightWall),feRoof(A.feRoof),
  feFloor(A.feFloor),feBack(A.feBack),concFront(A.concFront),
  concLeftWall(A.concLeftWall),concRightWall(A.concRightWall),
  concRoof(A.concRoof),concFloor(A.concFloor),
  concBack(A.concBack),feMat(A.feMat),concMat(A.concMat)
  /*!
    Copy constructor
    \param A :: EssHut to copy
  */
{}

EssHut&
EssHut::operator=(const EssHut& A)
  /*!
    Assignment operator
    \param A :: EssHut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
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

EssHut::~EssHut() 
  /*!
    Destructor
  */
{}

void
EssHut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("EssHut","populate");
  
  FixedRotate::populate(Control);

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

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  return;
}



void
EssHut::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("EssHut","createSurfaces");

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
EssHut::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("EssHut","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  makeCell("Iron",System,cellIndex++,feMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
		"1 -22 23 -24 25 -26 (12:-13:14:-15:16)");
  makeCell("Conc",System,cellIndex++,concMat,0.0,HR);

  // Front wall:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 13 -14 15 -16");
  makeCell("FrontWall",System,cellIndex++,feMat,0.0,HR);
  setCell("IronFront",cellIndex-1);
  
  // Ring of concrete
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -1 23 -24 25 -26 (-13:14:-15:16)");
  makeCell("ConcWall",System,cellIndex++,concMat,0.0,HR);

  // Front concrete face
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -11 23 -24 25 -26");
  makeCell("FrontWall",System,cellIndex++,concMat,0.0,HR);
  
  // Exclude:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 23 -24  25 -26");
  addOuterSurf(HR);      

  return;
}

void
EssHut::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("EssHut","createLinks");
    
  // INNER VOID
  setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  setConnect(1,Origin+Y*(voidLength/2.0),Y);
  setConnect(2,Origin-X*(voidWidth/2.0),-X);
  setConnect(3,Origin+X*(voidWidth/2.0),X);
  setConnect(4,Origin-Z*voidDepth,-Z);
  setConnect(5,Origin+Z*voidHeight,Z);  

  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  setLinkSurf(1,SMap.realSurf(buildIndex+2));
  setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  setLinkSurf(3,SMap.realSurf(buildIndex+4));
  setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  setLinkSurf(5,SMap.realSurf(buildIndex+6));

  // MID VOID
  setConnect(6,Origin-Y*(feFront+voidLength/2.0),-Y);
  setConnect(7,Origin+Y*(feBack+voidLength/2.0),Y);
  setConnect(8,Origin-X*(feLeftWall+voidWidth/2.0),-X);
  setConnect(9,Origin+X*(feRightWall+voidWidth/2.0),X);
  setConnect(10,Origin-Z*(feFloor+voidDepth),-Z);
  setConnect(11,Origin+Z*(feRoof+voidHeight),Z);  

  setLinkSurf(6,-SMap.realSurf(buildIndex+11));
  setLinkSurf(7,SMap.realSurf(buildIndex+12));
  setLinkSurf(8,-SMap.realSurf(buildIndex+13));
  setLinkSurf(9,SMap.realSurf(buildIndex+14));
  setLinkSurf(10,-SMap.realSurf(buildIndex+15));
  setLinkSurf(11,SMap.realSurf(buildIndex+16));


    // OUTER VOID
  setConnect(12,Origin-Y*(feFront+concFront+voidLength/2.0),-Y);
  setConnect(13,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  setConnect(14,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),-X);
  setConnect(15,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  setConnect(16,Origin-Z*(concFloor+feFloor+voidDepth),-Z);
  setConnect(17,Origin+Z*(concRoof+feRoof+voidHeight),Z);  

  setLinkSurf(12,-SMap.realSurf(buildIndex+21));
  setLinkSurf(13,SMap.realSurf(buildIndex+22));
  setLinkSurf(14,-SMap.realSurf(buildIndex+23));
  setLinkSurf(15,SMap.realSurf(buildIndex+24));
  setLinkSurf(16,-SMap.realSurf(buildIndex+25));
  setLinkSurf(17,SMap.realSurf(buildIndex+26));

  nameSideIndex(0,"InnerFront");
  nameSideIndex(6,"MidFront");
  nameSideIndex(12,"OuterFront");
  return;
}

void
EssHut::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("EssHut","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,voidLength/2.0+feFront);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
