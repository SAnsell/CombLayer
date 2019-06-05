/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxs/cosaxsTube.cxx
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
#include "InnerZone.h"
#include "FrontBackCut.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "GateValveCube.h"
#include "GateValveCylinder.h"
#include "cosaxsTubeNoseCone.h"
#include "cosaxsTubeStartPlate.h"

#include "ContainedGroup.h"
#include "portItem.h"
#include "PipeTube.h"

#include "cosaxsTube.h"

namespace xraySystem
{

cosaxsTube::cosaxsTube(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  delayPortFlag(0),
  buildZone(*this,cellIndex),
  noseCone(new xraySystem::cosaxsTubeNoseCone(keyName+"NoseCone")),
  gateA(new constructSystem::GateValveCylinder(keyName+"GateA")),
  startPlate(new xraySystem::cosaxsTubeStartPlate(keyName+"StartPlate"))
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(noseCone);
  OR.addObject(gateA);
  OR.addObject(startPlate);

  for(size_t i=0;i<8;i++)
    {
      seg[i] = std::make_shared<constructSystem::PipeTube>
	(keyName+"Segment"+std::to_string(i+1));
      OR.addObject(seg[i]);
    }
}

cosaxsTube::cosaxsTube(const cosaxsTube& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  outerRadius(A.outerRadius),
  outerLength(A.outerLength),
  buildZone(A.buildZone),
  noseCone(A.noseCone),
  gateA(A.gateA),
  startPlate(A.startPlate),
  seg(A.seg)
  /*!
    Copy constructor
    \param A :: cosaxsTube to copy
  */
{}

cosaxsTube&
cosaxsTube::operator=(const cosaxsTube& A)
  /*!
    Assignment operator
    \param A :: cosaxsTube to copy
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
      outerRadius=A.outerRadius;
      outerLength=A.outerLength;
      noseCone=A.noseCone;
      gateA=A.gateA;
      startPlate=A.startPlate;
      seg=A.seg;
    }
  return *this;
}

cosaxsTube*
cosaxsTube::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new cosaxsTube(*this);
}

cosaxsTube::~cosaxsTube()
  /*!
    Destructor
  */
{}

void
cosaxsTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("cosaxsTube","populate");

  FixedOffset::populate(Control);

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerLength=Control.EvalVar<double>(keyName+"OuterLength");

  return;
}

void
cosaxsTube::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTube","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
cosaxsTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("cosaxsTube","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(outerLength),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);

  const std::string Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  const HeadRule HR(Out);
  buildZone.setSurround(HR);
  ELog::EM << "use here: -HeadRule(SMap.realSurf(buildIndex+7))" << ELog::endCrit;

  return;
}

void
cosaxsTube::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("cosaxsTube","createObjects");

  std::string Out;
  const std::string frontStr(frontRule());
  const std::string backStr(backRule());

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  addOuterSurf(Out+frontStr+backStr);

  int outerCell;
  buildZone.setFront(getRule("front"));//HeadRule(SMap.realSurf(buildIndex+1)));
  buildZone.setBack(getRule("back"));//HeadRule(-SMap.realSurf(buildIndex+2)));

  MonteCarlo::Object* masterCell=buildZone.constructMasterCell(System,*this);

  noseCone->createAll(System, *this, 0);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*noseCone,-1);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*noseCone,2);
  noseCone->insertInCell(System,outerCell);

  gateA->setFront(*noseCone,2);
  gateA->createAll(System,*noseCone,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*gateA,2);
  gateA->insertInCell(System,outerCell);

  startPlate->setFront(*gateA,2);
  startPlate->createAll(System,*gateA,2);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*startPlate,2);
  startPlate->insertInCell(System,outerCell);

  // tube segments
  attachSystem::FixedComp *last = startPlate.get();

  for (size_t i=0; i<8; i++)
    {
      seg[i]->setFront(*last,2);
      if (delayPortFlag) seg[i]->delayPorts();
      seg[i]->createAll(System,*last,2);

      outerCell=buildZone.createOuterVoidUnit(System,masterCell,*seg[i],2);
      seg[i]->insertAllInCell(System,outerCell);

      last = seg[i].get();
    }


  return;
}

void
cosaxsTube::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("cosaxsTube","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
cosaxsTube::createPorts(Simulation& System)
  /*!
    Generic function to create the ports
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("cosaxsTube","createPorts");
  
  for (size_t i=0; i<8; i++)
    seg[i]->createPorts(System);
  return;
}

void
cosaxsTube::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTube","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
