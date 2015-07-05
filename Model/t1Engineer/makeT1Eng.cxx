/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/makeT1Eng.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "GroupOrigin.h"
#include "FlightLine.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "Window.h"
#include "t1Vessel.h"
#include "ProtonVoid.h"
#include "BeamWindow.h"
#include "t1CylVessel.h"
#include "t1BulkShield.h"
#include "TargetBase.h"
#include "TS2FlatTarget.h"
#include "TS2ModifyTarget.h"
#include "BulletPlates.h"
#include "BulletVessel.h"
#include "BulletTarget.h"
#include "FishGillVessel.h"
#include "FishGillTarget.h"
#include "CylReflector.h"
#include "EngReflector.h"
#include "ModBase.h"
#include "TriUnit.h"
#include "TriangleMod.h"
#include "H2Section.h"
#include "LayerInfo.h"
#include "CH4Layer.h"
#include "InnerLayer.h"
#include "SplitInner.h"
#include "HPreMod.h"
#include "CH4PreModBase.h"
#include "CH4PreMod.h"
#include "CH4PreFlat.h"
#include "ConicInfo.h"
#include "CylMod.h"
#include "CylPreSimple.h"
#include "MonoPlug.h"
#include "SupplyPipe.h"
#include "World.h"
#include "AttachSupport.h"

#include "makeT1Eng.h"

#include "Debug.h"

namespace ts1System
{

makeT1Eng::makeT1Eng() :
  TarObj(new ts1System::FishGillTarget("FishGillTarget")),
  RefObj(new ts1System::EngReflector("EngRefl")),
  TriMod(new SplitInner("TriModInner","TriModLayer")),
  ColdCentObj(new constructSystem::GroupOrigin("ColdCent")),
  H2Mod(new InnerLayer("H2Inner","H2Layer")),
//  H2PMod(new HPreMod("HPreMod")),
  H2PMod(new CH4PreFlat("HPreMod")),
  CH4Mod(new InnerLayer("CH4ModInner","CH4Mod")),
  CH4PMod(new CH4PreFlat("CH4PreFlat")),
  
  VoidObj(new shutterSystem::t1CylVessel("t1CylVessel")),
  BulkObj(new shutterSystem::t1BulkShield("t1Bulk")),
  MonoTopObj(new shutterSystem::MonoPlug("MonoTop")),
  MonoBaseObj(new shutterSystem::MonoPlug("MonoBase")),
  
  CH4PipeObj(new constructSystem::SupplyPipe("CH4PipeEng")),
  H2PipeObj(new constructSystem::SupplyPipe("H2PipeEng")),
  WaterPipeObj(new constructSystem::SupplyPipe("WaterPipe")),
  WaterReturnObj(new constructSystem::SupplyPipe("WaterReturn")),

