/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/PreMod.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Exception.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "surfExpand.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "PreMod.h"

namespace moderatorSystem
{

PreMod::PreMod(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  centOrgFlag(1),
  divideSurf(0),targetSurf(0),
  rFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PreMod::PreMod(const PreMod& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  centOrgFlag(A.centOrgFlag),width(A.width),height(A.height),
  depth(A.depth),alThickness(A.alThickness),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat),divideSurf(A.divideSurf),
  targetSurf(A.targetSurf),rFlag(A.rFlag)
  /*!
    Copy constructor
    \param A :: PreMod to copy
  */
{}

PreMod&
PreMod::operator=(const PreMod& A)
  /*!
    Assignment operator
    \param A :: PreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      centOrgFlag=A.centOrgFlag;
      width=A.width;
      height=A.height;
      depth=A.depth;
      alThickness=A.alThickness;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      divideSurf=A.divideSurf;
      targetSurf=A.targetSurf;
      rFlag=A.rFlag;
    }
  return *this;
}

PreMod::~PreMod() 
  /*!
    Destructor
  */
{}

void
PreMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function dataase
  */
{
  ELog::RegMethod RegA("PreMod","populate");

  FixedRotate::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  alThickness=Control.EvalVar<double>(keyName+"AlThick");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
    
void
PreMod::createSurfaces()
  /*!
    Create All the surfaces
    \param FC :: Fixed unit that connects to this moderator
    \param baseIndex :: base number
  */
{
  ELog::RegMethod RegA("PreMod","createSurface");

  // Outer DIVIDE PLANE/Cylinder
  if (!isActive("divide"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("divide",SMap.realSurf(buildIndex+1));
    }
  
  //  SMap.addMatch(buildIndex+5,cFlag*FC.getLinkSurf(baseIndex));
  //if (targetSurf)
  //SMap.addMatch(buildIndex+7,targetSurf);  // This is a cylinder [hopefully]

  // Outer surfaces:
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  // Inner surfaces:
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(depth-alThickness),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(width/2.0-alThickness),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(width/2.0-alThickness),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height-alThickness),Z);

  ELog::EM<<"Surface target == "<<getRule("divide")<<ELog::endDiag;
  if (isActive("target"))
    makeExpandedSurf(SMap,"target",buildIndex+17,Origin,alThickness);

  makeExpandedSurf(SMap,"base",buildIndex+15,Origin,-alThickness); 
  ELog::EM<<"Surface 15 == "<<Z<< ":: "<<*SMap.realSurfPtr(buildIndex+15)<<ELog::endDiag;
  if (isActive("divider"))
    {
      ELog::EM<<"Divider "<<keyName<<ELog::endDiag;
      makeExpandedSurf(SMap,"divide",buildIndex+11,
		       Origin,alThickness);
      setCutSurf("divEdge",-SMap.realSurf(buildIndex+11));
    }
  return;
}

void
PreMod::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreMod","createObjects");

  if (keyName=="decPM") return;
  ELog::EM<<"Keyt == "<<keyName<<ELog::endDiag;
  const HeadRule targetHR=getRule("target");
  const HeadRule divideHR=getRule("divide");
  const HeadRule divEdgeHR=getRule("divEdge");
  const HeadRule baseHR=getRule("base");

  ELog::EM<<"Target == "<<targetHR<<ELog::endDiag;
  ELog::EM<<"Base == "<<baseHR<<ELog::endDiag;
  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -4 -6");
  addOuterSurf(HR*targetHR*divideHR*baseHR);

  HR*=ModelSupport::getHeadRule(SMap,buildIndex,
				    "(12:-13:14:-15:16:-17)");
  System.addCell(cellIndex++,alMat,modTemp,HR*divEdgeHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 13 -14 15 -16 17");
  System.addCell(cellIndex++,modMat,modTemp,HR*divEdgeHR);
  return;
}

void
PreMod::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("PreMod","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  
  FixedComp::setConnect(1,Origin+Y*depth,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
        
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,getRule("base"));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  nameSideIndex(4,"minusZ");
  nameSideIndex(5,"plusZ");
    
  return;
}
  
void
PreMod::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int baseIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param baseIndex :: base number
  */
{
  ELog::RegMethod RegA("PreMod","createAll");

  populate(System.getDataBase());  
  FixedRotate::createUnitVector(FC,baseIndex,0);
  setCutSurf("base",FC,baseIndex);

  if (centOrgFlag) Origin-=Y*(depth/2.0);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
