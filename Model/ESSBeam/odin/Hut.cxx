/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/Hut.cxx
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
#include "SurfMap.h"

#include "Hut.h"

namespace essSystem
{

Hut::Hut(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),
  hutIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Hut::Hut(const Hut& A) : 
  attachSystem::FixedOffsetGroup(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  hutIndex(A.hutIndex),cellIndex(A.cellIndex),
  voidHeight(A.voidHeight),voidWidth(A.voidWidth),
  voidDepth(A.voidDepth),voidLength(A.voidLength),
  voidNoseLen(A.voidNoseLen),voidNoseWidth(A.voidNoseWidth),
  feLeftWall(A.feLeftWall),feRightWall(A.feRightWall),
  feRoof(A.feRoof),feFloor(A.feFloor),
  feNoseFront(A.feNoseFront),feNoseSide(A.feNoseSide),
  feBack(A.feBack),concLeftWall(A.concLeftWall),
  concRightWall(A.concRightWall),concRoof(A.concRoof),
  concFloor(A.concFloor),concNoseFront(A.concNoseFront),
  concNoseSide(A.concNoseSide),concBack(A.concBack),
  wallYStep(A.wallYStep),wallThick(A.wallThick),
  feMat(A.feMat),concMat(A.concMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: Hut to copy
  */
{}

Hut&
Hut::operator=(const Hut& A)
  /*!
    Assignment operator
    \param A :: Hut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      voidHeight=A.voidHeight;
      voidWidth=A.voidWidth;
      voidDepth=A.voidDepth;
      voidLength=A.voidLength;
      voidNoseLen=A.voidNoseLen;
      voidNoseWidth=A.voidNoseWidth;
      feLeftWall=A.feLeftWall;
      feRightWall=A.feRightWall;
      feRoof=A.feRoof;
      feFloor=A.feFloor;
      feNoseFront=A.feNoseFront;
      feNoseSide=A.feNoseSide;
      feBack=A.feBack;
      concLeftWall=A.concLeftWall;
      concRightWall=A.concRightWall;
      concRoof=A.concRoof;
      concFloor=A.concFloor;
      concNoseFront=A.concNoseFront;
      concNoseSide=A.concNoseSide;
      concBack=A.concBack;
      wallYStep=A.wallYStep;
      wallThick=A.wallThick;
      feMat=A.feMat;
      concMat=A.concMat;
      wallMat=A.wallMat;
    }
  return *this;
}

Hut::~Hut() 
  /*!
    Destructor
  */
{}

void
Hut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Hut","populate");
  
  FixedOffsetGroup::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  voidNoseLen=Control.EvalVar<double>(keyName+"VoidNoseLen");
  voidNoseWidth=Control.EvalVar<double>(keyName+"VoidNoseWidth");
  
  feLeftWall=Control.EvalVar<double>(keyName+"FeLeftWall");
  feRightWall=Control.EvalVar<double>(keyName+"FeRightWall");
  feRoof=Control.EvalVar<double>(keyName+"FeRoof");
  feFloor=Control.EvalVar<double>(keyName+"FeFloor");
  feNoseFront=Control.EvalVar<double>(keyName+"FeNoseFront");
  feNoseSide=Control.EvalVar<double>(keyName+"FeNoseSide");
  feBack=Control.EvalVar<double>(keyName+"FeBack");

  concLeftWall=Control.EvalVar<double>(keyName+"ConcLeftWall");
  concRightWall=Control.EvalVar<double>(keyName+"ConcRightWall");
  concRoof=Control.EvalVar<double>(keyName+"ConcRoof");
  concFloor=Control.EvalVar<double>(keyName+"ConcFloor");
  concNoseFront=Control.EvalVar<double>(keyName+"ConcNoseFront");
  concNoseSide=Control.EvalVar<double>(keyName+"ConcNoseSide");
  concBack=Control.EvalVar<double>(keyName+"ConcBack");

  wallYStep=Control.EvalVar<double>(keyName+"WallYStep");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
Hut::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Hut","createUnitVector");

  // add nosecone + half centre
  yStep+=voidLength/2.0+voidNoseLen+feNoseFront;

  attachSystem::FixedComp& Outer=getKey("Outer");
  attachSystem::FixedComp& Inner=getKey("Inner");

  Outer.createUnitVector(FC,sideIndex);
  Inner.createUnitVector(FC,sideIndex);
  applyOffset();
  setDefault("Inner");
  return;
}

void
Hut::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Hut","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,hutIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,hutIndex+6,Origin+Z*voidHeight,Z);  

