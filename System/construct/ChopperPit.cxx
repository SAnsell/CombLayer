/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperPit.cxx
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

#include "ChopperPit.h"

namespace constructSystem
{

ChopperPit::ChopperPit(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  pitIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pitIndex+1),activeFront(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ChopperPit::ChopperPit(const ChopperPit& A) : 
  attachSystem::FixedOffsetGroup(A)
  ,attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  pitIndex(A.pitIndex),cellIndex(A.cellIndex),
  activeFront(A.activeFront),frontCut(A.frontCut),
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
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      activeFront=A.activeFront;
      frontCut=A.frontCut;
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
  
  FixedOffsetGroup::populate(Control);

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
  colletMat=ModelSupport::EvalDefMat<int>(Control,keyName+"ColletMat",-1);

  if (colletDepth*colletHeight*colletWidth<Geometry::zeroTol)
    colletMat=-1;
  
  return;
}

void
ChopperPit::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ChopperPit","createUnitVector");

  yStep+=voidLength/2.0+feFront;

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
ChopperPit::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ChopperPit","createSurfaces");

  // Inner void
  if (!activeFront)
    ModelSupport::buildPlane(SMap,pitIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,pitIndex+6,Origin+Z*voidHeight,Z);  

  // Fe system [front face is link surf]
  if (!activeFront)
    ModelSupport::buildPlane(SMap,pitIndex+11,
			     Origin-Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+13,
			   Origin-X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+14,
			   Origin+X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+15,
			   Origin-Z*(voidDepth+feDepth),Z);
  ModelSupport::buildPlane(SMap,pitIndex+16,
			   Origin+Z*(voidHeight+feHeight),Z);  

  // Conc system
  if (!activeFront)
    ModelSupport::buildPlane(SMap,pitIndex+21,
			     Origin-Y*(feFront+concFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+22,
			   Origin+Y*(feBack+concBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+23,
			   Origin-X*(feWidth+concWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+24,
			   Origin+X*(feWidth+concWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+25,
			   Origin-Z*(voidDepth+feDepth+concDepth),Z);
  ModelSupport::buildPlane(SMap,pitIndex+26,
			   Origin+Z*(voidHeight+feHeight+concHeight),Z);  

  if (colletMat>=0)
    {
      // Collet on iron exit wall:
      ModelSupport::buildPlane(SMap,pitIndex+102,
			       Origin+Y*(colletDepth+voidLength/2.0),Y);  
      ModelSupport::buildPlane(SMap,pitIndex+103,
			       Origin-X*(colletWidth/2.0),X);
      ModelSupport::buildPlane(SMap,pitIndex+104,
			       Origin+X*(colletWidth/2.0),X);  
      ModelSupport::buildPlane(SMap,pitIndex+105,
			       Origin-Z*(colletDepth/2.0),Z);
      ModelSupport::buildPlane(SMap,pitIndex+106,
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

  std::string Out;
  const std::string frontSurf=frontCut.display();
  // Void 
  Out=ModelSupport::getSetComposite(SMap,pitIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+frontSurf));
  setCell("Void",cellIndex-1);

  HeadRule Collet;
  if (colletMat>=0)
    {
      Out=ModelSupport::getSetComposite(SMap,pitIndex,"2 -102 103 -104 105 -106");
      Collet.procString(Out);
      System.addCell(MonteCarlo::Qhull(cellIndex++,colletMat,0.0,Out));
      Collet.makeComplement();
      setCell("Collet",cellIndex-1);
    }

  // Split into three to allow beampiles more readilty
  // front:
  if (!activeFront)
    {
      Out=ModelSupport::getSetComposite(SMap,pitIndex," 11 -1 3 -4 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+frontSurf));
      addCell("MidLayerFront",cellIndex-1);
      addCell("MidLayer",cellIndex-1);
    }
    
  // Sides:
  Out=ModelSupport::getSetComposite(SMap,pitIndex,"11 -12 13 -14 15 -16 (-3:4:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+frontSurf));
  addCell("MidLayerSide",cellIndex-1);
  // back
  Out=ModelSupport::getSetComposite(SMap,pitIndex,"2 -12 3 -4 5 -6");
  Out+=Collet.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("MidLayerBack",cellIndex-1);
  addCell("MidLayer",cellIndex-1);
  
  // Make full exclude:
  Out=ModelSupport::getSetComposite
    (SMap,pitIndex,"21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out+frontSurf));
  setCell("Outer",cellIndex-1);

  // Exclude:
  Out=ModelSupport::getSetComposite(SMap,pitIndex," 21 -22 23 -24 25 -26 ");
  addOuterSurf(Out+frontSurf);      

  return;
}

void
ChopperPit::createFrontLinks(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the front links from a fixed component
    \param FC :: Front wall
    \param sideIndex :: Link point for front face
  */
{
  ELog::RegMethod RegA("ChopperPit","createFrontLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  innerFC.setLinkSignedCopy(0,FC,sideIndex);
  midFC.setLinkSignedCopy(0,FC,sideIndex);
  outerFC.setLinkSignedCopy(0,FC,sideIndex);

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

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  innerFC.setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  innerFC.setLinkSurf(0,-SMap.realSurf(pitIndex+1));
	  
  // Fe system [front face is link surf]

  midFC.setConnect(0,Origin-Y*(feFront+voidLength/2.0),-Y);
  midFC.setLinkSurf(0,-SMap.realSurf(pitIndex+11));

  // Conc esction
  outerFC.setConnect(0,Origin-Y*(concFront+feFront+voidLength/2.0),Y);
  outerFC.setLinkSurf(0,-SMap.realSurf(pitIndex+21));

  createCommonLinks();
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

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(1,SMap.realSurf(pitIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(pitIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(pitIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(pitIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(pitIndex+6));
	  
  // Fe system [front face is link surf]

  midFC.setConnect(1,Origin+Y*(feBack+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(feWidth+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(feWidth+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(voidDepth+feDepth),-Z);
  midFC.setConnect(5,Origin+Z*(voidHeight+feHeight),Z);  

  midFC.setLinkSurf(1,SMap.realSurf(pitIndex+12));
  midFC.setLinkSurf(2,-SMap.realSurf(pitIndex+13));
  midFC.setLinkSurf(3,SMap.realSurf(pitIndex+14));
  midFC.setLinkSurf(4,-SMap.realSurf(pitIndex+15));
  midFC.setLinkSurf(5,SMap.realSurf(pitIndex+16));

  // Conc esction
  outerFC.setConnect(1,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concWidth+feWidth+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concWidth+feWidth+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concDepth+voidDepth+feDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concHeight+voidHeight+feHeight),Z);  

  outerFC.setLinkSurf(1,SMap.realSurf(pitIndex+22));
  outerFC.setLinkSurf(2,-SMap.realSurf(pitIndex+23));
  outerFC.setLinkSurf(3,SMap.realSurf(pitIndex+24));
  outerFC.setLinkSurf(4,-SMap.realSurf(pitIndex+25));
  outerFC.setLinkSurf(5,SMap.realSurf(pitIndex+26));
  
  return;
}

void
ChopperPit::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("ChopperPit","createLinks");
  
  if (!activeFront)
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
  createFrontLinks(WFC,sideIndex);
  frontCut=WFC.getFullRule(sideIndex);
  activeFront=1;
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
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

  
}  // NAMESPACE constructSystem
