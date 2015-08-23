/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperPit.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "CellMap.h"

#include "ChopperPit.h"

namespace constructSystem
{

ChopperPit::ChopperPit(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  pitIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pitIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

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

  yStep=voidLength/2.0+feFront;

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
  ModelSupport::buildPlane(SMap,pitIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,pitIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,pitIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,pitIndex+6,Origin+Z*voidHeight,Z);  

  // Fe system [front face is link surf]
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
ChopperPit::createObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex,
			  const std::string& cutRule)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
    \param FC :: FixedComp of front face
    \param sideIndex :: Fixed link point of object [signed]
    \param cutRule :: Exclude from the front cut 
   */
{
  ELog::RegMethod RegA("ChopperPit","createObjects");

  std::string Out;

  HeadRule frontFace(FC.getSignedLinkString(sideIndex));

  // Void 
  Out=ModelSupport::getComposite(SMap,pitIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  HeadRule Collet;
  if (colletMat>=0)
    {
      Out=ModelSupport::getComposite(SMap,pitIndex,"2 -102 103 -104 105 -106");
      Collet.procString(Out);
      System.addCell(MonteCarlo::Qhull(cellIndex++,colletMat,0.0,Out));
      setCell("Collet",cellIndex-1);
    }
  
  Out=ModelSupport::getComposite(SMap,pitIndex,
				 " -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  Collet.makeComplement();
  Out+=Collet.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   frontFace.display()));
  addCell("MidLayer",cellIndex-1);
  
  // Make full exclude:
  Out=ModelSupport::getComposite(SMap,pitIndex," -12 13 -14 15 -16 ");
  frontFace.addIntersection(Out);
  frontFace.makeComplement();
  Out=ModelSupport::getComposite(SMap,pitIndex," 21 -22 23 -24 25 -26 ");
  Out+=frontFace.display()+" "+cutRule;
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  setCell("Outer",cellIndex-1);

  // Exclude:
  Out=ModelSupport::getComposite(SMap,pitIndex," 21 -22 23 -24 25 -26 ");
  addOuterSurf(Out);      

  return;
}

void
ChopperPit::createLinks()
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
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(0,-SMap.realSurf(pitIndex+1));
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

  midFC.setLinkSurf(0,-SMap.realSurf(pitIndex+11));
  midFC.setLinkSurf(1,SMap.realSurf(pitIndex+12));
  midFC.setLinkSurf(2,-SMap.realSurf(pitIndex+13));
  midFC.setLinkSurf(3,SMap.realSurf(pitIndex+14));
  midFC.setLinkSurf(4,-SMap.realSurf(pitIndex+15));
  midFC.setLinkSurf(5,SMap.realSurf(pitIndex+16));

  // Conc esction
  outerFC.setConnect(0,Origin-Y*(concFront+feFront+voidLength/2.0),Y);
  outerFC.setConnect(1,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concWidth+feWidth+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concWidth+feWidth+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concDepth+voidDepth+feDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concHeight+voidHeight+feHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(pitIndex+21));
  outerFC.setLinkSurf(1,SMap.realSurf(pitIndex+22));
  outerFC.setLinkSurf(2,-SMap.realSurf(pitIndex+23));
  outerFC.setLinkSurf(3,SMap.realSurf(pitIndex+24));
  outerFC.setLinkSurf(4,-SMap.realSurf(pitIndex+25));
  outerFC.setLinkSurf(5,SMap.realSurf(pitIndex+26));

  
  
  
  return;
}

void
ChopperPit::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex,
		      const std::string& cutRule)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
    \param cutRule :: Cut for main exclude
  */
{
  ELog::RegMethod RegA("ChopperPit","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System,FC,FIndex,cutRule);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
