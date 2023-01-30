/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/JawValveBase
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
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "JawUnit.h"
#include "JawValveBase.h" 

namespace constructSystem
{

JawValveBase::JawValveBase(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  JItem(Key+"Jaw")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

JawValveBase::JawValveBase(const JawValveBase& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),attachSystem::CellMap(A),
  attachSystem::SurfMap(A),attachSystem::FrontBackCut(A),  
  length(A.length),wallThick(A.wallThick),portARadius(A.portARadius),
  portAThick(A.portAThick),portALen(A.portALen),
  portBRadius(A.portBRadius),portBThick(A.portBThick),
  portBLen(A.portBLen),JItem(A.JItem),voidMat(A.voidMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: JawValveBase to copy
  */
{}

JawValveBase&
JawValveBase::operator=(const JawValveBase& A)
  /*!
    Assignment operator
    \param A :: JawValveBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      wallThick=A.wallThick;
      portARadius=A.portARadius;
      portAThick=A.portAThick;
      portALen=A.portALen;
      portBRadius=A.portBRadius;
      portBThick=A.portBThick;
      portBLen=A.portBLen;
      JItem=A.JItem;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}


JawValveBase::~JawValveBase() 
  /*!
    Destructor
  */
{}

void
JawValveBase::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("JawValveBase","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  portARadius=Control.EvalPair<double>(keyName+"PortARadius",
				       keyName+"PortRadius");
  portAThick=Control.EvalPair<double>(keyName+"PortAThick",
				      keyName+"PortThick");
  portALen=Control.EvalPair<double>(keyName+"PortALen",
				    keyName+"PortLen");

  portBRadius=Control.EvalPair<double>(keyName+"PortBRadius",
				       keyName+"PortRadius");
  portBThick=Control.EvalPair<double>(keyName+"PortBThick",
				      keyName+"PortThick");
  portBLen=Control.EvalPair<double>(keyName+"PortBLen",
				    keyName+"PortLen");


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
JawValveBase::createSurfaces()
  /*!
    Create the surfaces
    If front/back given it is at portLen from the wall and 
    length/2+portLen from origin.
  */
{
  ELog::RegMethod RegA("JawValveBase","createSurfaces");

  // front planes
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+11,Origin-Y*(wallThick+length/2.0),Y);
  
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
			       Origin-Y*(portALen+length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+101));
    }
  
  // back planes
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(wallThick+length/2.0),Y);
  
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+102,
			       Origin+Y*(portBLen+length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+102));
    }


  // flange

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,portARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,portARadius+portAThick);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portBRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,portBRadius+portBThick);
  
  return;
}

  
void
JawValveBase::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("JawValveBase","createLinks");

  //stufff for intersection

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}

void
JawValveBase::createOuterObjects(Simulation& System)
  /*!
    Adds outer objects  of the JawValve
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("JawValveCylinder","createObjects");

  HeadRule HR;

  const bool portAExtends(wallThick<=portALen);  // port extends
  const bool portBExtends(wallThick<=portBLen);  // port extends

  const HeadRule& frontHR=getFrontRule();  // 101
  const HeadRule& backHR=getBackRule();    // -102
  const HeadRule frontComp=frontHR.complement();
  const HeadRule backComp=backHR.complement();    // 102


  // seal ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 107 -117");
  makeCell("FrontSeal",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -107");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
  
  if (!portAExtends)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -117");
      makeCell("FrontVoidExtra",System,cellIndex++,voidMat,0.0,HR*frontComp);
    }
       
  // seal ring
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 207 -217");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -207");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);
  
  if (!portBExtends)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -217");
      makeCell("BackVoidExtra",System,cellIndex++,voidMat,0.0,HR*backComp);
    }
  
  if (portAExtends || portBExtends)
    {
      if (!portAExtends)
	HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -217");
      else if (!portBExtends)
	HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -117");
      else 
	HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-1 -117): (2 -217)");
      addOuterUnionSurf(HR*frontHR*backHR);
    }
  return;
}

  
void
JawValveBase::createJaws(Simulation& System)
  /*!
    Create the jaws
    \param System :: Simuation to use
   */
{
  ELog::RegMethod RegA("JawValveBase","creatJaws");

  JItem.addInsertCell(this->getCells("Void"));
  JItem.createAll(System,*this,0);
  return;
}
  
void
JawValveBase::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("JawValveTube","createAll(FC)");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,length/2.0+portALen);

  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  createJaws(System);
  
  return;
}

}  // NAMESPACE constructSystem
