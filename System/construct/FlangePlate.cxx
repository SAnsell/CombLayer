/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/FlangePlate.cxx
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
#include "Quaternion.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "surfDivide.h"

#include "FlangePlate.h"

namespace constructSystem
{

FlangePlate::FlangePlate(const std::string& Key) : 
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  plateIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(plateIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

FlangePlate::FlangePlate(const FlangePlate& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  plateIndex(A.plateIndex),cellIndex(A.cellIndex),
  radius(A.radius),thick(A.thick),innerRadius(A.innerRadius),
  plateMat(A.plateMat),innerMat(A.innerMat)
  /*!
    Copy constructor
    \param A :: FlangePlate to copy
  */
{}

FlangePlate&
FlangePlate::operator=(const FlangePlate& A)
  /*!
    Assignment operator
    \param A :: FlangePlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      radius=A.radius;
      thick=A.thick;
      innerRadius=A.innerRadius;
      plateMat=A.plateMat;
      innerMat=A.innerMat;
    }
  return *this;
}

FlangePlate::~FlangePlate() 
  /*!
    Destructor
  */
{}

void
FlangePlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FlangePlate","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  thick=Control.EvalVar<double>(keyName+"Thick");

  innerRadius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);


  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  innerMat=ModelSupport::EvalDefMat<int>(Control,keyName+"InnerMat",0);

  return;
}

void
FlangePlate::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("FlangePlate","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  Origin-=Y*thick;
  applyOffset();

  return;
}

void
FlangePlate::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("FlangePlate","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,plateIndex+1,Origin-Y*(thick/2.0),Y);
      setFront(SMap.realSurf(1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,plateIndex+2,Origin+Y*(thick/2.0),Y);
      setBack(-SMap.realSurf(2));
    }
  ModelSupport::buildCylinder(SMap,plateIndex+7,Origin,Y,radius);

  if (innerRadius>Geometry::zeroTol &&
      innerRadius<radius-Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,plateIndex+107,Origin,Y,innerRadius);
  
  return;
}

void
FlangePlate::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("FlangePlate","createObjects");

  std::string Out;
  
  const std::string frontStr=frontRule();
  const std::string backStr=backRule();

  if (innerRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,plateIndex," 107 -7 ");
      makeCell("Main",System,cellIndex++,plateMat,0.0,Out+frontStr+backStr);

      Out=ModelSupport::getSetComposite(SMap,plateIndex," -107 ");
      makeCell("Inner",System,cellIndex++,innerMat,0.0,Out+frontStr+backStr);
    }
  else
    {
      Out=ModelSupport::getSetComposite(SMap,plateIndex," -7 ");
      makeCell("Main",System,cellIndex++,plateMat,0.0,Out+frontStr+backStr);
    }

  Out=ModelSupport::getSetComposite(SMap,plateIndex," -7 ");
  addOuterSurf(Out+backStr+frontStr);

  return;
}

  
void
FlangePlate::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FlangePlate","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  return;
}
  
  
void
FlangePlate::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FlangePlate","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
