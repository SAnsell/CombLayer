/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/TWCavity.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "TWCavity.h"

namespace tdcSystem
{

TWCavity::TWCavity(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TWCavity::TWCavity(const TWCavity& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  nCells(A.nCells),cellLength(A.cellLength),cellRadius(A.cellRadius),
  irisLength(A.irisLength),irisRadius(A.irisRadius),
  couplerLength(A.couplerLength),
  couplerWidth(A.couplerWidth),
  wallThick(A.wallThick),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: TWCavity to copy
  */
{}

TWCavity&
TWCavity::operator=(const TWCavity& A)
  /*!
    Assignment operator
    \param A :: TWCavity to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      nCells=A.nCells;
      cellLength=A.cellLength;
      cellRadius=A.cellRadius;
      irisLength=A.irisLength;
      irisRadius=A.irisRadius;
      couplerLength=A.couplerLength;
      couplerWidth=A.couplerWidth;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
    }
  return *this;
}

TWCavity*
TWCavity::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TWCavity(*this);
}

TWCavity::~TWCavity()
  /*!
    Destructor
  */
{}

void
TWCavity::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TWCavity","populate");

  FixedRotate::populate(Control);

  nCells=ModelSupport::EvalMat<int>(Control,keyName+"NCells");
  cellLength=Control.EvalVar<double>(keyName+"CellLength");
  cellRadius=Control.EvalVar<double>(keyName+"CellRadius");
  irisLength=Control.EvalVar<double>(keyName+"IrisLength");
  irisRadius=Control.EvalVar<double>(keyName+"IrisRadius");
  couplerLength=Control.EvalVar<double>(keyName+"CouplerLength");
  couplerWidth=Control.EvalVar<double>(keyName+"CouplerWidth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
TWCavity::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TWCavity","createSurfaces");

  // -irisLengh since last cell does not have iris (its inside back copuler cell)
  const double totalLength(couplerLength*2+(cellLength+irisLength)*nCells-irisLength);

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*totalLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(couplerLength-irisLength-cellLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(totalLength-couplerLength+irisLength+cellLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+21,Origin+Y*(couplerLength-irisLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(totalLength-couplerLength+irisLength),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,irisRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,cellRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,cellRadius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(couplerLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(totalLength-couplerLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(couplerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(couplerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(couplerWidth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(couplerWidth/2.0),Z);
  const double couplerR(couplerWidth/sqrt(2.0));
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,couplerR);

  double y(couplerLength+cellLength);
  int SI(buildIndex+1000);
  for (int i=0; i<nCells-1; ++i)
    {
      ModelSupport::buildPlane(SMap,SI+1,Origin+Y*(y),Y);
      y += irisLength;
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*(y),Y);
      y += cellLength;
      SI += 10;
    }

  return;
}

void
TWCavity::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TWCavity","createObjects");

  int SI(buildIndex+1000);
  std::string Out,Out1;
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // front coupler cell
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -101 103 -104 105 -106 ")+frontStr;
  makeCell("FrontCouplerCell",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 -11 ")+frontStr;
  makeCell("FrontCouplerFrontWall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -17 -11 ")+frontStr;
  makeCell("FrontCouplerFrontWall",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,SI," -17 11 -21 ");
  makeCell("FrontCouplerInnerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,SI," 7 -17 21 -101 ");
  makeCell("FrontCouplerIrisWall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,SI," -7 21 -101 ");
  makeCell("FrontCouplerIrisVoid",System,cellIndex++,0,0.0,Out);

  // front coupler cell corners
  Out1=ModelSupport::getComposite(SMap,buildIndex," -107 -101 ")+frontStr;
  for (const std::string surf : {" -103 ", " 104 ", " -105 ", " 106 "})
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,surf);
      makeCell("FrontCouplerOuterVoid",System,cellIndex++,0,0.0,Out+Out1);
    }

  // back coupler cell
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 102 103 -104 105 -106 ")+backStr;
  makeCell("BackCouplerCell",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 12 ")+backStr;
  makeCell("BackCouplerBackWall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -17 12 ")+backStr;
  makeCell("BackCouplerBackWall",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 22 -12 ");
  makeCell("BackCouplerInnerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 102 -22 ");
  makeCell("BackCouplerIrisWall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 102 -22 ");
  makeCell("BackCouplerIrisVoid",System,cellIndex++,0,0.0,Out);

  // back coupler cell corners
  Out1=ModelSupport::getComposite(SMap,buildIndex," -107 102 ")+backStr;
  for (const std::string surf : {" -103 ", " 104 ", " -105 ", " 106 "})
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,surf);
      makeCell("BackCouplerOuterVoid",System,cellIndex++,0,0.0,Out+Out1);
    }

  for (int i=0; i<nCells; ++i)
    {
      if (i==0)
	Out=ModelSupport::getComposite(SMap,buildIndex,SI," 101 -1M -17 ");
      else if (i==nCells-1)
	Out=ModelSupport::getComposite(SMap,buildIndex,SI-10," 2M -102 -17 ");
      else
	Out=ModelSupport::getComposite(SMap,buildIndex,SI-10,SI," 2M -1N -17 ");

      makeCell("NormalCell",System,cellIndex++,0,0.0,Out);

      if (i!=nCells-1)
	{
	  Out1=ModelSupport::getComposite(SMap,buildIndex,SI," 1M -2M 7 -17 ");
	  makeCell("IrisWall",System,cellIndex++,wallMat,0.0,Out1);
	  Out1=ModelSupport::getComposite(SMap,buildIndex,SI," 1M -2M -7 ");
	  makeCell("IrisVoid",System,cellIndex++,0,0.0,Out1);
	}

      SI += 10;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 17 -27 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 27 -107 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 ")+frontStr+backStr;
  addOuterSurf(Out);

  return;
}


void
TWCavity::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TWCavity","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  return;
}

void
TWCavity::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TWCavity","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
