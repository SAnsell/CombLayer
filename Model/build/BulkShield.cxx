/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/BulkShield.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "inputParam.h"
#include "PointOperation.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "chipDataStore.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "InsertComp.h"
#include "LinearComp.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "Torpedo.h"
#include "ChipIRShutterFlat.h"
#include "ChipIRInsert.h"
#include "IMatBulkInsert.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "ZoomShutter.h"
#include "IMatShutter.h"
#include "BlockShutter.h"
#include "BulkShield.h"

namespace shutterSystem
{

const size_t BulkShield::chipShutter(0);
const size_t BulkShield::imatShutter(3);
const size_t BulkShield::zoomShutter(9);
const size_t BulkShield::letShutter(6);

BulkShield::BulkShield(const std::string& Key)  : 
  attachSystem::FixedComp(Key,0),attachSystem::ContainedComp(),
  bulkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bulkIndex+1),populated(0),
  numberBeamLines(18)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

BulkShield::BulkShield(const BulkShield& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  bulkIndex(A.bulkIndex),cellIndex(A.cellIndex),
  populated(A.populated),numberBeamLines(A.numberBeamLines),
  TData(A.TData),GData(A.GData),BData(A.BData),vXoffset(A.vXoffset),
  torpedoRadius(A.torpedoRadius),shutterRadius(A.shutterRadius),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  totalHeight(A.totalHeight),totalDepth(A.totalDepth),
  ironMat(A.ironMat),torpedoCell(A.torpedoCell),
  shutterCell(A.shutterCell),innerCell(A.innerCell),
  outerCell(A.outerCell)
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
      cellIndex=A.cellIndex;
      populated=A.populated;
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
      torpedoCell=A.torpedoCell;
      shutterCell=A.shutterCell;
      innerCell=A.innerCell;
      outerCell=A.outerCell;
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
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  
  vXoffset=Control.EvalVar<double>("voidXoffset");
  
  torpedoRadius=Control.EvalVar<double>(keyName+"TorpedoRadius");
  shutterRadius=Control.EvalVar<double>(keyName+"ShutterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");  
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  totalHeight=Control.EvalVar<double>(keyName+"Roof");
  totalDepth=Control.EvalVar<double>(keyName+"Floor");
  
  ironMat=ModelSupport::EvalMat<int>(Control,keyName+"IronMat");

  CS.setVNum(chipIRDatum::torpedoRing,torpedoRadius);

  populated = 1;
  return;
}

