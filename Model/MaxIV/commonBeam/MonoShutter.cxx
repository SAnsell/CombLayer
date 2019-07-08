/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoShutter.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "Quadratic.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "ShutterUnit.h"
#include "MonoShutter.h"

namespace xraySystem
{

MonoShutter::MonoShutter(const std::string& Key) :
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB",
			       "ShutterA","ShutterB"),
  attachSystem::FixedOffset(Key,3),
  attachSystem::ExternalCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap(),
  
  shutterPipe(new constructSystem::PortTube(keyName+"Pipe")),
  monoShutterA(new xraySystem::ShutterUnit(keyName+"UnitA")),
  monoShutterB(new xraySystem::ShutterUnit(keyName+"UnitB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(shutterPipe);
  OR.addObject(monoShutterA);
  OR.addObject(monoShutterB);
}

MonoShutter::~MonoShutter()
  /*!
    Destructor
   */
{}

void
MonoShutter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoShutter","populate");

  FixedOffset::populate(Control);

  divideBStep=Control.EvalVar<double>(keyName+"DivideBStep");

  divideThick=Control.EvalVar<double>(keyName+"DivideThick");
  divideRadius=Control.EvalVar<double>(keyName+"DivideRadius");
  
  dMat=ModelSupport::EvalMat<int>(Control,keyName+"DivideMat");
  
  return;
}


void
MonoShutter::createSurfaces()
  /*!
    Create planes for mid plane divider
  */
{
  ELog::RegMethod RegA("MonoShutter","createSurfaces");

  const Geometry::Vec3D CPoint(shutterPipe->getLinkPt(0));
  const Geometry::Vec3D beamPoint(shutterPipe->getLinkPt(1));

  ModelSupport::buildPlane(SMap,buildIndex+1,
			   CPoint-Y*(divideThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   CPoint+Y*(divideThick/2.0),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,beamPoint,Y,divideRadius);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   CPoint+Y*(divideBStep-divideThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   CPoint+Y*(divideBStep+divideThick/2.0),Y);
  
  return; 
}

void
MonoShutter::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("MonoShutter","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  makeCell("DivideAVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 7 ");
  makeCell("DivideA",System,cellIndex++,dMat,0.0,Out+
	   shutterPipe->getSurfString("VoidCyl"));

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 -7 ");
  makeCell("DivideBVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 7 ");
  makeCell("DivideB",System,cellIndex++,dMat,0.0,Out+
	   shutterPipe->getSurfString("VoidCyl"));

  // Special cells for replacing splitPipe->getCell("Void")
  Out=ModelSupport::getComposite(SMap,buildIndex," -1  ");
  makeCell("FrontVoid",System,cellIndex++,0,0.0,Out+
	   shutterPipe->getSurfString("VoidFront")+
	   shutterPipe->getSurfString("VoidCyl"));

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -11  ");
  makeCell("MidVoid",System,cellIndex++,0,0.0,Out+
	   shutterPipe->getSurfString("VoidCyl"));

  Out=ModelSupport::getComposite(SMap,buildIndex," 12  ");
  makeCell("BackVoid",System,cellIndex++,0,0.0,Out+
	   shutterPipe->getSurfString("VoidBack")+
	   shutterPipe->getSurfString("VoidCyl"));


  const int CN=shutterPipe->getCell("Void");
  
  System.removeCell(CN);
  monoShutterA->insertInCell("Inner",System,getCell("FrontVoid"));
  monoShutterB->insertInCell("Inner",System,getCell("MidVoid"));
  return; 
}

void
MonoShutter::buildComponents(Simulation& System)
  /*!
    Construct the object to be built
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("MonoShutter","buildComponents");
 
  shutterPipe->addInsertCell("Main",getCC("Main").getInsertCells());

  if (isActive("front"))
    shutterPipe->setFront(this->getRuleStr("front"));  
  shutterPipe->createAll(System,*this,0);

  
  const constructSystem::portItem& PIA=shutterPipe->getPort(0);
  monoShutterA->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterA->addInsertCell("Inner",PIA.getCell("Void"));
  monoShutterA->addInsertCell("Outer",getCC("Main").getInsertCells());
  monoShutterA->createAll(System,*shutterPipe,0,PIA,2);

  const constructSystem::portItem& PIB=shutterPipe->getPort(1);
  monoShutterB->addInsertCell("Inner",shutterPipe->getCell("Void"));
  monoShutterB->addInsertCell("Inner",PIB.getCell("Void"));
  monoShutterB->addInsertCell("Outer",getCC("Main").getInsertCells());
  monoShutterB->createAll(System,*shutterPipe,0,PIB,2);


  ContainedGroup::addOuterSurf("Main",shutterPipe->getCC("Main"));
  ContainedGroup::addOuterUnionSurf("Main",PIA);
  ContainedGroup::addOuterUnionSurf("Main",PIB);
  ContainedGroup::addOuterSurf("FlangeA",shutterPipe->getCC("FlangeA"));
  ContainedGroup::addOuterSurf("FlangeB",shutterPipe->getCC("FlangeB"));
  ContainedGroup::addOuterSurf("ShutterA",monoShutterA->getCC("Outer"));
  ContainedGroup::addOuterSurf("ShutterB",monoShutterB->getCC("Outer"));

  SurfMap::addSurf("PortACut",shutterPipe->getSignedSurf("PortACut"));
  SurfMap::addSurf("PortBCut",shutterPipe->getSignedSurf("PortBCut"));
  
  return;
}

void
MonoShutter::createLinks()
  /*!
    Create a front/back links and centre origin
   */
{
  ELog::RegMethod RControl("MonoShutter","createLinks");
  
  setLinkSignedCopy(0,*shutterPipe,1);
  setLinkSignedCopy(1,*shutterPipe,2);
  setLinkSignedCopy(2,*shutterPipe,11);   // center origin
  
  return;
}

void
MonoShutter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("MonoShutter","createAll");
 
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildComponents(System);

  createSurfaces();
  createObjects(System);
  createLinks();

  return;
}

}  // NAMESPACE xraySystem