  TriFLA(new moderatorSystem::FlightLine("TriFlightA")),
  TriFLB(new moderatorSystem::FlightLine("TriFlightB")),
  H2FL(new moderatorSystem::FlightLine("H2Flight")),
  CH4FLA(new moderatorSystem::FlightLine("CH4FlightA")),
  CH4FLB(new moderatorSystem::FlightLine("CH4FlightB"))
  /*!
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(TarObj);
  OR.addObject(VoidObj);
  OR.addObject(BulkObj);
  OR.addObject(MonoTopObj);
  OR.addObject(MonoBaseObj);

  OR.addObject(RefObj);

  OR.addObject(TriMod);
  OR.addObject(TriFLA);
  OR.addObject(TriFLB);

  OR.addObject(H2Mod);
  OR.addObject(H2PMod);
  OR.addObject(CH4Mod);
  OR.addObject(CH4PMod);
      
  OR.addObject(H2FL);
  OR.addObject(CH4FLA);
  OR.addObject(CH4FLB);

  OR.addObject(CH4PipeObj);
  OR.addObject(H2PipeObj);
  OR.addObject(WaterPipeObj);
  OR.addObject(WaterReturnObj);

}

makeT1Eng::makeT1Eng(const makeT1Eng& A) : 
  TarObj(A.TarObj->clone()),
  BWindowObj(new ts1System::BeamWindow(*A.BWindowObj)),
  RefObj(new EngReflector(*A.RefObj)),
  TriMod(new SplitInner(*A.TriMod)),
  ColdCentObj(new constructSystem::GroupOrigin(*A.ColdCentObj)),
  H2Mod(new InnerLayer(*A.H2Mod)),
  H2PMod(new CH4PreFlat(*A.H2PMod)),
  CH4Mod(new InnerLayer(*A.CH4Mod)),  
  CH4PMod(new CH4PreFlat(*A.CH4PMod)),
  VoidObj(new shutterSystem::t1CylVessel(*A.VoidObj)),
  BulkObj(new shutterSystem::t1BulkShield(*A.BulkObj)),
  MonoTopObj(new shutterSystem::MonoPlug(*A.MonoTopObj)),
  MonoBaseObj(new shutterSystem::MonoPlug(*A.MonoBaseObj)),
  H2PipeObj(new constructSystem::SupplyPipe(*A.H2PipeObj)),
  WaterPipeObj(new constructSystem::SupplyPipe(*A.WaterPipeObj)),
  WaterReturnObj(new constructSystem::SupplyPipe(*A.WaterReturnObj)),
  TriFLA(new moderatorSystem::FlightLine(*A.TriFLA)),
  TriFLB(new moderatorSystem::FlightLine(*A.TriFLB)),
  H2FL(new moderatorSystem::FlightLine(*A.H2FL)),
  CH4FLA(new moderatorSystem::FlightLine(*A.CH4FLA)),
  CH4FLB(new moderatorSystem::FlightLine(*A.CH4FLB))
  /*!
    Copy constructor
    \param A :: makeT1Eng to copy
  */
{}

makeT1Eng&
makeT1Eng::operator=(const makeT1Eng& A)
  /*!
    Assignment operator
    \param A :: makeT1Eng to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *TarObj = *A.TarObj;
      *BWindowObj = *A.BWindowObj;
      *RefObj = *A.RefObj;
      *TriMod = *A.TriMod;
      *ColdCentObj = *A.ColdCentObj;
      *H2Mod = *A.H2Mod; 
      *H2PMod = *A.H2PMod;
      *CH4Mod = *A.CH4Mod;
      *CH4PMod = *A.CH4PMod;
      *VoidObj = *A.VoidObj;
      *BulkObj = *A.BulkObj;
      *MonoTopObj = *A.MonoTopObj;
      *MonoBaseObj = *A.MonoBaseObj;
      *H2PipeObj=*A.H2PipeObj;
      *WaterPipeObj=*A.WaterPipeObj;
      *WaterReturnObj=*A.WaterReturnObj;
      *TriFLA = *A.TriFLA;
      *TriFLB = *A.TriFLB;
      *H2FL = *A.H2FL;
      *CH4FLA = *A.CH4FLA;
      *CH4FLB = *A.CH4FLB;
    }
  return *this;
}


makeT1Eng::~makeT1Eng()
  /*!
    Destructor
   */
{}

void
makeT1Eng::buildH2Pipe(Simulation& System)
  /*!
    Build the H2 Pipe
    \param System :: Simualtion to use
   */
{
  ELog::RegMethod RegA("makeT1Eng","buildH2Pipe");

  H2PipeObj->setAngleSeg(12);
  H2PipeObj->setOption("");   // no modification to the variable name
  
  const std::vector<size_t> LS = {1,2,3,4,5};
  H2PipeObj->setLayerSeq(LS);

  H2PipeObj->createAll(System,*H2Mod,0,5,4);  
  return;
}

void
makeT1Eng::buildCH4Pipe(Simulation& System)
  /*!
    Build the CH4 Pipe
    \param System :: Simualtion to use
   */
{
  ELog::RegMethod RegA("makeT1Eng","buildCH4Pipe");

  CH4PipeObj->setAngleSeg(12);
  CH4PipeObj->setOption("");   // no modification to the variable name
  const std::vector<size_t> LS = {1,2,3,4,5};
  CH4PipeObj->setLayerSeq(LS);
  // poisoning layer check
  for(const int CN : CH4Mod->getInnerCells())
    CH4PipeObj->addInsertCell(1,CN);

  CH4PipeObj->createAll(System,*CH4Mod,1,6,4);  
  return;
}

void
makeT1Eng::buildWaterPipe(Simulation& System)
  /*!
    Build the Water Pipe
    \param System :: Simualtion sytem
   */
{
  ELog::RegMethod RegA("makeT1Eng","buildWaterPipe");

  WaterPipeObj->setAngleSeg(12);
  WaterPipeObj->setOption("");   // no modifiecation to the variable name
  WaterReturnObj->setAngleSeg(12);
  WaterReturnObj->setOption("");   // no modifiecation to the variable name

  WaterPipeObj->setWallOffset(4);
  WaterPipeObj->createAll(System,*TriMod,0,6,5);  
  
  WaterReturnObj->setWallOffset(4);
  WaterReturnObj->createAll(System,*TriMod,0,6,5);
  
  return;
}

  
void 
makeT1Eng::build(Simulation* SimPtr,
		     const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeT1Eng","build");

  int voidCell(74123);
  if (IParam.flag("isolate") &&
      IParam.compValue("I",std::string("Target")))
    {
      TarObj->addInsertCell(voidCell);
      TarObj->createAll(*SimPtr,World::masterOrigin());
      return;
    }


  if (!IParam.flag("exclude") ||
      (!IParam.compValue("E",std::string("Bulk")) && 
       !IParam.compValue("E",std::string("Reflector")) )) 
    {
    //  VoidObj->addInsertCell(74123);  
      VoidObj->createAll(*SimPtr);
      
      BulkObj->addInsertCell(voidCell);  
      BulkObj->createAll(*SimPtr,IParam,*VoidObj);

      MonoTopObj->createAll(*SimPtr,2,*VoidObj,*BulkObj);
      MonoBaseObj->createAll(*SimPtr,1,*VoidObj,*BulkObj);
      voidCell=VoidObj->getVoidCell();
    }
  else
    {
      VoidObj->createStatus(*SimPtr);
    }
  
  RefObj->addInsertCell(voidCell);
  RefObj->createAll(*SimPtr,World::masterOrigin());

  RefObj->addToInsertChain(*TarObj);
  TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
  TarObj->createAll(*SimPtr,World::masterOrigin());
  //  TarObjModify->createAll(*SimPtr,*TarObj);

  // Allows the reflector to be excluded if need be:
  if (IParam.flag("exclude") &&
      IParam.compValue("E",std::string("Reflector")))
    {
      SimPtr->voidObject(RefObj->getKeyName());
      return;
    }
  
  TarObj->addProtonLineInsertCell(RefObj->getCells());
  TarObj->addProtonLine(*SimPtr,*RefObj,-3);

  TriMod->createAll(*SimPtr,World::masterOrigin(),0,0);
  RefObj->addToInsertControl(*SimPtr,*TriMod,*TriMod);

  ColdCentObj->createAll(SimPtr->getDataBase(),World::masterOrigin());

  H2Mod->createAll(*SimPtr,*ColdCentObj,0,0);
  RefObj->addToInsertControl(*SimPtr,*H2Mod,*H2Mod); //
  
  H2PMod->createAll(*SimPtr,*H2Mod,4,0);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*H2PMod,*H2Mod); //
  RefObj->addToInsertControl(*SimPtr,*H2PMod,*H2PMod);
//  RefObj->addToInsertControl(*SimPtr,*H2Mod,*H2Mod);