void
BulkShield::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComp
  */
{
  ELog::RegMethod RegA("BulkShield","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
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
  ModelSupport::buildPlane(SMap,bulkIndex+5,Origin-Z*totalDepth,Z);
  ModelSupport::buildPlane(SMap,bulkIndex+6,Origin+Z*totalHeight,Z);

  // Layers:
  ModelSupport::buildCylinder(SMap,bulkIndex+7,
			      Origin,Z,torpedoRadius);
  ModelSupport::buildCylinder(SMap,bulkIndex+17,
			      Origin,Z,shutterRadius);
  ModelSupport::buildCylinder(SMap,bulkIndex+27,
			      Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,bulkIndex+37,
			      Origin,Z,outerRadius);
  
  return;
}

void 
BulkShield::createTorpedoes(Simulation& System,
			    const attachSystem::ContainedComp& CC)
  /*!
    Create the torpedo tubes: Must be called after createShutter
    \param System :: Simulation object to add data
    \param CC :: Void vessel containment
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

  MonteCarlo::Qhull* torpedoObj=System.findQhull(torpedoCell);
  if (!torpedoObj)
    throw ColErr::InContainerError<int>(torpedoCell,"torpedoObject");

  for(size_t i=0;i<static_cast<size_t>(numberBeamLines);i++)
    {
      TData[i]->setExternal(SMap.realSurf(bulkIndex+7));
      TData[i]->createAll(System,*GData[i],CC);          
      torpedoObj->addSurfString(TData[i]->getExclude());

      // Now Torpedos might intersect with there neighbours
      if (i) TData[i]->addCrossingIntersect(System,*TData[i-1]);

    }  

  return;
}

void
BulkShield::createShutters(Simulation& System,		      
			   const mainSystem::inputParam& IParam)
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

  const bool chipFlag(!IParam.flag("exclude") || 
		      !IParam.compNoCaseValue("E",std::string("chipir")));
  const bool imatFlag(!IParam.flag("exclude") || 
   		      !IParam.compNoCaseValue("E",std::string("imat")));

  const bool zoomFlag(!IParam.flag("exclude") || 
   		      !IParam.compNoCaseValue("E",std::string("zoom")));
  const bool letFlag(!IParam.flag("exclude") || 
   		      !IParam.compNoCaseValue("E",std::string("LET")));

  GData.clear();
  for(size_t i=0;i<numberBeamLines;i++)
    {
      if (i==chipShutter && chipFlag)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new ChipIRShutterFlat(i+1,"shutter","chipShutter")));
      else if (i==imatShutter && imatFlag)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new IMatShutter(i+1,"shutter","imatShutter")));
      else if (i==zoomShutter && zoomFlag)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new ZoomShutter(i+1,"shutter","zoomShutter")));
      else if (i==letShutter && letFlag)
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new BlockShutter(i+1,"shutter","letShutter")));
      else
	GData.push_back(std::shared_ptr<GeneralShutter>
			(new GeneralShutter(i+1,"shutter")));
      OR.addObject(GData.back());
      //      OR.addObject(StrFunc::makeString(std::string("shutter"),i),
      //      GData.back());
    }

  MonteCarlo::Qhull* shutterObj=System.findQhull(shutterCell);
  if (!shutterObj)
    throw ColErr::InContainerError<int>(shutterCell,"shutterCell");

  for(size_t i=0;i<static_cast<size_t>(numberBeamLines);i++)
    {
      GData[i]->setExternal(SMap.realSurf(bulkIndex+7),
			    SMap.realSurf(bulkIndex+17),
			    SMap.realSurf(bulkIndex+6),
			    SMap.realSurf(bulkIndex+5));
      GData[i]->setDivide(40000);
      GData[i]->createAll(System,0.0,0);
      shutterObj->addSurfString(GData[i]->getExclude());
    }

  return;
}

void
BulkShield::createBulkInserts(Simulation& System,
			      const mainSystem::inputParam& IParam)
/*!
    Construct and build all the bulk insert
    \param System :: Simulation to use
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("BulkShield","createBulkInserts");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const bool chipFlag(!IParam.flag("exclude") || 
		      !IParam.compValue("E",std::string("chipIR")));
  const bool imatFlag(!IParam.flag("exclude") || 
   		      (!IParam.compValue("E",std::string("imat")) &&
		       !IParam.compValue("E",std::string("IMat"))) );

  for(size_t i=0;i<numberBeamLines;i++)
    {
      std::shared_ptr<BulkInsert> BItem;
      if (i==chipShutter && chipFlag)
	{
	  BItem=std::shared_ptr<BulkInsert>
	    (new ChipIRInsert(i,"bulkInsert","chipInsert"));
	}
      else if (i==imatShutter && imatFlag)
	BItem=std::shared_ptr<BulkInsert>
	  (new IMatBulkInsert(i,"bulkInsert","imatInsert"));
      else
	BItem=std::shared_ptr<BulkInsert>(new BulkInsert(i,"bulkInsert"));


      BItem->setLayers(innerCell,outerCell);
      BItem->setExternal(SMap.realSurf(bulkIndex+17),
			 SMap.realSurf(bulkIndex+27),
			 SMap.realSurf(bulkIndex+37) );
      BItem->createAll(System,*GData[static_cast<size_t>(i)]);    
      OR.addObject(BItem->getKeyName(),BItem);
      BData.push_back(BItem);
    }
  return;
}

void
BulkShield::createObjects(Simulation& System,
			  const attachSystem::ContainedComp& CC)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param CC :: Excluded object to inner layers [Void vessel]
   */
{
  ELog::RegMethod RegA("BulkShield","createObjects");
  
  // Torpedo
  std::string Out;
  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -7")+CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  torpedoCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -17 7")+CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  shutterCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -27 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  innerCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -37 27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ironMat,0.0,Out));
  outerCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -6 -37");
  addOuterSurf(Out);

  return;
}

void
BulkShield::processVoid(Simulation& System)
  /*!
    Adds this as the outside layer of the void
    \param System :: Simulation to obtain/add void
  */
{
  ELog::RegMethod RegA("BulkShield","processVoid");
  // Add void
  MonteCarlo::Qhull* Obj=System.findQhull(74123);
  if (Obj)
    Obj->procString("-1 "+getExclude());
  else
    System.addCell(MonteCarlo::Qhull(74123,0,0.0,"-1 "+getExclude()));
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
		      const mainSystem::inputParam& IParam,
		      const attachSystem::FixedComp& FC,
		      const attachSystem::ContainedComp& CC)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param IParam :: Input Parameters
    \param CC :: Void Vessel containment
   */
{
  ELog::RegMethod RegA("BulkShield","createAll");
  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System,CC);
  processVoid(System);
  createShutters(System,IParam);
  createBulkInserts(System,IParam);
  createTorpedoes(System,CC);
  return;
}

  
}  // NAMESPACE shutterSystem
