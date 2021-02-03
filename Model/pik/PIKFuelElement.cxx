/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pik/PIKFuelElement.cxx
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#include "SurfMap.h"

#include "PIKFuelElement.h"

namespace pikSystem
{

PIKFuelElement::PIKFuelElement(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PIKFuelElement::PIKFuelElement(const PIKFuelElement& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  length(A.length),radius(A.radius),height(A.height),
  mainMat(A.mainMat)
  /*!
    Copy constructor
    \param A :: PIKFuelElement to copy
  */
{}

PIKFuelElement&
PIKFuelElement::operator=(const PIKFuelElement& A)
  /*!
    Assignment operator
    \param A :: PIKFuelElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      radius=A.radius;
      height=A.height;
      mainMat=A.mainMat;
    }
  return *this;
}

PIKFuelElement*
PIKFuelElement::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PIKFuelElement(*this);
}

PIKFuelElement::~PIKFuelElement()
  /*!
    Destructor
  */
{}

void
PIKFuelElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PIKFuelElement","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}

void
PIKFuelElement::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PIKFuelElement","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);

  ModelSupport::buildCone(SMap,buildIndex+8,Origin+Z*(height/2.0),Z,90-30,-1);


  return;
}

void
PIKFuelElement::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PIKFuelElement","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -8 ");
  makeCell("MainCell",System,cellIndex++,mainMat,0.0,Out);

  addOuterSurf(Out);

  return;
}


void
PIKFuelElement::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PIKFuelElement","createLinks");

  return;
}

void
PIKFuelElement::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PIKFuelElement","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // pikSystem
