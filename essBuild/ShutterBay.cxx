/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/ShutterBay.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "World.h"
#include "ShutterBay.h"

namespace essSystem
{

ShutterBay::ShutterBay(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  bulkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bulkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ShutterBay::ShutterBay(const ShutterBay& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  bulkIndex(A.bulkIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),radius(A.radius),
  height(A.height),depth(A.depth),mat(A.mat)
  /*!
    Copy constructor
    \param A :: ShutterBay to copy
  */
{}

ShutterBay&
ShutterBay::operator=(const ShutterBay& A)
  /*!
    Assignment operator
    \param A :: ShutterBay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
    }
  return *this;
}

ShutterBay::~ShutterBay() 
  /*!
    Destructor
  */
{}

void
ShutterBay::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("ShutterBay","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  
void
ShutterBay::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("ShutterBay","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}
  
void
ShutterBay::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ShutterBay","createSurface");

  // rotation of axis:

  // Dividing planes:
  ModelSupport::buildPlane(SMap,bulkIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,bulkIndex+2,Origin,X);

  ModelSupport::buildPlane(SMap,bulkIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,bulkIndex+6,Origin+Z*height,Z);
  ModelSupport::buildCylinder(SMap,bulkIndex+7,Origin,Z,radius);
  
  return;
}

void
ShutterBay::createObjects(Simulation& System,
			  const attachSystem::ContainedComp& CC)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param CC :: Bulk object
  */
{
  ELog::RegMethod RegA("ShutterBay","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -7 ");
  addOuterSurf(Out);
  Out+=CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  

  return;
}

void
ShutterBay::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("ShutterBay","createLinks");

  FixedComp::setConnect(0,Origin-Y*radius,-Y);   // outer point
  FixedComp::setConnect(1,Origin+Y*radius,Y);   // outer point
  FixedComp::setConnect(2,Origin-X*radius,-X);   // outer point
  FixedComp::setConnect(3,Origin+X*radius,X);   // outer point
  FixedComp::setConnect(4,Origin-Z*depth,-Z);  // base
  FixedComp::setConnect(5,Origin+Z*height,Z);  // 

  FixedComp::setLinkSurf(0,SMap.realSurf(bulkIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(bulkIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(bulkIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(bulkIndex+7));

  FixedComp::addBridgeSurf(0,-SMap.realSurf(bulkIndex+1));
  FixedComp::addBridgeSurf(1,SMap.realSurf(bulkIndex+1));
  FixedComp::addBridgeSurf(2,-SMap.realSurf(bulkIndex+2));
  FixedComp::addBridgeSurf(3,SMap.realSurf(bulkIndex+2));
  FixedComp::setLinkSurf(4,-SMap.realSurf(bulkIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(bulkIndex+6));


  return;
}

void
ShutterBay::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const attachSystem::ContainedComp& CC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param CC :: Central origin
  */
{
  ELog::RegMethod RegA("ShutterBay","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System,CC);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
