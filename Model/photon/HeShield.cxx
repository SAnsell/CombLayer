/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/HeShield.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "HeShield.h"


namespace photonSystem
{
      
HeShield::HeShield(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
}

HeShield::HeShield(const HeShield& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  length(A.length),
  width(A.width),height(A.height),
  frontPolyThick(A.frontPolyThick),collLen(A.collLen),
  collWidth(A.collWidth),collHeight(A.collHeight),
  collThick(A.collThick),cdThick(A.cdThick),
  polyMat(A.polyMat),cdMat(A.cdMat)
  /*!
    Copy constructor
    \param A :: HeShield to copy
  */
{}

HeShield&
HeShield::operator=(const HeShield& A)
  /*!
    Assignment operator
    \param A :: HeShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      frontPolyThick=A.frontPolyThick;
      collLen=A.collLen;
      collWidth=A.collWidth;
      collHeight=A.collHeight;
      collThick=A.collThick;
      cdThick=A.cdThick;
      polyMat=A.polyMat;
      cdMat=A.cdMat;
    }
  return *this;
}


HeShield::~HeShield()
  /*!
    Destructor
  */
{}

HeShield*
HeShield::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new HeShield(*this);
}

void
HeShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("HeShield","populate");

  FixedRotate::populate(Control);
  
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  frontPolyThick=Control.EvalVar<double>(keyName+"FrontPolyThick");

  collLen=Control.EvalVar<double>(keyName+"CollLen");
  collWidth=Control.EvalVar<double>(keyName+"CollWidth");
  collHeight=Control.EvalVar<double>(keyName+"CollHeight");
  collThick=Control.EvalVar<double>(keyName+"CollThick");

  polyMat=ModelSupport::EvalMat<int>(Control,keyName+"PolyMat");
  cdMat=ModelSupport::EvalMat<int>(Control,keyName+"CdMat");


  return;
}

void
HeShield::createSurfaces()
  /*!
    Create simple structures
  */
{
  ELog::RegMethod RegA("HeShield","createSurfaces");

  
  // boundary surfaces

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // Cd collimator
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*collLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(collWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(collWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(collHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(collHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+111,Origin-Y*collThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+113,
			   Origin-X*(collThick+collWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+114,
			   Origin+X*(collThick+collWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+115,
			   Origin-Z*(collThick+collHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   Origin+Z*(collThick+collHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+201,
			   Origin-Y*(collThick+frontPolyThick),Y);
  
  return; 
}

void
HeShield::createObjects(Simulation& System)
  /*!
    Create the tubed moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("HeShield","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex,"1 -2 3 -4 5 -6");
  makeCell("Main",System,cellIndex++,polyMat,0.0,HR);

  // cd void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex,
				"-1 101  103 -104 105 -106");
  System.addCell(cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,buildIndex,"-1 101 113 -114 115 -116 (-103:104:-105:106)");
  System.addCell(cellIndex++,cdMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,buildIndex,"-1 111 3 -4 5 -6 (-113:114:-115:116)");
  System.addCell(cellIndex++,cdMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,buildIndex,"-111 201 3 -4 5 -6 (-113:114:-115:116)");
  System.addCell(cellIndex++,polyMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -2 3 -4 5 -6");
  addOuterSurf(HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,buildIndex,
				"-1 101 113 -114 115 -116");
  addOuterUnionSurf(HR);
  

  return; 
}

void
HeShield::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("HeShield","createLinks");
  
  

  return;
}
  
void
HeShield::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("HeShield","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE photonSystem

