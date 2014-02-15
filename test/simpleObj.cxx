/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/simpleObj.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "simpleObj.h"

namespace testSystem
{

simpleObj::simpleObj(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  simpIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(simpIndex+1),refFlag(0),xyAngle(0.0),zAngle(0.0),
  xSize(15.0),ySize(3.0),zSize(15.0),defMat(3)
  /*!
    Constructor : Unlike typicaly constructors of this type
    the variables are populated. It is anticipated as need to
    change them takes place they are updated later. 
    \param Key :: Name for item in search
  */
{}

simpleObj::simpleObj(const simpleObj& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  simpIndex(A.simpIndex),cellIndex(A.cellIndex),refFlag(A.refFlag),
  offset(A.offset),xyAngle(A.xyAngle),zAngle(A.zAngle),xSize(A.xSize),
  ySize(A.ySize),zSize(A.zSize),defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: simpleObj to copy
  */
{}

simpleObj&
simpleObj::operator=(const simpleObj& A)
  /*!
    Assignment operator
    \param A :: simpleObj to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      refFlag=A.refFlag;
      offset=A.offset;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      xSize=A.xSize;
      ySize=A.ySize;
      zSize=A.zSize;
      defMat=A.defMat;
    }
  return *this;
}

simpleObj::~simpleObj() 
 /*!
   Destructor
 */
{}
  
void
simpleObj::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Origner
  */
{
  ELog::RegMethod RegA("simpleObj","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  Origin+=offset;
  applyAngleRotate(xyAngle,0);
  return;
}

void
simpleObj::createUnitVector(const attachSystem::FixedComp& FC,
			      const double XA,const double YA,
			      const double ZA)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Origner
  */
{
  ELog::RegMethod RegA("simpleObj","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  Origin+=offset;
  applyAngleRotate(XA,YA,ZA);
  return;
}

void
simpleObj::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("simpleObj","createSurface");

  if (refFlag)
    {
      // Outer layer:
      ModelSupport::buildPlane(SMap,simpIndex+1,Origin-Y*ySize/2.0,-Y);
      ModelSupport::buildPlane(SMap,simpIndex+2,Origin+Y*ySize/2.0,-Y);
      ModelSupport::buildPlane(SMap,simpIndex+3,Origin-X*xSize/2.0,-X);
      ModelSupport::buildPlane(SMap,simpIndex+4,Origin+X*xSize/2.0,-X);
      ModelSupport::buildPlane(SMap,simpIndex+5,Origin-Z*zSize/2.0,-Z);
      ModelSupport::buildPlane(SMap,simpIndex+6,Origin+Z*zSize/2.0,-Z);
    }
  else
    {
  // Outer layer:
      ModelSupport::buildPlane(SMap,simpIndex+1,Origin-Y*ySize/2.0,Y);
      ModelSupport::buildPlane(SMap,simpIndex+2,Origin+Y*ySize/2.0,Y);
      ModelSupport::buildPlane(SMap,simpIndex+3,Origin-X*xSize/2.0,X);
      ModelSupport::buildPlane(SMap,simpIndex+4,Origin+X*xSize/2.0,X);
      ModelSupport::buildPlane(SMap,simpIndex+5,Origin-Z*zSize/2.0,Z);
      ModelSupport::buildPlane(SMap,simpIndex+6,Origin+Z*zSize/2.0,Z);
    }

  return;
}

void
simpleObj::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("simpleObj","createObjects");
  
  std::string Out;
  if (refFlag)
    Out=ModelSupport::getComposite(SMap,simpIndex,"-1 2 -3 4 -5 6");
  else
    Out=ModelSupport::getComposite(SMap,simpIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  mainCell=cellIndex-1;
  return;
}

void
simpleObj::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("simpleObj","createLinks");

  FixedComp::setConnect(0,Origin-Y*ySize/2.0,-Y);
  FixedComp::setConnect(1,Origin+Y*ySize/2.0,Y);
  FixedComp::setConnect(2,Origin-X*xSize/2.0,-X);
  FixedComp::setConnect(3,Origin+X*xSize/2.0,X);
  FixedComp::setConnect(4,Origin-Z*zSize/2.0,-Z);
  FixedComp::setConnect(5,Origin+Z*zSize/2.0,Z);

  int surfSign(-1);
  for(int i=0;i<6;i++)
    {
      FixedComp::setLinkSurf(static_cast<size_t>(i),
			     surfSign*SMap.realSurf(simpIndex+1+i));
      surfSign*=-1;
    }

  return;
}

std::string
simpleObj::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,simpIndex,surfList);
}

void
simpleObj::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
  */
{
  ELog::RegMethod RegA("simpleObj","createAll");

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

void
simpleObj::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const double XA,const double YA,const double ZA)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
  */
{
  ELog::RegMethod RegA("simpleObj","createAll");
  createUnitVector(FC,XA,YA,ZA);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE testSystem