  // NOSE:
  ModelSupport::buildPlane(SMap,hutIndex+11,
			   Origin-Y*(voidNoseLen+voidLength/2.0),Y);
  
  ModelSupport::buildPlane
    (SMap,hutIndex+13,
     Origin-Y*(voidNoseLen+voidLength/2.0)-X*(voidNoseWidth/2.0),
     Origin-Y*(voidLength/2.0)-X*(voidWidth/2.0),
     Origin-Y*(voidLength/2.0)-X*(voidWidth/2.0)+Z,
     X);
  
  ModelSupport::buildPlane
    (SMap,hutIndex+14,
     Origin-Y*(voidNoseLen+voidLength/2.0)+X*(voidNoseWidth/2.0),
     Origin-Y*(voidLength/2.0)+X*(voidWidth/2.0),
     Origin-Y*(voidLength/2.0)+X*(voidWidth/2.0)+Z,
     X);


  // FE WALLS:
  ModelSupport::buildPlane(SMap,hutIndex+102,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+103,
			   Origin-X*(feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+104,
			   Origin+X*(feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+105,
			   Origin-Z*(feFloor+voidDepth),Z);  
  ModelSupport::buildPlane(SMap,hutIndex+106,
			   Origin+Z*(feRoof+voidHeight),Z);  


  // FE NOSE:
  ModelSupport::buildPlane(SMap,hutIndex+111,
			   Origin-Y*(feNoseFront+voidNoseLen+voidLength/2.0),Y);
  
  ModelSupport::buildPlane
    (SMap,hutIndex+113,
     Origin-Y*(feNoseFront+voidNoseLen+voidLength/2.0)-X*(voidNoseWidth/2.0),
     Origin-Y*(voidLength/2.0)-X*(feNoseSide+voidWidth/2.0),
     Origin-Y*(voidLength/2.0)-X*(feNoseSide+voidWidth/2.0)+Z,
     X);
  
  ModelSupport::buildPlane
    (SMap,hutIndex+114,
     Origin-Y*(feNoseFront+voidNoseLen+voidLength/2.0)+X*(voidNoseWidth/2.0),
     Origin-Y*(voidLength/2.0)+X*(feNoseSide+voidWidth/2.0),
     Origin-Y*(voidLength/2.0)+X*(feNoseSide+voidWidth/2.0)+Z,
     X);


  // CONC WALLS:
  ModelSupport::buildPlane(SMap,hutIndex+202,
			   Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,hutIndex+203,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,hutIndex+204,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,hutIndex+205,Origin-Z*(concFloor+feFloor+voidDepth),Z);  
  ModelSupport::buildPlane
    (SMap,hutIndex+206,Origin+Z*(concRoof+feRoof+voidHeight),Z);  


  // CONC NOSE:
  ModelSupport::buildPlane
    (SMap,hutIndex+211,Origin-Y*(concNoseFront+feNoseFront+
				 voidNoseLen+voidLength/2.0),Y);
  
  ModelSupport::buildPlane
    (SMap,hutIndex+213,
     Origin-Y*(concNoseFront+feNoseFront+
	       voidNoseLen+voidLength/2.0)-X*(voidNoseWidth/2.0),
     Origin-Y*(voidLength/2.0)-X*(concNoseSide+feNoseSide+voidWidth/2.0),
     Origin-Y*(voidLength/2.0)-X*(concNoseSide+feNoseSide+voidWidth/2.0)+Z,
     X);
  
  ModelSupport::buildPlane
    (SMap,hutIndex+214,
     Origin-Y*(concNoseFront+feNoseSide+
	       voidNoseLen+voidLength/2.0)+X*(voidNoseWidth/2.0),
     Origin-Y*(voidLength/2.0)+X*(concNoseSide+feNoseSide+voidWidth/2.0),
     Origin-Y*(voidLength/2.0)+X*(concNoseSide+feNoseSide+voidWidth/2.0)+Z,
     X);

  // WALL LAYER:
  ModelSupport::buildPlane(SMap,hutIndex+1001,
			   Origin+Y*(wallYStep-voidLength/2.0),Y);  
  ModelSupport::buildPlane(SMap,hutIndex+1002,
			   Origin+Y*(wallYStep+wallThick-voidLength/2.0),Y);
  SurfMap::addSurf("InnerWallFront",hutIndex+1001);
  SurfMap::addSurf("InnerWallBack",hutIndex+1002);
  
  return;
}

void
Hut::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Hut","createObjects");

  const int frontWallCut((wallYStep<0) ? 1 : 0);
  const int backWallCut((wallYStep+wallThick>0) ? 1 : 0);
  std::string Out;

  // Void [main]
  if (backWallCut)
    Out=ModelSupport::getComposite(SMap,hutIndex,"1002 -2 3 -4 5 -6");
  else
    Out=ModelSupport::getComposite(SMap,hutIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("VoidMain",cellIndex-1);

  // Void [front]
  if (frontWallCut)
    Out=ModelSupport::getComposite(SMap,hutIndex,"11 -1001 13 -14 5 -6");
  else
    Out=ModelSupport::getComposite(SMap,hutIndex,"11 -1 13 -14 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("VoidNose",cellIndex-1);
  // EXTRA If required:

  if (!frontWallCut && backWallCut) // completely within nose cone
    {
      Out=ModelSupport::getComposite(SMap,hutIndex,"1 -1001 3 -4 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
    }
  else if (frontWallCut && !backWallCut) // completely within nose cone
    {
      Out=ModelSupport::getComposite(SMap,hutIndex,"1002 -1 13 -14 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
    }
  
  // Make Wall
  Out=ModelSupport::getComposite
    (SMap,hutIndex,"1001 -1002 5 -6 ");

  if (frontWallCut)
    Out+=ModelSupport::getComposite(SMap,hutIndex," 13 -14 ");
  if (backWallCut)
    Out+=ModelSupport::getComposite(SMap,hutIndex," 3 -4 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  setCell("InnerWall",cellIndex-1);


  
  // Fe [main]
  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "1 -102 103 -104 105 -106 "
				 " (2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  setCell("FeMain",cellIndex-1);

  // Fe [nose]
  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "111 -1 113 -114 103 -104 105 -106 "
				 " (-11:-13:14:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  setCell("FeNose",cellIndex-1);


  // Conc [main]
  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "1 213 -214 -202 203 -204 205 -206 "
				 " (102:-103:104:-105:106) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  setCell("ConcMain",cellIndex-1);

  // Conc [nose]
  Out=ModelSupport::getComposite(SMap,hutIndex,
				 "111 -1 213 -214 203 -204 205 -206 "
				 " (-103:-113:104:114:-105:106) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  setCell("ConcNose",cellIndex-1);


  // FRONT CONC SECTION:
  Out=ModelSupport::getComposite(SMap,hutIndex,"-111 211 213 -214 205 -206 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  setCell("ConcNoseFront",cellIndex-1);
  
  // Exclude:
  Out=ModelSupport::getComposite
    (SMap,hutIndex," 211 213 203 -204 -214 -202 203 -204 205 -206 ");
  addOuterSurf(Out);      

  return;
}

void
Hut::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Hut","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  // INNER VOID
  innerFC.setConnect(0,Origin-Y*(voidNoseLen+voidLength/2.0),-Y);
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(0,-SMap.realSurf(hutIndex+11));
  innerFC.setLinkSurf(1,SMap.realSurf(hutIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(hutIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(hutIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(hutIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(hutIndex+6));

  
    // OUTER VOID
  outerFC.setConnect(0,Origin-Y*(feNoseFront+voidNoseLen+voidLength/2.0),-Y);
  outerFC.setConnect(1,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concFloor+feFloor+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concRoof+feRoof+voidHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(hutIndex+111));
  outerFC.setLinkSurf(1,SMap.realSurf(hutIndex+202));
  outerFC.setLinkSurf(2,-SMap.realSurf(hutIndex+203));
  outerFC.setLinkSurf(3,SMap.realSurf(hutIndex+204));
  outerFC.setLinkSurf(4,-SMap.realSurf(hutIndex+205));
  outerFC.setLinkSurf(5,SMap.realSurf(hutIndex+206));

  
  return;
}

void
Hut::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Hut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
