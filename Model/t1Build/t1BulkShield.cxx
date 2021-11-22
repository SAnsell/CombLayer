/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1BulkShield.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "shutterBlock.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "GeneralShutter.h"
#include "BlockShutter.h"
#include "BulkInsert.h"
#include "t1BulkShield.h"

namespace shutterSystem
{

const size_t t1BulkShield::sandalsShutter(1);  // North 1
const size_t t1BulkShield::prismaShutter(2);  // North 2
const size_t t1BulkShield::surfShutter(3);  // North 3
const size_t t1BulkShield::crispShutter(4);  // North 4
const size_t t1BulkShield::loqShutter(5);  // North 5
const size_t t1BulkShield::irisShutter(6);  // North 6
const size_t t1BulkShield::polarisIIShutter(7);  // North 7
const size_t t1BulkShield::toscaShutter(8);  // North 8
const size_t t1BulkShield::hetShutter(9);  // North 9
const size_t t1BulkShield::mapsShutter(10);  // South 1
const size_t t1BulkShield::vesuvioShutter(11);  // South 2
const size_t t1BulkShield::sxdShutter(12);  // South 3
const size_t t1BulkShield::merlinShutter(13);  // South 4
const size_t t1BulkShield::s5Shutter(14);  // South 5
const size_t t1BulkShield::mariShutter(15);  // South 6
const size_t t1BulkShield::gemShutter(16);  // South 7
const size_t t1BulkShield::hrpdShutter(17);  // South 8
const size_t t1BulkShield::pearlShutter(18);  // South 9

t1BulkShield::t1BulkShield(const std::string& Key)  : 
  attachSystem::FixedComp(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  numberBeamLines(18)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

t1BulkShield::t1BulkShield(const t1BulkShield& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),  attachSystem::ExternalCut(A),
  numberBeamLines(A.numberBeamLines),
  GData(A.GData),BData(A.BData),vYoffset(A.vYoffset),
  voidRadius(A.voidRadius),shutterRadius(A.shutterRadius),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  totalHeight(A.totalHeight),totalDepth(A.totalDepth),
  ironMat(A.ironMat)
  /*!
    Copy constructor
    \param A :: t1BulkShield to copy
  */
{}

t1BulkShield&
t1BulkShield::operator=(const t1BulkShield& A)
  /*!
    Assignment operator
    \param A :: t1BulkShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      GData=A.GData;
      BData=A.BData;
      vYoffset=A.vYoffset;
      voidRadius=A.voidRadius;
      shutterRadius=A.shutterRadius;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      totalHeight=A.totalHeight;
      totalDepth=A.totalDepth;
      ironMat=A.ironMat;
    }
  return *this;
}

t1BulkShield::~t1BulkShield() 
  /*!
    Destructor
  */
{}

void
t1BulkShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Functional database to usex
  */
{
  ELog::RegMethod RegA("t1BulkShield","populate");

  vYoffset=Control.EvalVar<double>("voidYoffset");
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius");
  
  shutterRadius=Control.EvalVar<double>(keyName+"ShutterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  totalHeight=Control.EvalVar<double>(keyName+"Roof");
  totalDepth=Control.EvalVar<double>(keyName+"Floor");
  
  ironMat=ModelSupport::EvalMat<int>(Control,keyName+"IronMat");

  return;
}

void
t1BulkShield::createSurfaces()
  /*!
    Create all the surfaces
    \param FC :: Fixed object of inner surface(s)
  */
{
  ELog::RegMethod RegA("t1BulkShield","createSurface");

  //
  // Top/Base
  //
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*totalDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*totalHeight,Z);

  // Layers:
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin,Z,shutterRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,
			      Origin,Z,outerRadius);
  // INNER LAYER:
  const int SN=ExternalCut::getRule("Inner").getPrimarySurface();
  SMap.addMatch(buildIndex+7,SN);
  return;
}

void
t1BulkShield::createShutters(Simulation& System)
  /*!
    Construct and build all the shutters
    \param System :: Simulation object to add data
  */
{
  ELog::RegMethod RegA("t1BulkShield","createShutters");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  GData.clear();
  for(size_t i=1;i<=numberBeamLines;i++)
    {
      if (i==sandalsShutter)
	GData.push_back(std::make_shared<BlockShutter>
			(i,"shutter","sandalsShutter"));
      else if (i==prismaShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","prismaShutter")));
      else if (i==surfShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","surfShutter")));
      else if (i==crispShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","crispShutter")));
      else if (i==loqShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","loqShutter")));
      else if (i==irisShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","irisShutter")));
      else if (i==polarisIIShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","polarisIIShutter")));
      else if (i==toscaShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","toscaShutter")));
      else if (i==hetShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","hetShutter")));
      else if (i==mapsShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","mapsShutter")));
      else if (i==vesuvioShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","vesuvioShutter")));
      else if (i==sxdShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","sxdShutter")));
      else if (i==merlinShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","merlinShutter")));
//      else if (i==s5Shutter)
//	GData.push_back(std::shared_ptr<GeneralShutter>
//			(new BlockShutter(i,"shutter","s5Shutter")));
      else if (i==mariShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","mariShutter")));
      else if (i==gemShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","gemShutter")));
      else if (i==hrpdShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i,"shutter","hrpdShutter")));
      else if (i==pearlShutter)
	GData.push_back(std::shared_ptr<GeneralShutter>
	(new BlockShutter(i,"shutter","pearlShutter")));		
      else
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new GeneralShutter(i,"shutter")));
      // Not registered under KeyName 
      OR.addObject(GData.back());
    }
    

  for(size_t i=0;i<static_cast<size_t>(numberBeamLines);i++)
    {
      GData[i]->setCutSurf("RInner",-SMap.realSurf(buildIndex+7));
      GData[i]->setCutSurf("ROuter",-SMap.realSurf(buildIndex+17));
      GData[i]->setCutSurf("TopPlane",-SMap.realSurf(buildIndex+6));
      GData[i]->setCutSurf("BasePlane",SMap.realSurf(buildIndex+5));
      GData[i]->createAll(System,*this,0);    
      CellMap::insertComponent(System,"shutterCell",GData[i]->getExclude());
    }
  return;
}

void
t1BulkShield::createBulkInserts(Simulation& System)
  /*!
    Construct and build all the bulk insert
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("t1BulkShield","createBulkInserts");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const int innerCell=CellMap::getCell("innerCell");
  const int outerCell=CellMap::getCell("outerCell");
  for(size_t i=0;i<numberBeamLines;i++)
    {
      std::shared_ptr<BulkInsert> 
	BItem=std::shared_ptr<BulkInsert>(new BulkInsert(i,"bulkInsert"));

      BItem->setLayers(innerCell,outerCell);
      BItem->setCutSurf("Divider",
			GData[i]->getKey("Main").getLinkSurf("Divider"));
      BItem->setCutSurf("RInner",-SMap.realSurf(buildIndex+17));
      BItem->setCutSurf("RMid",-SMap.realSurf(buildIndex+27)); 
      BItem->setCutSurf("ROuter",-SMap.realSurf(buildIndex+37));

      OR.addObject(BItem->getKeyName(),BItem);
      BItem->createAll(System,*GData[i],0);
      
      BData.push_back(BItem);
    }
  return;
}

void
t1BulkShield::createObjects(Simulation& System)
  /*!
    Adds the main bulk components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("t1BulkShield","createObjects");

  const std::string innerComp=ExternalCut::getComplementStr("FullInner");
  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -17 7 ");
  makeCell("shutterCell",System,cellIndex++,ironMat,0.0,Out+innerComp);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -27 17");
  makeCell("innerCell",System,cellIndex++,ironMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -37 27");
  makeCell("outerCell",System,cellIndex++,ironMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -37");
  addOuterSurf(Out);

  return;
}

void
t1BulkShield::processVoid(Simulation& System)
  /*!
    Adds this as the outside layer of the void
    \param System :: Simulation to obtain/add void
  */
{
  ELog::RegMethod RegA("t1BulkShield","processVoid");
  // Add void
  MonteCarlo::Object* Obj=System.findObject(74123);
  if (Obj)
    Obj->procString("-1 "+getExclude());
  else
    System.addCell(MonteCarlo::Object(74123,0,0.0,"-1 "+getExclude()));
  return;
}

const shutterSystem::GeneralShutter* 
t1BulkShield::getShutter(const int Index) const
  /*!
    Get a shutter pointer
    \param Index :: Index of shutter to get
    \return shutter pointer
   */
{
  ELog::RegMethod RegA("t1BulkShield","getShutter");
  if (Index<0 || Index>=static_cast<int>(GData.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(GData.size()),
				  RegA.getBase());

  return GData[static_cast<size_t>(Index)].get();
}

const shutterSystem::BulkInsert* 
t1BulkShield::getInsert(const int Index) const
  /*!
    Get an insert pointer
    \param Index :: Index of insert to get
    \return BulkInsert point 
   */
{
  ELog::RegMethod RegA("t1BulkShield","getInsert");
  if (Index<0 || Index>=static_cast<int>(BData.size()))
    throw ColErr::IndexError<int>(Index,static_cast<int>(BData.size()),
				  RegA.getBase());
  return BData[static_cast<size_t>(Index)].get();
}

void
t1BulkShield::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("t1BulkShield","createLinks");

  FixedComp::setConnect(0,Origin,-X);
  FixedComp::setConnect(1,Origin-Z*totalDepth,-Z);
  FixedComp::setConnect(2,Origin+Z*totalHeight,Z);

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+37));
  FixedComp::setLinkSurf(1,-SMap.realSurf(buildIndex+5));  // base
  FixedComp::addLinkSurf(2,SMap.realSurf(buildIndex+6));

  return;
}

void
t1BulkShield::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Create the main shield
    \param System :: Simulation to process
    \param IParam :: Input Parameters
    \param CVoid :: Void Vessel containment
   */
{
  ELog::RegMethod RegA("t1BulkShield","createAll");

  // const mainSystem::inputParam& IParam,
  // const t1CylVessel& CVoid)

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  createSurfaces();
  createObjects(System); 
  processVoid(System);
  createShutters(System);
  createBulkInserts(System);
  createLinks();

  return;
}


  
}  // NAMESPACE shutterSystem
