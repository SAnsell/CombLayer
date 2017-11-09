/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/FaradayCup.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "FaradayCup.h"

namespace essSystem
{

FaradayCup::FaradayCup(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FaradayCup::FaradayCup(const FaradayCup& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  active(A.active),
  engActive(A.engActive),
  length(A.length),outerRadius(A.outerRadius),innerRadius(A.innerRadius),
  faceLength(A.faceLength),
  faceRadius(A.faceRadius),
  absLength(A.absLength),
  absMat(A.absMat),
  baseLength(A.baseLength),
  colLength(A.colLength),
  colMat(A.colMat),wallMat(A.wallMat),
  airMat(A.airMat),
  shieldRadius(A.shieldRadius),
  shieldInnerRadius(A.shieldInnerRadius),
  shieldLength(A.shieldLength),
  shieldInnerLength(A.shieldInnerLength),
  shieldMat(A.shieldMat)
  /*!
    Copy constructor
    \param A :: FaradayCup to copy
  */
{}

FaradayCup&
FaradayCup::operator=(const FaradayCup& A)
  /*!
    Assignment operator
    \param A :: FaradayCup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      active=A.active;
      engActive=A.engActive;
      length=A.length;
      outerRadius=A.outerRadius;
      innerRadius=A.innerRadius;
      faceLength=A.faceLength;
      faceRadius=A.faceRadius;
      absLength=A.absLength;
      absMat=A.absMat;
      baseLength=A.baseLength;
      colLength=A.colLength;
      colMat=A.colMat;
      wallMat=A.wallMat;
      airMat=A.airMat;
      shieldRadius=A.shieldRadius;
      shieldInnerRadius=A.shieldInnerRadius;
      shieldLength=A.shieldLength;
      shieldInnerLength=A.shieldInnerLength;
      shieldMat=A.shieldMat;
    }
  return *this;
}

FaradayCup*
FaradayCup::clone() const
  /*!
    Clone self
    \return new (this)
  */
{
  return new FaradayCup(*this);
}

FaradayCup::~FaradayCup()
  /*!
    Destructor
  */
{}

void
FaradayCup::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FaradayCup","populate");

  FixedOffset::populate(Control);
  active=Control.EvalDefVar<int>(keyName+"Active", 1);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  faceLength=Control.EvalVar<double>(keyName+"FaceLength");
  faceRadius=Control.EvalVar<double>(keyName+"FaceRadius");
  absLength=Control.EvalVar<double>(keyName+"AbsorberLength");
  absMat=ModelSupport::EvalMat<int>(Control,keyName+"AbsorberMat");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  colLength=Control.EvalVar<double>(keyName+"CollectorLength");
  colMat=ModelSupport::EvalMat<int>(Control,keyName+"CollectorMat");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");

  shieldRadius=Control.EvalVar<double>(keyName+"ShieldRadius");
  shieldInnerRadius=Control.EvalVar<double>(keyName+"ShieldInnerRadius");
  shieldLength=Control.EvalVar<double>(keyName+"ShieldLength");
  shieldInnerLength=Control.EvalVar<double>(keyName+"ShieldInnerLength");
  shieldMat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat");

  return;
}

void
FaradayCup::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
FaradayCup::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FaradayCup","createSurfaces");

  double dy(0.0);
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin+Y*dy,Y);
  dy += faceLength;
  ModelSupport::buildPlane(SMap,surfIndex+11,Origin+Y*dy,Y);
  dy += absLength;
  ModelSupport::buildPlane(SMap,surfIndex+21,Origin+Y*dy,Y);
  dy += baseLength;
  ModelSupport::buildPlane(SMap,surfIndex+31,Origin+Y*dy,Y);
  dy += colLength;
  ModelSupport::buildPlane(SMap,surfIndex+41,Origin+Y*dy,Y);

  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildCylinder(SMap,surfIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+17,Origin,Y,outerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+27,Origin,Y,faceRadius);

  ModelSupport::buildPlane(SMap,surfIndex+102,Origin+Y*(shieldInnerLength),Y);
  ModelSupport::buildPlane(SMap,surfIndex+112,Origin+Y*(shieldLength),Y);
  ModelSupport::buildCylinder(SMap,surfIndex+107,Origin,Y,shieldInnerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+117,Origin,Y,shieldRadius);

  return;
}

void
FaradayCup::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FaradayCup","createObjects");

  if (active)
    {

      std::string Out;
      // collimator
      Out=ModelSupport::getComposite(SMap,surfIndex," 1 -11 -27 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,surfIndex," 1 -11 27 -17 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      
      // absorber
      Out=ModelSupport::getComposite(SMap,surfIndex," 11 -21 -17 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));
      
      // base
      Out=ModelSupport::getComposite(SMap,surfIndex," 21 -31 -7 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,surfIndex," 21 -41 7 -17 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      
      // collector
      Out=ModelSupport::getComposite(SMap,surfIndex," 31 -41 -7 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,colMat,0.0,Out));
      
      // back plane
      Out=ModelSupport::getComposite(SMap,surfIndex," 41 -2 -17 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      
      // shielding
      Out=ModelSupport::getComposite(SMap,surfIndex," 1 -102 -107 (-1:2:17) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,surfIndex," 1 -112 -117 (-1:102:107) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,surfIndex," 1 -112 -117 ");
      addOuterSurf(Out);
    }
  return;
}


void
FaradayCup::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("FaradayCup","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  return;
}




void
FaradayCup::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
