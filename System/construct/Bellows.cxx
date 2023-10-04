/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Bellows.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "GeneralPipe.h"
#include "Bellows.h"

namespace constructSystem
{

Bellows::Bellows(const std::string& Key) :
  GeneralPipe(Key)
  /*!
    Constructor
    \param Key :: KeyName
  */
{}
  
Bellows::Bellows(const Bellows& A) : 
  GeneralPipe(A),
  bellowThick(A.bellowThick),
  bellowStep(A.bellowStep),
  bellowMat(A.bellowMat)
  /*!
    Copy constructor
    \param A :: Bellows to copy
  */
{}

Bellows&
Bellows::operator=(const Bellows& A)
  /*!
    Assignment operator
    \param A :: Bellows to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralPipe::operator=(A);
      bellowThick=A.bellowThick;
      bellowStep=A.bellowStep;
      bellowMat=A.bellowMat;
    }
  return *this;
}

Bellows::~Bellows() 
  /*!
    Destructor
  */
{}

void
Bellows::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Bellows","populate");

  bellowThick=Control.EvalVar<double>(keyName+"BellowThick");
  bellowStep=Control.EvalDefVar<double>(keyName+"BellowStep",0.0);

  bellowMat=ModelSupport::EvalDefMat(Control,keyName+"BellowMat",feMat);
  outerVoid=1;  // no options:
  return;
}

void
Bellows::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Bellows","createSurfaces");


  GeneralPipe::createSurfaces();
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,
			      Y,radius+feThick+bellowThick);
  
  
  getShiftedFront(SMap,buildIndex+121,Y,(flangeALength+bellowStep));
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  FrontBackCut::getShiftedBack(SMap,buildIndex+12,Y,-flangeBLength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+22,Y,
			       -(flangeBLength+bellowStep));

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,radius+feThick+bellowThick);

  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);
  
  return;
}
  
void
Bellows::createObjects(Simulation& System)
  /*!
    Adds the vacuum pipe and surround
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Bellows","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  // FLANGE Front:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 -107 7");
  makeCell("FrontFlange",System,cellIndex++,feMat,0.0,HR*frontHR);

  // FLANGE Back:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -207 7");
  makeCell("BackFlange",System,cellIndex++,feMat,0.0,HR*backHR);

  // Inner clip if present
  if (bellowStep>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -17 7");
      makeCell("FrontClip",System,cellIndex++,feMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 22 -17 7");
      makeCell("BackClip",System,cellIndex++,feMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 -27 7");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -27 17");
      makeCell("FrontSpaceVoid",System,cellIndex++,0,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 22 -27 17");
      makeCell("BackSpaceVoid",System,cellIndex++,0,0.0,HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -22 -27 7");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,HR);
    }

  HR=HeadRule(SMap,buildIndex,27);
  GeneralPipe::createOuterVoid(System,HR);
  // we can simplify outer void if the flanges have the same radii
  if (std::abs(flangeARadius-flangeBRadius)<Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 27 -107");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);

      HR=HeadRule(SMap,buildIndex,-107);
      addOuterSurf("Main",HR*frontHR*backHR);
    }
  else if (flangeARadius>flangeBRadius)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 -12 27 -107");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"12 207 -107");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR*backHR);
      HR=HeadRule(SMap,buildIndex,-107);
      addOuterSurf("Main",HR*frontHR*backHR);
    }
  else 
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 -12 27 -207");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 107 -207");
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR*frontHR);
      HR=HeadRule(SMap,buildIndex,-207);
      addOuterSurf("Main",HR*frontHR*backHR);
    }
  return;
}
  
void
Bellows::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Bellows","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
