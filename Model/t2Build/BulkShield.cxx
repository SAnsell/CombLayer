/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/BulkShield.cxx
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
#include <array>

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
#include "Convex.h"
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
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "Torpedo.h"
#include "BulkShield.h"

namespace shutterSystem
{

const size_t BulkShield::chipShutter(0);
const size_t BulkShield::zoomShutter(9);
const size_t BulkShield::letShutter(6);

BulkShield::BulkShield(const std::string& Key)  : 
  attachSystem::FixedComp(Key,0),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  numberBeamLines(18)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

BulkShield::BulkShield(const BulkShield& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::ExternalCut(A),
  numberBeamLines(A.numberBeamLines),
  TData(A.TData),GData(A.GData),BData(A.BData),vXoffset(A.vXoffset),
  torpedoRadius(A.torpedoRadius),shutterRadius(A.shutterRadius),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  totalHeight(A.totalHeight),totalDepth(A.totalDepth),
  ironMat(A.ironMat)
  /*!
    Copy constructor
    \param A :: BulkShield to copy
  */
{}

BulkShield&
BulkShield::operator=(const BulkShield& A)
  /*!
    Assignment operator
    \param A :: BulkShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      TData=A.TData;
      GData=A.GData;
      BData=A.BData;
      vXoffset=A.vXoffset;
      torpedoRadius=A.torpedoRadius;
      shutterRadius=A.shutterRadius;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      totalHeight=A.totalHeight;
      totalDepth=A.totalDepth;
      ironMat=A.ironMat;
    }
  return *this;
}

BulkShield::~BulkShield() 
  /*!
    Destructor
  */
{}

void
BulkShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BulkShield","populate");
  
  vXoffset=Control.EvalVar<double>("voidXoffset");
  
  torpedoRadius=Control.EvalVar<double>(keyName+"TorpedoRadius");
  shutterRadius=Control.EvalVar<double>(keyName+"ShutterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  totalHeight=Control.EvalVar<double>(keyName+"Roof");
  totalDepth=Control.EvalVar<double>(keyName+"Floor");
  
  ironMat=ModelSupport::EvalMat<int>(Control,keyName+"IronMat");

  return;
}

void
BulkShield::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp
  */
{
  ELog::RegMethod RegA("BulkShield","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  Origin+=Y*vXoffset;
  return;
}

void
BulkShield::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("BulkShield","createSurface");

  //
  // Top/Base
  //
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*totalDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*totalHeight,Z);

  // Layers:
  ModelSupport::buildCylinder(SMap,buildIndex+7,
			      Origin,Z,torpedoRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin,Z,shutterRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,
			      Origin,Z,outerRadius);
  
  return;
}

void 
BulkShield::createTorpedoes(Simulation& System)
  /*!
    Create the torpedo tubes: Must be called after createShutter
    \param System :: Simulation object to add data
  */
{ 
  ELog::RegMethod RegA("BulkShield","createTorpedoes");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  TData.clear();
  for(size_t i=0;i<numberBeamLines;i++)
    {
      // Needs specialization to include with/without topedo
      TData.push_back(std::shared_ptr<Torpedo>
		      (new Torpedo(i,"torpedo")));
      OR.addObject(TData.back());
    }

  const int torpedoCell=getCell("Torpedo");
  MonteCarlo::Object* torpedoObj=System.findObject(torpedoCell);
  if (!torpedoObj)
    throw ColErr::InContainerError<int>(torpedoCell,"torpedoObject");

  for(size_t i=0;i<static_cast<size_t>(numberBeamLines);i++)
    {
      TData[i]->setCutSurf("Outer",SMap.realSurf(buildIndex+7));
      TData[i]->createAll(System,*GData[i],2);          
      torpedoObj->addSurfString(TData[i]->getExclude());

      // Now Torpedos might intersect with there neighbours
      if (i) TData[i]->addCrossingIntersect(System,*TData[i-1]);

    }  

  return;
}

void
BulkShield::createShutters(Simulation& System)
  /*!
    Construct and build all the shutters
    \param System :: Simulation object to add data
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("BulkShield","createShutters");
  
  // This is ugly since it is also declared in makeZoom/makeChipIR

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // const bool chipFlag(excludeSet.find("chipIR")==excludeSet.end());
  // const bool zoomFlag(excludeSet.find("zoom")==excludeSet.end());
  // const bool letFlag(excludeSet.find("LET")==excludeSet.end());

  GData.clear();
  for(size_t i=0;i<numberBeamLines;i++)
    {
      // if (i==chipShutter && chipFlag)
      // 	GData.push_back(std::shared_ptr<GeneralShutter>
      // 			(new ChipIRShutterFlat(i+1,"shutter","chipShutter")));
      // else if (i==zoomShutter && zoomFlag)
      // 	GData.push_back(std::shared_ptr<GeneralShutter>
      // 			(new ZoomShutter(i+1,"shutter","zoomShutter")));
      // else if (i==letShutter && letFlag)
      // 	GData.push_back(std::shared_ptr<GeneralShutter>
      // 			(new BlockShutter(i+1,"shutter","letShutter")));
      //      else
      GData.push_back(std::shared_ptr<GeneralShutter>
		      (new GeneralShutter(i+1,"shutter")));
      OR.addObject(GData.back());
    }

  // const int shutterCell=getCell("Shutter");
  // MonteCarlo::Object* shutterObj=System.findObject(shutterCell);
  // if (!shutterObj)
  //   throw ColErr::InContainerError<int>(shutterCell,"shutterCell");

  for(size_t i=0;i<numberBeamLines;i++)
    {
      GData[i]->setCutSurf("RInner",-SMap.realSurf(buildIndex+7));
      GData[i]->setCutSurf("ROuter",-SMap.realSurf(buildIndex+17));
      GData[i]->setCutSurf("TopPlane",-SMap.realSurf(buildIndex+6));
      GData[i]->setCutSurf("BasePlane",SMap.realSurf(buildIndex+5));

      GData[i]->createAll(System,*this,0);
      CellMap::insertComponent(System,"Shutter",GData[i]->getExclude());
    }

  return;
}

void
BulkShield::createBulkInserts(Simulation& System)
/*!
    Construct and build all the bulk insert
    \param System :: Simulation to use
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("BulkShield","createBulkInserts");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const bool chipFlag(excludeSet.find("chipIR")==excludeSet.end());
  const int innerCell=getCell("Inner");
  const int outerCell=getCell("Outer");
  
  for(size_t i=0;i<numberBeamLines;i++)
    {
      std::shared_ptr<BulkInsert> BItem;
      // if (i==chipShutter && chipFlag)
      // 	{
      // 	  BItem=std::shared_ptr<BulkInsert>
      // 	    (new ChipIRInsert(i,"bulkInsert","chipInsert"));
      // 	}
      // else
      BItem=std::shared_ptr<BulkInsert>(new BulkInsert(i,"bulkInsert"));
	
      BItem->setLayers(innerCell,outerCell);
      BItem->setCutSurf("Divider",
			GData[i]->getKey("Main").getLinkSurf("Divider"));
      BItem->setCutSurf("RInner",-SMap.realSurf(buildIndex+17));
      BItem->setCutSurf("RMid",-SMap.realSurf(buildIndex+27)); 
      BItem->setCutSurf("ROuter",-SMap.realSurf(buildIndex+37));


      OR.addObject(BItem->getKeyName(),BItem);
      BItem->createAll(System,*GData[static_cast<size_t>(i)],0);
      
      BData.push_back(BItem);
    }
  return;
}

void
BulkShield::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BulkShield","createObjects");

  const std::string bulkInsertStr=ExternalCut::getRuleStr("BulkInsert");
  // Inner volume [for torpedoes etc]
  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -7")+bulkInsertStr;
  makeCell("Torpedo",System,cellIndex++,ironMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -17 7")+bulkInsertStr;
  makeCell("Shutter",System,cellIndex++,ironMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -27 17");
  makeCell("Inner",System,cellIndex++,ironMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -37 27");
  makeCell("Outer",System,cellIndex++,ironMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"5 -6 -37");
  addOuterSurf(Out);

  return;
}



const shutterSystem::Torpedo* 
BulkShield::getTorpedo(const size_t Index) const
  /*!
    Get a torpedo pointer
    \param Index :: Index of shutter/torpedo to get
    \return shutter pointer
   */
{
  ELog::RegMethod RegA("BulkShield","getTorpedo");

  if (Index>=TData.size())
    throw ColErr::IndexError<size_t>(Index,TData.size(),"Index/TData.size()");
  return TData[Index].get();
}

const shutterSystem::GeneralShutter* 
BulkShield::getShutter(const size_t Index) const
  /*!
    Get a shutter pointer
    \param Index :: Index of shutter to get
    \return shutter pointer
   */
{
  ELog::RegMethod RegA("BulkShield","getShutter");
  if (Index>=GData.size())
    throw ColErr::IndexError<size_t>(Index,GData.size(),"Index/GData.size()");

  return GData[Index].get();
}

const shutterSystem::BulkInsert* 
BulkShield::getInsert(const size_t Index) const
  /*!
    Get an insert pointer
    \param Index :: Index of insert to get
    \return BulkInsert point 
   */
{
  ELog::RegMethod RegA("BulkShield","getInsert");

  if (Index>=BData.size())
    throw ColErr::IndexError<size_t>(Index,BData.size(),"Index/BData.size()");

  return BData[Index].get();
}


int
BulkShield::calcTorpedoPlanes(const int BeamLine,
			      std::vector<int>& Window,
			      int& dSurf) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface. Torpedo plane is teh inn
    \param BeamLine :: BeamLine number to use
    \param Window :: Window of surface to use
    \param dSurf :: divide surface
    \return Master Plane
  */
{
  ELog::RegMethod RegA("BulkShield","calcTorpedoPlanes");

  ELog::EM<<"Calling unfinished code for beamline:"
	  <<BeamLine<<ELog::endErr;
  
  dSurf=0;
  Window.clear();
  return 0;
}

int
BulkShield::calcShutterPlanes(const int BeamLine,
			      std::vector<int>& Window,
			      int& dSurf) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface. Torpedo plane is teh inn
    \param BeamLine :: BeamLine number to use
    \param Window :: Window of surface to use
    \param dSurf :: divide surface
    \return Master Plane
  */
{
  ELog::RegMethod RegA("BulkShield","calcShutterPlanes");

  ELog::EM<<"Calling unfinished code for beamline:"
	  <<BeamLine<<ELog::endErr;
  
  dSurf=0;
  Window.clear();
  return 0;
}

int
BulkShield::calcOuterPlanes(const int BeamLine,
			    std::vector<int>& Window,
			    int& dSurf) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface 
    \param BeamLine :: BeamLine number to use
    \param Window :: Window of surface to use
    \param dSurf :: divide surface
    \return Master Plane
  */
{
  ELog::RegMethod RegA("BulkShield","calcOuterPlanes");

  ELog::EM<<"Calling unfinished code for beamline:"
	  <<BeamLine<<ELog::endErr;
  
  dSurf=0;
  Window.clear();
  
  return 0;
}

void
BulkShield::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: Fiixed
   */
{
  ELog::RegMethod RegA("BulkShield","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  //  processVoid(System);

  createShutters(System);
  createBulkInserts(System);
  //  createTorpedoes(System);
  insertObjects(System);
  return;
}

  
}  // NAMESPACE shutterSystem
