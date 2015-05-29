/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/TS2flatTarget.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
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
#include "TS2flatTarget.h"

namespace TMRSystem
{

TS2flatTarget::TS2flatTarget(const std::string& Key) :
  attachSystem::FixedComp(Key,0),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TS2flatTarget::TS2flatTarget(const TS2flatTarget& A) : 
  attachSystem::FixedComp(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
  populated(A.populated),yOffset(A.yOffset),mainLength(A.mainLength),
  coreRadius(A.coreRadius),cladRadius(A.cladRadius),waterRadius(A.waterRadius),
  pressureRadius(A.pressureRadius),wMat(A.wMat),taMat(A.taMat),
  waterMat(A.waterMat),targetTemp(A.targetTemp),waterTemp(A.waterTemp),
  externTemp(A.externTemp)
  /*!
    Copy constructor
    \param A :: TS2flatTarget to copy
  */
{}

TS2flatTarget&
TS2flatTarget::operator=(const TS2flatTarget& A)
  /*!
    Assignment operator
    \param A :: TS2flatTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      yOffset=A.yOffset;
      mainLength=A.mainLength;
      coreRadius=A.coreRadius;
      cladRadius=A.cladRadius;
      waterRadius=A.waterRadius;
      pressureRadius=A.pressureRadius;
      wMat=A.wMat;
      taMat=A.taMat;
      waterMat=A.waterMat;
      targetTemp=A.targetTemp;
      waterTemp=A.waterTemp;
      externTemp=A.externTemp;
    }
  return *this;
}

TS2flatTarget::~TS2flatTarget() 
  /*!
    Destructor
  */
{}

void
TS2flatTarget::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  yOffset=Control.EvalVar<double>(keyName+"YOffset");

  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  coreRadius=Control.EvalVar<double>(keyName+"CoreRadius");
  cladRadius=Control.EvalVar<double>(keyName+"CladRadius");
  waterRadius=Control.EvalVar<double>(keyName+"WaterRadius");
  pressureRadius=Control.EvalVar<double>(keyName+"PressureRadius");

  wMat=Control.EvalVar<int>(keyName+"WMat");
  taMat=Control.EvalVar<int>(keyName+"TaMat");
  waterMat=Control.EvalVar<int>(keyName+"WaterMat");

  targetTemp=Control.EvalVar<double>(keyName+"TargetTemp");
  waterTemp=Control.EvalVar<double>(keyName+"WaterTemp");
  externTemp=Control.EvalVar<double>(keyName+"ExternTemp");
  
  populated=1;
  return;
}

void
TS2flatTarget::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("TS2flatTarget","createUnitVector");

  FixedComp::createUnitVector();
  Origin= Y*yOffset;    // Positive y displacement
  return;
}


void
TS2flatTarget::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("TS2flatTarget","createSurface");
  
  // front face [plane]
  ModelSupport::buildPlane(SMap,protonIndex+1,Origin,Y);
  // back face [plane]
  ModelSupport::buildPlane(SMap,protonIndex+2,
			   Origin+Y*mainLength,Y);  
  // Basis cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+7,
			      Origin,Y,coreRadius);  
  // Ta cladding [Inner]
  ModelSupport::buildCylinder(SMap,protonIndex+27,
			      Origin,Y,cladRadius);  
  
  // WATER
  ModelSupport::buildCylinder(SMap,protonIndex+47,
			      Origin,Y,waterRadius);  

  // pressure cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+57,
			      Origin,Y,pressureRadius);  
  
  return;
}

void
TS2flatTarget::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TS2flatTarget","createObjects");

  std::string Out;
  // Main cylinder:
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
  
  // Main Cylinder
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 7 -27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));  

  // -- WATER --
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 27 -47");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  
  // Ta Press:
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 47 -57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  
  // Spacer Void around target:
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 57 -11T");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // back section
  Out=ModelSupport::getComposite(SMap,protonIndex,"2 -11T 186T");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // Front spacer:
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -11T -190T");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
  return;
}
  
void
TS2flatTarget::createAll(Simulation& System)
  /*!
    Generic function to create everything
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("TS2flatTarget","createAll");
  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
