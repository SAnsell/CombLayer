/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   teaSetBuild/Mug.cxx
 *
 * Copyright (c) 2004-2018 by Konstantin Batkov
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
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
#include "support.h"
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "Mug.h"

namespace teaSetSystem
{

Mug::Mug(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  mugIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(mugIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

Mug::Mug(const Mug& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  mugIndex(A.mugIndex),cellIndex(A.cellIndex),
  radius(A.radius),height(A.height),wallThick(A.wallThick),
  handleRadius(A.handleRadius),handleOffset(A.handleOffset),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: Mug to copy
  */
{}

Mug&
Mug::operator=(const Mug& A)
  /*!
    Assignment operator
    \param A :: Mug to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      radius=A.radius;
      height=A.height;
      wallThick=A.wallThick;
      handleRadius=A.handleRadius;
      handleOffset=A.handleOffset;
      wallMat=A.wallMat;
    }
  return *this;
}

Mug::~Mug()
  /*!
    Destructor
  */
{}
  

void
Mug::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("Mug","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  handleRadius=Control.EvalVar<double>(keyName+"HandleRadius");
  handleOffset=Control.EvalVar<double>(keyName+"HandleOffset");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
Mug::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Mug","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  FixedOffset::applyOffset();

  return;
}

void
Mug::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("Mug","createSurfaces");

  // dividers
  ModelSupport::buildPlane(SMap,mugIndex+3,Origin,X);

  // inner surfaces (contact tee)
  ModelSupport::buildPlane(SMap,mugIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,mugIndex+6,Origin+Z*(height/2.0),Z);  

  ModelSupport::buildCylinder(SMap,mugIndex+7,Origin,Z,radius);

  // wall
  ModelSupport::buildPlane(SMap,mugIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildCylinder(SMap,mugIndex+17,Origin,Z,radius+wallThick);

  // handle
  ModelSupport::buildPlane(SMap,mugIndex+101,Origin-Y*(wallThick/2.0),Y);
  ModelSupport::buildPlane(SMap,mugIndex+102,Origin+Y*(wallThick/2.0),Y);

  ModelSupport::buildCylinder(SMap,mugIndex+107,Origin+X*radius,Y,handleRadius);

  return; 
}

void
Mug::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("Mug","createObjects");

  std::string Out;

  // Inner 
  Out=ModelSupport::getComposite(SMap,mugIndex," 5 -6 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Wall
  Out=ModelSupport::getComposite(SMap,mugIndex," 15 -6 -17 (-5:6:7) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Handle
  Out=ModelSupport::getComposite(SMap,mugIndex," 3 17 101 -102 -107 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,mugIndex,
				 " (15 -6 -17) : (3 17 101 -102 -107) ");
  addOuterSurf(Out);

  return; 
}

void
Mug::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("Mug","createLinks");

  //  throw ColErr::AbsObjMethod("Not implemented yet");

  return;
}

void
Mug::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("Mug","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE teaSetSystem
