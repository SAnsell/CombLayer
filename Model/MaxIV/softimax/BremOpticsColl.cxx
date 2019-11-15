/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/softimax/BremOpticsColl.cxx
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
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
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "BremOpticsColl.h"

namespace xraySystem
{

BremOpticsColl::BremOpticsColl(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BremOpticsColl::BremOpticsColl(const BremOpticsColl& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),extWidth(A.extWidth),extHeight(A.extHeight),
  wallThick(A.wallThick),
  holeXStep(A.holeXStep),
  holeZStep(A.holeZStep),
  holeWidth(A.holeWidth),
  holeHeight(A.holeHeight),
  colYStep(A.colYStep),
  colLength(A.colLength),
  colRadius(A.colRadius),
  extActive(A.extActive),
  extXStep(A.extXStep),
  extZStep(A.extZStep),
  innerRadius(A.innerRadius),
  flangeARadius(A.flangeARadius),
  flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),
  flangeBLength(A.flangeBLength),
  voidMat(A.voidMat),wallMat(A.wallMat),
  colMat(A.colMat)
  /*!
    Copy constructor
    \param A :: BremOpticsColl to copy
  */
{}

BremOpticsColl&
BremOpticsColl::operator=(const BremOpticsColl& A)
  /*!
    Assignment operator
    \param A :: BremOpticsColl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      extWidth=A.extWidth;
      extHeight=A.extHeight;
      wallThick=A.wallThick;
      holeXStep=A.holeXStep;
      holeZStep=A.holeZStep;
      colYStep=A.colYStep;
      holeWidth=A.holeWidth;
      holeHeight=A.holeHeight;
      colLength=A.colLength;
      colRadius=A.colRadius;
      extActive=A.extActive;
      extXStep=A.extXStep;
      extZStep=A.extZStep;
      innerRadius=A.innerRadius;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      colMat=A.colMat;
    }
  return *this;
}

BremOpticsColl*
BremOpticsColl::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BremOpticsColl(*this);
}

BremOpticsColl::~BremOpticsColl()
  /*!
    Destructor
  */
{}

void
BremOpticsColl::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BremOpticsColl","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  extWidth=Control.EvalVar<double>(keyName+"ExtWidth");
  extHeight=Control.EvalVar<double>(keyName+"ExtHeight");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  holeXStep=Control.EvalDefVar<double>(keyName+"HoleXStep",0.0);
  holeZStep=Control.EvalDefVar<double>(keyName+"HoleZStep",0.0);
  holeWidth=Control.EvalVar<double>(keyName+"HoleWidth");
  holeHeight=Control.EvalVar<double>(keyName+"HoleHeight");

  colYStep=Control.EvalDefVar<double>(keyName+"ColYStep",0.0);
  colLength=Control.EvalVar<double>(keyName+"ColLength");

  extActive=Control.EvalDefVar<int>(keyName+"ExtActive", 1);
  extXStep=Control.EvalDefVar<double>(keyName+"ExtXStep", 0.0);
  extZStep=Control.EvalDefVar<double>(keyName+"ExtZStep", 0.0);

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  colRadius=Control.EvalVar<double>(keyName+"ColRadius");

  if (colRadius>innerRadius)
    throw ColErr::RangeError<double>(colRadius, 0, innerRadius,
				   "ColRadius must be <= InnerRadius");

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
                                         keyName+"FlangeRadius");
  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
                                         keyName+"FlangeLength");

  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
                                         keyName+"FlangeRadius");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
                                         keyName+"FlangeLength");



  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  colMat=ModelSupport::EvalMat<int>(Control,keyName+"ColMat");

  return;
}

void
BremOpticsColl::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BremOpticsColl","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
BremOpticsColl::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BremOpticsColl","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+11));

      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
				      SMap.realPtr<Geometry::Plane>(buildIndex+11),
				      flangeALength);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+1,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				      flangeALength);
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+12));

      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
				      SMap.realPtr<Geometry::Plane>(buildIndex+12),
				      -flangeBLength);
    } else
    {
      ModelSupport::buildShiftedPlane(SMap, buildIndex+2,
	      SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				      -flangeBLength);
    }

  //// pipe

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,innerRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,flangeBRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,colRadius);

  // absorber
  /// inner part
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*((length-colLength)/2.0+colYStep),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*((length+colLength)/2.0+colYStep),Y);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(holeWidth/2.0-holeXStep),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(holeWidth/2.0+holeXStep),X);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(holeHeight/2.0-holeZStep),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(holeHeight/2.0+holeZStep),Z);

  // external part
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(extWidth/2.0-extXStep),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(extWidth/2.0+extXStep),X);

  ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(extHeight/2.0-extZStep),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(extHeight/2.0+extZStep),Z);

  return;
}

void
BremOpticsColl::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BremOpticsColl","createObjects");

  std::string Out;
  const std::string front(frontRule());
  const std::string back(backRule());

  Out=ModelSupport::getComposite(SMap,buildIndex," -27 7 -1 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+front);

  Out=ModelSupport::getComposite(SMap,buildIndex," -37 7 2 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+back);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -101 ");
  makeCell("InnerVoidFront",System,cellIndex++,voidMat,0.0,Out+front);

  if (colRadius<innerRadius)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -107 (-103:104:-105:106) ");
      makeCell("Absorber",System,cellIndex++,colMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 107 -7 (-103:104:-105:106) ");
      makeCell("OutsideAbsorber",System,cellIndex++,voidMat,0.0,Out);
    } else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -7 (-103:104:-105:106) ");
      makeCell("Absorber",System,cellIndex++,colMat,0.0,Out);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("Hole",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 102 ");
  makeCell("InnerVoidBack",System,cellIndex++,voidMat,0.0,Out+back);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 7 -17 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);


  // outer boundary
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -27 -1 ");
  addOuterSurf(Out+front);
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -37 2 ");
  addOuterUnionSurf(Out+back);

  if (extActive)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 203 -204 205 -206 17 ");
      makeCell("External",System,cellIndex++,colMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 203 -204 205 -206 ");
      addOuterUnionSurf(Out);
    }

  Out=ModelSupport::getSetComposite(SMap,buildIndex," 1 -2 -17");
  addOuterUnionSurf(Out);

  return;
}


void
BremOpticsColl::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BremOpticsColl","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(extWidth/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(extWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(extHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(extHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
BremOpticsColl::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BremOpticsColl","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
