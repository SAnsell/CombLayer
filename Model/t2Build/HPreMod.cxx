/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/HPreMod.cxx
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
#include "BaseMap.h"
#include "SurfMap.h"
#include "HPreMod.h"

namespace moderatorSystem
{

HPreMod::HPreMod(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

HPreMod::HPreMod(const HPreMod& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::SurfMap(A),
  width(A.width),height(A.height),
  depth(A.depth),alThickness(A.alThickness),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: HPreMod to copy
  */
{}

HPreMod&
HPreMod::operator=(const HPreMod& A)
  /*!
    Assignment operator
    \param A :: HPreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::SurfMap::operator=(A);
      width=A.width;
      height=A.height;
      depth=A.depth;
      alThickness=A.alThickness;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

HPreMod::~HPreMod() 
  /*!
    Destructor
  */
{}

void
HPreMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function dataase
  */
{
  ELog::RegMethod RegA("HPreMod","populate");

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
HPreMod::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("HPreMod","createSurface");

  if (!isActive("divider"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("divider",SMap.realSurf(buildIndex+1));
    }
  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin+Z*height,Z);

  // Inner surfaces:
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(depth-alThickness),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(width/2.0-alThickness),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(width/2.0-alThickness),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin+Z*(height-alThickness),Z);

  if (isActive("target"))
    makeExpandedSurf(SMap,"target",buildIndex+17,Origin,alThickness);
  makeExpandedSurf(SMap,"base",buildIndex+16,Origin,-alThickness);
  makeExpandedSurf(SMap,"divider",buildIndex+11,Origin,alThickness);

  return;
}

void
HPreMod::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HPreMod","createObjects");

  const HeadRule targetHR=getRule("target");  // surf 1020101 (c/y)
  const HeadRule divHR=getRule("divider");  // surf -1040001
  const HeadRule baseHR=getRule("base");  // surf 1060046
    
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -4 -5");
  addOuterSurf(HR*targetHR*baseHR*divHR);


  HR*=ModelSupport::getHeadRule(SMap,buildIndex,
				    "(11:12:-13:14:15:-16:-17)");
  System.addCell(cellIndex++,alMat,modTemp,HR*targetHR*baseHR*divHR);

  // inner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -12 13 -14 -15 16 17");
  System.addCell(cellIndex++,modMat,modTemp,HR);
  return;
}

void
HPreMod::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("HPreMod","createLinks");
  const HeadRule baseHR=getComplementRule("base");  // surf 1060046
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+1));
  
  FixedComp::setConnect(1,Origin+Y*depth,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
        
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,baseHR);
  
  FixedComp::setConnect(5,Origin-Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));

  nameSideIndex(4,"minusZ");
  nameSideIndex(5,"plusZ");
    
  return;
}
  
void
HPreMod::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int baseIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param baseIndex :: base number
  */
{
  ELog::RegMethod RegA("HPreMod","createAll");

  populate(System.getDataBase());  
  FixedComp::createUnitVector(FC,baseIndex,0);
  FixedRotate::applyOffset();

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
