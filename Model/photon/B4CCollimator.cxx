/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/B4CCollimator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "B4CCollimator.h"

namespace photonSystem
{
      
B4CCollimator::B4CCollimator(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

B4CCollimator::B4CCollimator(const B4CCollimator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  radius(A.radius),length(A.length),viewWidth(A.viewWidth),
  viewHeight(A.viewHeight),outerMat(A.outerMat)
  /*!
    Copy constructor
    \param A :: B4CCollimator to copy
  */
{}

B4CCollimator&
B4CCollimator::operator=(const B4CCollimator& A)
  /*!
    Assignment operator
    \param A :: B4CCollimator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      radius=A.radius;
      length=A.length;
      viewWidth=A.viewWidth;
      viewHeight=A.viewHeight;
      outerMat=A.outerMat;
    }
  return *this;
}


B4CCollimator::~B4CCollimator()
  /*!
    Destructor
  */
{}

B4CCollimator*
B4CCollimator::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new B4CCollimator(*this);
}

void
B4CCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("B4CCollimator","populate");

  FixedOffset::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");

  viewWidth=Control.EvalVar<double>(keyName+"ViewWidth");
  viewHeight=Control.EvalVar<double>(keyName+"ViewHeight");

  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  return;
}

void
B4CCollimator::createUnitVector(const attachSystem::FixedComp& FC,
				const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
    \param sideIndex :: sinde track
  */
{
  ELog::RegMethod RegA("B4CCollimator","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
B4CCollimator::createSurfaces()
  /*!
    Create surface for the collimator
  */
{
  ELog::RegMethod RegA("B4CCollimator","createSurfaces");

  // Outer surfaces:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);


  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(viewWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(viewWidth/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(viewHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(viewHeight/2.0),Z);  

  return; 
}

void
B4CCollimator::createObjects(Simulation& System)
  /*!
    Create the collimator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("B4CCollimator","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 103 -104 105 -106");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2 -7 (-103:104:-105:106)");
  System.addCell(MonteCarlo::Object(cellIndex++,outerMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  addOuterSurf(Out);
  return; 
}

void
B4CCollimator::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("B4CCollimator","createLinks");
  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex-7));

  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex-7));

  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex-7));

  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex-7));

  return;
}

void
B4CCollimator::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("B4CCollimator","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE photonSystem
