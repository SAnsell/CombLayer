/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   instrument/CylSample.cxx
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
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "CylSample.h"

namespace instrumentSystem
{

CylSample::CylSample(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,3)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylSample::CylSample(const CylSample& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  nLayers(A.nLayers),radius(A.radius),
  height(A.height),mat(A.mat)
  /*!
    Copy constructor
    \param A :: CylSample to copy
  */
{}

CylSample&
CylSample::operator=(const CylSample& A)
  /*!
    Assignment operator
    \param A :: CylSample to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      nLayers=A.nLayers;
      radius=A.radius;
      height=A.height;
      mat=A.mat;
    }
  return *this;
}

CylSample::~CylSample()
  /*!
    Destructor
   */
{}

void
CylSample::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylSample","populate");

  FixedOffset::populate(Control);
    // Master values

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");   
  for(size_t i=0;i<nLayers;i++)
    {
      radius.push_back(Control.EvalVar<double>
		      (StrFunc::makeString(keyName+"Radius",i+1)));   
      height.push_back(Control.EvalVar<double>
		       (StrFunc::makeString(keyName+"Height",i+1)));   
      mat.push_back(ModelSupport::EvalMat<int>
		    (Control,StrFunc::makeString(keyName+"Material",i+1)));   
    }

  return;
}

void
CylSample::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: signed direction to link
  */
{
  ELog::RegMethod RegA("CylSample","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
CylSample::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CylSample","createSurfaces");

  int SI(buildIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Z,radius[i]);  
      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*height[i]/2.0,Z);  
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*height[i]/2.0,Z);  
      SI+=10;
    }

  // Across sides
  return; 
}

void
CylSample::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("CylSample","createObjects");

  std::string Out,OutInner;
  int SI(buildIndex);
  for(size_t i=0;i<nLayers;i++)
    {
      Out=ModelSupport::getComposite(SMap,SI," -7 5 -6 ");
      System.addCell(MonteCarlo::Object(cellIndex++,mat[i],0.0,Out+OutInner));
      OutInner=ModelSupport::getComposite(SMap,SI," (7:-5:6) ");
      SI+=10;
    }
  addOuterSurf(Out);
  return; 
}

void
CylSample::createLinks()
  /*!
    Creates a full attachment set
  */
{
  
  return;
}


void
CylSample::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Side index
   */
{
  ELog::RegMethod RegA("CylSample","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
