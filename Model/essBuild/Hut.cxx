/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essModel/Hut.cxx
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

#include "Hut.h"

namespace essSystem
{

Hut::Hut(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  hutIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hutIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

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
  feNoseWall=Control.EvalVar<double>(keyName+"FeNoseWall");
  feNoseSide=Control.EvalVar<double>(keyName+"FeNoseSide");
  feBack=Control.EvalVar<double>(keyName+"FeBack");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

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
  yStep+=voidLength/2.0+vodNoseLen+feNoseWall;

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
  ModelSupport::buildPlane(SMap,hutIndex+106,
			   Origin+Z*(feRoof+voidHeight),Z);  

  
  // Fe system [front face is link surf]
  ModelSupport::buildPlane(SMap,hutIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+13,
			   Origin-X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+14,
			   Origin+X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+15,
			   Origin-Z*(voidDepth+feDepth),Z);
  ModelSupport::buildPlane(SMap,hutIndex+16,
			   Origin+Z*(voidHeight+feHeight),Z);  

  // Conc system
  ModelSupport::buildPlane(SMap,hutIndex+21,
			   Origin-Y*(feFront+concFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+22,
			   Origin+Y*(feBack+concBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,hutIndex+23,
			   Origin-X*(feWidth+concWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+24,
			   Origin+X*(feWidth+concWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,hutIndex+25,
			   Origin-Z*(voidDepth+feDepth+concDepth),Z);
  ModelSupport::buildPlane(SMap,hutIndex+26,
			   Origin+Z*(voidHeight+feHeight+concHeight),Z);  

  return;
}

void
Hut::createObjects(Simulation& System,
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
  ELog::RegMethod RegA("Hut","createObjects");

  std::string Out;

  HeadRule frontFace(FC.getSignedLinkString(sideIndex));

  // Void 
  Out=ModelSupport::getComposite(SMap,hutIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,hutIndex,
				 " -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   frontFace.display()));
  setCell("MidLayer",cellIndex-1);
  
  // Make full exclude:
  Out=ModelSupport::getComposite(SMap,hutIndex," -12 13 -14 15 -16 ");
  frontFace.addIntersection(Out);
  frontFace.makeComplement();
  Out=ModelSupport::getComposite(SMap,hutIndex," 21 -22 23 -24 25 -26 ");
  Out+=frontFace.display()+" "+cutRule;
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  setCell("Outer",cellIndex-1);

  // Exclude:
  Out=ModelSupport::getComposite(SMap,hutIndex," 21 -22 23 -24 25 -26 ");
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
	  
  // Fe system [front face is link surf]

  midFC.setConnect(1,Origin+Y*(feBack+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(feWidth+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(feWidth+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(voidDepth+feDepth),-Z);
  midFC.setConnect(5,Origin+Z*(voidHeight+feHeight),Z);  

  midFC.setLinkSurf(0,-SMap.realSurf(hutIndex+11));
  midFC.setLinkSurf(1,SMap.realSurf(hutIndex+12));
  midFC.setLinkSurf(2,-SMap.realSurf(hutIndex+13));
  midFC.setLinkSurf(3,SMap.realSurf(hutIndex+14));
  midFC.setLinkSurf(4,-SMap.realSurf(hutIndex+15));
  midFC.setLinkSurf(5,SMap.realSurf(hutIndex+16));

  // Conc esction
  outerFC.setConnect(0,Origin-Y*(concFront+feFront+voidLength/2.0),Y);
  outerFC.setConnect(1,Origin+Y*(concBack+feBack+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concWidth+feWidth+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concWidth+feWidth+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concDepth+voidDepth+feDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concHeight+voidHeight+feHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(hutIndex+21));
  outerFC.setLinkSurf(1,SMap.realSurf(hutIndex+22));
  outerFC.setLinkSurf(2,-SMap.realSurf(hutIndex+23));
  outerFC.setLinkSurf(3,SMap.realSurf(hutIndex+24));
  outerFC.setLinkSurf(4,-SMap.realSurf(hutIndex+25));
  outerFC.setLinkSurf(5,SMap.realSurf(hutIndex+26));

  
  
  
  return;
}

void
Hut::createAll(Simulation& System,
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
  ELog::RegMethod RegA("Hut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System,FC,FIndex,cutRule);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