  CH4Mod->createAll(*SimPtr,*ColdCentObj,0,0);
  RefObj->addToInsertControl(*SimPtr,*CH4Mod,*CH4Mod);

  CH4PMod->createAll(*SimPtr,*CH4Mod,5,0);
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,*CH4Mod);
  RefObj->addToInsertControl(*SimPtr,*CH4PMod,*CH4PMod);

  // Test is moderators intersect
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4Mod,*H2PMod);  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4Mod,*H2Mod);
  
  // FLIGHTLINES:
  const std::string Out=RefObj->getLinkComplement(2);
  TriFLA->addBoundarySurf("inner",Out);  
  TriFLA->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(TriFLA->getCC("outer"));
  TriFLA->createAll(*SimPtr,*TriMod,*TriMod,TriMod->getSideIndex(0));

  TriFLB->addBoundarySurf("inner",Out);  
  TriFLB->addBoundarySurf("outer",Out);  
  TriFLB->createAll(*SimPtr,*TriMod,*TriMod,TriMod->getSideIndex(1));
  attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,TriFLB->getCC("outer"));  

  H2FL->addBoundarySurf("inner",Out);  
  H2FL->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(H2FL->getCC("outer"));
  H2FL->createAll(*SimPtr,1,1,*H2Mod);

  CH4FLA->addBoundarySurf("inner",Out);  
  CH4FLA->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(CH4FLA->getCC("outer"));
  CH4FLA->createAll(*SimPtr,*CH4Mod,*CH4Mod);

  CH4FLB->addBoundarySurf("inner",Out);  
  CH4FLB->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(CH4FLB->getCC("outer"));

  CH4FLB->createAll(*SimPtr,*CH4Mod,*CH4Mod);

  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,*H2PMod);  
  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
				    H2FL->getCC("outer"));  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
				    CH4FLA->getCC("outer"));  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
				    CH4FLA->getCC("outer"));  
  attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
				    CH4FLB->getCC("outer"));  

  //  attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,*CH4Mod);

  H2FL->processIntersectMajor(*SimPtr,*CH4FLB,"inner","outer");
  CH4FLB->processIntersectMinor(*SimPtr,*H2FL,"inner","outer");

    buildCH4Pipe(*SimPtr);
    buildH2Pipe(*SimPtr);
//    buildH2Pipe(*SimPtr,H2ModName);    
  //  buildWaterPipe(*SimPtr);

  return;
}


}   // NAMESPACE ts1System

