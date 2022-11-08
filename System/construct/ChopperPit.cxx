/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperPit.cxx
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
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "ChopperPit.h"

namespace constructSystem
{

ChopperPit::ChopperPit(const std::string& Key) : 
  attachSystem::FixedRotate(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ChopperPit::ChopperPit(const ChopperPit& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  voidHeight(A.voidHeight),voidWidth(A.voidWidth),
  voidDepth(A.voidDepth),voidLength(A.voidLength),
  feHeight(A.feHeight),feDepth(A.feDepth),feWidth(A.feWidth),
  feFront(A.feFront),feBack(A.feBack),
  concHeight(A.concHeight),concDepth(A.concDepth),
  concWidth(A.concWidth),concFront(A.concFront),
  concBack(A.concBack),colletWidth(A.colletWidth),
  colletDepth(A.colletDepth),colletHeight(A.colletHeight),
  feMat(A.feMat),concMat(A.concMat),colletMat(A.colletMat)
  /*!
    Copy constructor
    \param A :: ChopperPit to copy
  */
{}

ChopperPit&
ChopperPit::operator=(const ChopperPit& A)
  /*!
    Assignment operator
    \param A :: ChopperPit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      voidHeight=A.voidHeight;
      voidWidth=A.voidWidth;
      voidDepth=A.voidDepth;
      voidLength=A.voidLength;
      feHeight=A.feHeight;
      feDepth=A.feDepth;
      feWidth=A.feWidth;
      feFront=A.feFront;
      feBack=A.feBack;
      concHeight=A.concHeight;
      concDepth=A.concDepth;
      concWidth=A.concWidth;
      concFront=A.concFront;
      concBack=A.concBack;
      colletWidth=A.colletWidth;
      colletDepth=A.colletDepth;
      colletHeight=A.colletHeight;
      feMat=A.feMat;
      concMat=A.concMat;
      colletMat=A.colletMat;
    }
  return *this;
}

ChopperPit::~ChopperPit() 
  /*!
    Destructor
  */
{}

void
ChopperPit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ChopperPit","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");


  feHeight=Control.EvalVar<double>(keyName+"FeHeight");
  feDepth=Control.EvalVar<double>(keyName+"FeDepth");
  feWidth=Control.EvalVar<double>(keyName+"FeWidth");
  feFront=Control.EvalVar<double>(keyName+"FeFront");
  feBack=Control.EvalVar<double>(keyName+"FeBack");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  concHeight=Control.EvalVar<double>(keyName+"ConcHeight");
  concDepth=Control.EvalVar<double>(keyName+"ConcDepth");
  concWidth=Control.EvalVar<double>(keyName+"ConcWidth");
  concFront=Control.EvalVar<double>(keyName+"ConcFront");
  concBack=Control.EvalVar<double>(keyName+"ConcBack");

  colletDepth=Control.EvalDefVar<double>(keyName+"ColletDepth",0.0);
  colletHeight=Control.EvalDefVar<double>(keyName+"ColletHeight",0.0);
  colletWidth=Control.EvalDefVar<double>(keyName+"ColletWidth",0.0);
  colletMat=ModelSupport::EvalDefMat(Control,keyName+"ColletMat",-1);

  if (colletDepth*colletHeight*colletWidth<Geometry::zeroTol)
    colletMat=-1;
  
  return;
}

void
ChopperPit::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ChopperPit","createSurfaces");

  // Inner void
  if (!isActive("front"))
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
      
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  

  // Fe system [front face is link surf]
  if (!isActive("front"))
    ModelSupport::buildPlane(SMap,buildIndex+11,
			     Origin-Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(voidDepth+feDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(voidHeight+feHeight),Z);  

  // Conc system
  if (!isActive("front"))
    ModelSupport::buildPlane(SMap,buildIndex+21,
			     Origin-Y*(feFront+concFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(feBack+concBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(feWidth+concWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(feWidth+concWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(voidDepth+feDepth+concDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(voidHeight+feHeight+concHeight),Z);  

  if (colletMat>=0)
    {
      // Collet on iron exit wall:
      ModelSupport::buildPlane(SMap,buildIndex+102,
			       Origin+Y*(colletDepth+voidLength/2.0),Y);  
      ModelSupport::buildPlane(SMap,buildIndex+103,
			       Origin-X*(colletWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+104,
			       Origin+X*(colletWidth/2.0),X);  
      ModelSupport::buildPlane(SMap,buildIndex+105,
			       Origin-Z*(colletDepth/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+106,
			       Origin+Z*(colletDepth/2.0),Z);
    }


  return;
}

void
ChopperPit::createObjects(Simulation& System)
  /*!
    Adds the main boxes
    Note that setcomposite is used because front surface
    may/maynot have been made
    
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ChopperPit","createObjects");


  HeadRule HR;
  const HeadRule frontHR=getRule("front"); // returns null if not set

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("Void",System,cellIndex++,0,0.0,HR*frontHR);

  HeadRule Collet;
  if (colletMat>=0)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -102 103 -104 105 -106");
      makeCell("Collet",System,cellIndex++,colletMat,0.0,HR);
      Collet=HR.complement();
    }

  // Split into three to allow beampiles more readilty
  // front:
  if (!isActive("front"))
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 3 -4 5 -6");
      makeCell("MidLayerFront",System,cellIndex++,feMat,0.0,HR);
      addCell("MidLayer",cellIndex-1);
    }
    
  // Sides:
  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"11 -12 13 -14 15 -16 (-3:4:-5:6)");
  makeCell("MidLayerSide",System,cellIndex++,feMat,0.0,HR*frontHR);

  // back
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 3 -4 5 -6");
  makeCell("MidLayerBack",System,cellIndex++,feMat,0.0,HR*Collet);
  addCell("MidLayer",cellIndex-1);
  
  // Make full exclude:
  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)");
  makeCell("Outer",System,cellIndex++,concMat,0.0,HR*frontHR);

  // Exclude:
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"21 -22 23 -24 25 -26");
  addOuterSurf(HR*frontHR);      

  return;
}
  
void
ChopperPit::createFrontLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ChopperPit","createLinks");


  setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  setLinkSurf(0,-SMap.realSurf(buildIndex+1));
	  
  // Fe system [front face is link surf]

  setConnect(6,Origin-Y*(feFront+voidLength/2.0),-Y);
  setLinkSurf(6,-SMap.realSurf(buildIndex+11));

  // Conc esction
  setConnect(12,Origin-Y*(concFront+feFront+voidLength/2.0),Y);
  setLinkSurf(12,-SMap.realSurf(buildIndex+21));

  createCommonLinks();
  FixedComp::nameSideIndex(0,"innerFront");
  FixedComp::nameSideIndex(6,"midFront");
  FixedComp::nameSideIndex(12,"outerFront");

  return;
}

void
ChopperPit::createCommonLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ChopperPit","createLinks");

  // note we have already done front 
  double D[5]=
    {voidLength/2.0,
     voidWidth/2.0,voidWidth/2.0,
     voidDepth,voidHeight};
  
  const double L[]=
    {feBack,feWidth,feWidth,feDepth,feHeight,
     concBack,concWidth,concWidth,concDepth,concHeight};

  double* LPtr(L);
  int BI(buildIndex);
  for(size_t index=0;index<18;index+=6)
    {
      setConnect(index+1,Origin+Y*D[0],Y);
      setConnect(index+2,Origin-X*D[1],-X);
      setConnect(index+3,Origin+X*D[2],X);
      setConnect(index+4,Origin-Z*D[3],-Z);
      setConnect(index+5,Origin+Z*D[4],Z);

      setLinkSurf(index+1,SMap.realSurf(BI+2));
      setLinkSurf(index+2,-SMap.realSurf(BI+3));
      setLinkSurf(index+3,SMap.realSurf(BI+4));
      setLinkSurf(index+4,-SMap.realSurf(BI+5));
      setLinkSurf(index+5,SMap.realSurf(BI+6));

      for(size_t i=0;i<5;i++)
	D[i] += *LPtr++;

      BI+=10;
    }
  
  
  return;
}

void
ChopperPit::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("ChopperPit","createLinks");
  
  if (!isActive("front"))
    createFrontLinks();
  createCommonLinks();
  return;
}
  
void
ChopperPit::addFrontWall(const attachSystem::FixedComp& WFC,
			 const long int sideIndex)
  /*!
    Add a front wall
    \param WFC :: Front line
  */
{
  FixedComp::setLinkCopy(0,FC,sideIndex);
  setCutSurf("front",WFC,sideIndex);
  FixedComp::nameSideIndex(0,"innerFront");

  return;
}
  
void
ChopperPit::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("ChopperPit","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,voidLength/2.0+feFront);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

  
}  // NAMESPACE constructSystem
