/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/makeT1Upgrade.cxx
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
#include <array>
#include <algorithm>
#include <iterator>
#include <memory>

#include <boost/format.hpp>

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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GroupOrigin.h"
#include "FlightLine.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "Window.h"
#include "BeamWindow.h"
#include "t1CylVessel.h"
#include "t1BulkShield.h"
#include "TargetBase.h"
#include "TS2target.h"
#include "TS2FlatTarget.h"
#include "TS2ModifyTarget.h"
#include "targCoolant.h"
#include "InnerTarget.h"
#include "Cannelloni.h"
#include "t1PlateTarget.h"
#include "SideCoolTarget.h"
#include "OpenBlockTarget.h"
#include "CylReflector.h"
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

#include "makeT1Upgrade.h"

#include "Debug.h"

namespace ts1System
{

makeT1Upgrade::makeT1Upgrade() :
  RefObj(new ts1System::CylReflector("CylRefl")),
  ColdCentObj(new constructSystem::GroupOrigin("ColdCent")),
  VoidObj(new shutterSystem::t1CylVessel("t1CylVessel")),
  BulkObj(new shutterSystem::t1BulkShield("t1Bulk")),
  MonoTopObj(new shutterSystem::MonoPlug("MonoTop")),
  MonoBaseObj(new shutterSystem::MonoPlug("MonoBase")),
  CH4PipeObj(new constructSystem::SupplyPipe("CH4Pipe")),
  H2PipeObj(new constructSystem::SupplyPipe("H2Pipe")),
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

  OR.addObject(VoidObj);
  OR.addObject(BulkObj);
  OR.addObject(MonoTopObj);
  OR.addObject(MonoBaseObj);

  OR.addObject(RefObj);
  OR.addObject(TriFLA);
  OR.addObject(TriFLB);

  OR.addObject(H2FL);
  OR.addObject(CH4FLA);
  OR.addObject(CH4FLB);

  OR.addObject(CH4PipeObj);
  OR.addObject(H2PipeObj);
  OR.addObject(WaterPipeObj);
  OR.addObject(WaterReturnObj);

}

makeT1Upgrade::makeT1Upgrade(const makeT1Upgrade& A) : 
  TarObj((A.TarObj) ? 
	 std::shared_ptr<constructSystem::TargetBase>
	 (A.TarObj->clone()) : A.TarObj),  
  BWindowObj(new ts1System::BeamWindow(*A.BWindowObj)),
  RefObj(new CylReflector(*A.RefObj)),
  TriMod((A.TriMod) ? 
	 std::shared_ptr<constructSystem::ModBase>
	  (A.TriMod->clone()) : A.TriMod),  
  ColdCentObj(new constructSystem::GroupOrigin(*A.ColdCentObj)),
  H2Mod((A.H2Mod) ? 
	 std::shared_ptr<constructSystem::ModBase>
	  (A.H2Mod->clone()) : A.H2Mod),  
  H2PMod((A.H2PMod) ? 
	 std::shared_ptr<ts1System::HPreMod>
	 (A.H2PMod->clone()) : A.H2PMod),  
  CH4Mod((A.CH4Mod) ? 
	 std::shared_ptr<constructSystem::ModBase>
	  (A.CH4Mod->clone()) : A.CH4Mod),  
  CH4PMod((A.CH4PMod) ? 
	 std::shared_ptr<ts1System::CH4PreModBase>
	  (A.CH4PMod->clone()) : A.CH4PMod),  
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
    \param A :: makeT1Upgrade to copy
  */
{}

makeT1Upgrade&
makeT1Upgrade::operator=(const makeT1Upgrade& A)
  /*!
    Assignment operator
    \param A :: makeT1Upgrade to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *TarObj = *A.TarObj;
      *BWindowObj = *A.BWindowObj;
      *RefObj = *A.RefObj;
      TriMod=((A.TriMod) ? 
	     std::shared_ptr<constructSystem::ModBase>
	     (A.TriMod->clone()) : A.TriMod);  
      *ColdCentObj = *A.ColdCentObj;
      H2Mod=((A.H2Mod) ? 
	     std::shared_ptr<constructSystem::ModBase>
	     (A.H2Mod->clone()) : A.H2Mod);  
      *H2PMod = *A.H2PMod;
      CH4Mod=((A.CH4Mod) ? 
	     std::shared_ptr<constructSystem::ModBase>
	     (A.CH4Mod->clone()) : A.CH4Mod);  
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


makeT1Upgrade::~makeT1Upgrade()
  /*!
    Destructor
   */
{}

std::string
makeT1Upgrade::buildTarget(Simulation& System,
			   const std::string& TType,
			   const int voidCell)
  /*!
    Create a target based on the 
    \param System :: Simulation for target
    \param TType :: Target Name
    \param voidCell :: Void cell number
    \return Container for reflector
  */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildTarget");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  ELog::EM<<"Target = "<<TType<<ELog::endDiag;

  if (TType=="t1PlateTarget" || TType=="t1Plate")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new t1PlateTarget("t1PlateTarget"));
      OR.addObject(TarObj);
      TarObj->addInsertCell(voidCell);
      TarObj->createAll(System,World::masterOrigin());
      return "PVessel";
    }
  else if (TType=="t1CylTarget" || TType=="t1Cyl")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new TMRSystem::TS2target("t1CylTarget"));
      OR.addObject(TarObj);
      // Cylinder [innernal] both the same
      TarObj->setRefPlates(RefObj->getLinkSurf(-3),
			   RefObj->getLinkSurf(-3));
      TarObj->createAll(System,World::masterOrigin());
      return "t1CylTarget";
    }    
  else if (TType=="t1CylFluxTrap" || TType=="t1CylFluxTrapTarget")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new TMRSystem::TS2FlatTarget("t1CylTarget"));
      OR.addObject("t1CylTarget",TarObj);
      // Cylinder [innernal] both the same
      TarObj->setRefPlates(RefObj->getLinkSurf(-3),
			   RefObj->getLinkSurf(-3));
      TarObj->createAll(System,World::masterOrigin());

      std::shared_ptr<TMRSystem::TS2ModifyTarget> TarObjModify
	(new TMRSystem::TS2ModifyTarget("t1CylFluxTrap"));
      TarObjModify->createAll(System,*TarObj);
      
      return "t1CylTarget";
    }    
  else if (TType=="t1InnerTarget" || TType=="t1Inner")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new ts1System::InnerTarget("t1Inner"));
      OR.addObject(TarObj);
      TarObj->setRefPlates(RefObj->getLinkSurf(-3),
			   RefObj->getLinkSurf(-3));
      TarObj->createAll(System,World::masterOrigin());
      return "t1Inner";
    }    
  else if (TType=="t1Side" || TType=="t1SideTarget")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new ts1System::SideCoolTarget("t1EllCylTarget"));
      OR.addObject(TarObj);
      /// Target

      TarObj->setRefPlates(RefObj->getLinkSurf(-3),
			   RefObj->getLinkSurf(-3));
      TarObj->createAll(System,World::masterOrigin());

      std::shared_ptr<ts1System::targCoolant> 
	TarCool(new ts1System::targCoolant("t1EllCylCool"));
      OR.addObject(TarCool);
      TarCool->addCells(TarObj->getInnerCells());
      TarCool->setContainer(TarObj->getContainer());
      TarCool->createAll(System,*TarObj);
      return "t1EllCylTarget";
    }    
  else if (TType=="t1Block" || TType=="t1BlockTarget")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new ts1System::OpenBlockTarget("t1BlockTarget"));
      OR.addObject(TarObj);
      TarObj->setRefPlates(RefObj->getLinkSurf(-3),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1BlockTarget";
    }    
  else if (TType=="t1Cannelloni" || TType=="t1CannelloniTarget")
    {
      TarObj=std::shared_ptr<constructSystem::TargetBase>
	(new ts1System::Cannelloni("t1Cannelloni"));
      OR.addObject(TarObj);
      TarObj->setRefPlates(RefObj->getLinkSurf(-3),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1Cannelloni";
    }    
  else if (TType=="Help" || TType=="help")
    {
      ELog::EM<<"Options [targetType] "<<ELog::endBasic;
      ELog::EM<<"    t1Block :: Open void blocks"<<ELog::endBasic;
      ELog::EM<<"    t1Cannelloni :: Inner cannolloni target"<<ELog::endBasic;
      ELog::EM<<"    t1Cyl   :: TS2 style cylindrical target"<<ELog::endBasic;
      ELog::EM<<"    t1CylFluxTrap :: Flux Trap target"<<ELog::endBasic;
      ELog::EM<<"    t1Inner :: Inner triple core target"<<ELog::endBasic;
      ELog::EM<<"    t1Plate :: Plate target [current]"<<ELog::endBasic;
      ELog::EM<<"    t1Side :: SideCoolded target"<<ELog::endBasic;
      return "";
    }    

  ELog::EM<<"Failed to understand target type :"
	  <<TType<<ELog::endErr;
  return "";
}

std::string
makeT1Upgrade::buildCH4Pre(Simulation& System,
			   const std::string& TType)
  /*!
    Create a target based on the 
    \param System :: Simulation for target
    \param TType :: Target Name
    \return Container for reflector
  */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildCH4PreMod");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (TType=="Wrapper")
    {
      CH4PMod=std::shared_ptr<ts1System::CH4PreModBase>
	(new CH4PreMod("CH4PreMod"));
      OR.addObject(CH4PMod);
      CH4PMod->createAll(System,*CH4Mod,0,3);
      return "CH4PreMod";
    }
  else if (TType=="Flat")
    {
      CH4PMod=std::shared_ptr<ts1System::CH4PreModBase>
	(new CH4PreFlat("CH4PreFlat"));
      OR.addObject(CH4PMod);
      CH4PMod->createAll(System,*CH4Mod,5,0);
      attachSystem::addToInsertSurfCtrl(System,*CH4PMod,*CH4Mod);
      return "CH4PreFlat";
    }
  else if (TType=="None")
    {
      return "None";
    }
  else if (TType=="Help" || TType=="help")
    {
      ELog::EM<<"Options = [CH4PreType] "<<ELog::endBasic;
      ELog::EM<<"    Flat :: Full wrapped pre-moderator"<<ELog::endBasic;
      ELog::EM<<"    Wrapper :: Full wrapped pre-moderator"<<ELog::endBasic;
      ELog::EM<<"    None :: No Premoderator "<<ELog::endBasic;
      return "";
    }    

  ELog::EM<<"Failed to understand CH4Pre type :"
	  <<TType<<ELog::endErr;
  return "";
}

std::string
makeT1Upgrade::buildCH4Mod(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const std::string& MType)
  /*!
    Create a methane moderator [CH4ModType]
    \param System :: Simulation for object
    \param FC :: Fixed component for coldMod center
    \param MType :: Moderator Name
    \return Container for reflector
  */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildCH4Mod");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (MType=="Basic")
    {
      CH4Mod=std::shared_ptr<constructSystem::ModBase>
	(new InnerLayer("CH4ModInner","CH4Mod"));
      OR.addObject(CH4Mod);
      CH4Mod->createAll(System,FC);
      return "CH4Mod";
    }
  else if (MType=="Layer")
    {
      CH4Mod=std::shared_ptr<constructSystem::ModBase>
	(new CH4Layer("CH4Layer"));
      OR.addObject(CH4Mod);
      CH4Mod->createAll(System,FC);
      return "CH4Layer";
    }
  else if (MType=="Help" || MType=="help")
    {
      ELog::EM<<"Options [CH4ModType]  "<<ELog::endBasic;
      ELog::EM<<"    Basic :: Standard TS1 Style"<<ELog::endBasic;
      ELog::EM<<"    Layer :: Layered Style"<<ELog::endBasic;
      return "";
    }     

  ELog::EM<<"Failed to understand CH4Mod type :"
	  <<MType<<ELog::endErr;
  return "";
}

void
makeT1Upgrade::buildH2Pipe(Simulation& System,
			   const std::string& MType)
  /*!
    Build the H2 Pipe
    \param System :: Simualtion to use
    \param MType :: Moderator type to connect to    
   */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildH2Pipe");

  H2PipeObj->setAngleSeg(12);
  H2PipeObj->setOption("");   // no modifiecation to the variable name
  if (MType=="Layer")
    {
      H2PipeObj->setWallOffset(6);
      H2PipeObj->createAll(System,*H2Mod,0,5,4);  
    }    

  // Moderator : [measure from origin]

  //  H2PipeObj->createAll(System,*H2Mod,0,5,5);  //,*H2PMod,2);

  return;
}

void
makeT1Upgrade::buildCH4Pipe(Simulation& System,
			    const std::string& MType)
  /*!
    Build the CH4 Pipe
    \param System :: Simualtion to use
    \param MType :: Moderator type to connect to    
   */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildCH4Pipe");

  CH4PipeObj->setAngleSeg(12);
  CH4PipeObj->setOption("");   // no modifiecation to the variable name
  if (MType=="Layer")
    {
      CH4PipeObj->setWallOffset(1);
      std::vector<size_t> LS;
      LS.push_back(1);
      LS.push_back(2);
      LS.push_back(3);
      LS.push_back(4);
      LS.push_back(5);
      CH4PipeObj->setLayerSeq(LS);
      CH4PipeObj->createAll(System,*CH4Mod,1,6,4);  
    }    

  // Moderator : [measure from origin]

  //  H2PipeObj->createAll(System,*H2Mod,0,5,5);  //,*H2PMod,2);

  return;
}

void
makeT1Upgrade::buildWaterPipe(Simulation& System,
			      const std::string& MType)
  /*!
    Build the Water Pipe
    \param System :: Simualtion sytem
    \param MType :: Moderator type to connect to
   */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildWaterPipe");
  return;
  WaterPipeObj->setAngleSeg(12);
  WaterPipeObj->setOption("");   // no modifiecation to the variable name
  WaterReturnObj->setAngleSeg(12);
  WaterReturnObj->setOption("");   // no modifiecation to the variable name
  ELog::EM<<"MType == "<<MType<<ELog::endDiag;
  if (MType=="Layer")
    {
      WaterPipeObj->setWallOffset(4);
      WaterPipeObj->createAll(System,*TriMod,0,6,5);  
      
      WaterReturnObj->setWallOffset(4);
      WaterReturnObj->createAll(System,*TriMod,0,6,5);  
    }
  else if (MType=="Triangle")
    {
      WaterPipeObj->setWallOffset(1);
      WaterPipeObj->createAll(System,*TriMod,0,2,1);  
      WaterReturnObj->setWallOffset(1);
      WaterReturnObj->createAll(System,*TriMod,0,2,1);  
    }
  return;
}

std::string
makeT1Upgrade::buildWaterMod(Simulation& System,
			     const attachSystem::FixedComp& FC,
			     const std::string& MType)
  /*!
    Create a Water moderator based on the input param
    \param System :: Simulation for target
    \param FC :: Fixed component for coldMod center
    \param MType :: Moderator Name
    \return Container for reflector
  */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildWaterMod");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  ELog::EM<<"MType - "<<MType<<ELog::endDiag;
  if (MType=="Triangle")
    {
      TriMod=std::shared_ptr<constructSystem::ModBase>
	(new moderatorSystem::TriangleMod("TriMod"));
      OR.addObject(TriMod);
      TriMod->createAll(System,FC);
      return "TriMod";
    }
  else if (MType=="Layer")
    {
      TriMod=std::shared_ptr<constructSystem::ModBase>
	  (new CH4Layer("TriModLayer"));
      OR.addObject(TriMod);
      TriMod->createAll(System,FC);
      
      return "TriModLayer";
    }
  else if (MType=="Help" || MType=="help")
    {
      ELog::EM<<"Options = [WaterModType]"<<ELog::endBasic;
      ELog::EM<<"    Triangle :: Complex Triangle Moderator"<<ELog::endBasic;
      ELog::EM<<"    Layer :: Basic layer moderator"<<ELog::endBasic;
      return "";
    }    
  
  ELog::EM<<"Failed to understand CH4Mod type :"
	  <<MType<<ELog::endErr;
  return "";
}
  
std::string
makeT1Upgrade::buildH2Mod(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const std::string& MType)
/*!
  Create a H2 moderator based on the input param
  \param System :: Simulation for target
  \param FC :: Fixed component for coldMod center
  \param MType :: Moderator Name
  \return Container name for reflector
*/
{
  ELog::RegMethod RegA("makeT1Upgrade","buildH2Mod");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (MType=="Basic")
    {
	H2Mod=std::shared_ptr<constructSystem::ModBase>
	  (new H2Section("H2Mod"));
	OR.addObject(H2Mod);
	H2Mod->createAll(System,FC);

	H2PMod=std::shared_ptr<ts1System::HPreMod>
	  (new HPreMod("HPreMod"));      
	H2PMod->createAll(System,*H2Mod,2);

	attachSystem::addToInsertControl
	  (System,*RefObj,"Reflector",*H2PMod,*H2PMod);

	return "H2Mod";
    }
  if (MType=="CylMod")
    {
	H2Mod=std::shared_ptr<constructSystem::ModBase>
	  (new constructSystem::CylMod("H2CylMod"));
	OR.addObject(H2Mod);
	H2Mod->createAll(System,FC);
	H2PCylMod=std::shared_ptr<ts1System::CylPreSimple>
	  (new CylPreSimple("H2CylPre"));      
	OR.addObject(H2PCylMod);
	return "CylMod";
    }
  else if (MType=="Layer")
    {
	H2Mod=std::shared_ptr<constructSystem::ModBase>
	  (new SplitInner("H2Inner","H2Layer"));
	OR.addObject(H2Mod);
	H2Mod->createAll(System,FC);
	return "H2Layer";
    }
  else if (MType=="Help" || MType=="help")
    {
	ELog::EM<<"Options = [H2ModType]"<<ELog::endBasic;
	ELog::EM<<"    Basic :: Standard TS1 Style"<<ELog::endBasic;
	ELog::EM<<"    CylMod :: Cylinder Moderator"<<ELog::endBasic;
	ELog::EM<<"    Layer :: Basic layer moderator"<<ELog::endBasic;
	return "";
    }    

  ELog::EM<<"Failed to understand CH4Mod type :"
	    <<MType<<ELog::endErr;
  return "";
}

void
makeT1Upgrade::buildHelp(Simulation& System) 
/*!
  Build a simple help schema if help is required
  \param System :: Simulation model
*/
{
  ELog::RegMethod RegA("makeT1Upgrade","buildHelp");

  buildTarget(System,"help",0);  
  buildH2Mod(System,*ColdCentObj,"help");
  buildCH4Mod(System,*ColdCentObj,"help");  
  buildCH4Pre(System,"help");

  return;
}


void 
makeT1Upgrade::build(Simulation& System,
		       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
  */
{
  // For output stream
  ELog::RegMethod RControl("makeT1Upgrade","build");

  if (IParam.flag("help"))
    buildHelp(System);

  const std::string TarName=
    IParam.getValue<std::string>("targetType",0);
  const std::string CH4PreName=
    IParam.getValue<std::string>("CH4PreType",0);
  const std::string CH4ModName=
    IParam.getValue<std::string>("CH4ModType",0);
  const std::string H2ModName=
    IParam.getValue<std::string>("H2ModType",0);
  const std::string WaterModName=
    IParam.getValue<std::string>("WaterModType",0);

  int voidCell(74123);
  if (!IParam.flag("exclude") ||
	(!IParam.compValue("E",std::string("Bulk")) && 
	 !IParam.compValue("E",std::string("Reflector")) )) 
    {
	//  VoidObj->addInsertCell(74123);  
	VoidObj->createAll(System);
    
	BulkObj->addInsertCell(voidCell);  
	BulkObj->createAll(System,IParam,*VoidObj);

	MonoTopObj->createAll(System,3,*VoidObj,*BulkObj);
	MonoBaseObj->createAll(System,2,*VoidObj,*BulkObj);
	voidCell=VoidObj->getVoidCell();
    }
  else
    {
      VoidObj->createStatus(System);
    }

  RefObj->addInsertCell(voidCell);
  RefObj->createAll(System,World::masterOrigin(),0);

  const std::string TarExcludeName=
    buildTarget(System,TarName,voidCell);

  RefObj->insertComponent(System,"Reflector",*TarObj);
  
  if (IParam.flag("exclude") &&
      IParam.compValue("E",std::string("Reflector")))
    {
      System.voidObject(RefObj->getKeyName());
      return;
    }

  const std::string WaterModExclude=
    buildWaterMod(System,World::masterOrigin(),WaterModName);

  attachSystem::addToInsertControl
	  (System,*RefObj,"Reflector",*TriMod,*TriMod);
  
  TarObj->addProtonLineInsertCell(RefObj->getCells());
  TarObj->addProtonLine(System,*RefObj,-3);

  // Cold centre
  ColdCentObj->createAll(System.getDataBase(),World::masterOrigin());
  const std::string H2ModExclude=
    buildH2Mod(System,*ColdCentObj,H2ModName);

  attachSystem::addToInsertControl
    (System,*RefObj,"Reflector",*H2Mod,*H2Mod);
    
  const std::string CH4ModExclude=
    buildCH4Mod(System,*ColdCentObj,CH4ModName);

  attachSystem::addToInsertControl
	  (System,*RefObj,"Reflector",*CH4Mod,*CH4Mod);

  const std::string CH4PreExclude=
    buildCH4Pre(System,CH4PreName);
  if (CH4PMod)
    {
      attachSystem::addToInsertControl
	  (System,*RefObj,"Reflector",*CH4PMod,*CH4PMod);
    }

  if (H2PMod && CH4Mod)
    attachSystem::addToInsertSurfCtrl(System,*CH4Mod,*H2PMod);  

  // FLIGHTLINES:
  const std::string Out=RefObj->getLinkString(-3);
  TriFLA->addBoundarySurf("inner",Out);  
  TriFLA->addBoundarySurf("outer",Out);
  TriFLA->createAll(System,*TriMod,*TriMod,TriMod->getSideIndex(0));
  RefObj->insertComponent(System,"Reflector",TriFLA->getCC("outer"));


  TriFLB->addBoundarySurf("inner",Out);  
  TriFLB->addBoundarySurf("outer",Out);
  TriFLB->createAll(System,*TriMod,*TriMod,TriMod->getSideIndex(1));
  RefObj->insertComponent(System,"Reflector",TriFLB->getCC("outer"));
  
  H2FL->addBoundarySurf("inner",Out);  
  H2FL->addBoundarySurf("outer",Out);
  H2FL->createAll(System,1,1,*H2Mod);
  RefObj->insertComponent(System,"Reflector",H2FL->getCC("outer"));

  CH4FLA->addBoundarySurf("inner",Out);  
  CH4FLA->addBoundarySurf("outer",Out);
  RefObj->insertComponent(System,"Reflector",CH4FLA->getCC("outer"));
  CH4FLA->createAll(System,*CH4Mod,*CH4Mod);

  CH4FLB->addBoundarySurf("inner",Out);  
  CH4FLB->addBoundarySurf("outer",Out);
  CH4FLB->createAll(System,*CH4Mod,*CH4Mod);
  RefObj->insertComponent(System,"Reflector",CH4FLB->getCC("outer"));

  if (H2Mod && CH4Mod)
    attachSystem::addToInsertSurfCtrl(System,*CH4Mod,*H2Mod);

  if (H2PCylMod)
    {
      H2PCylMod->createAll(System,*H2Mod,*H2FL);
      attachSystem::addToInsertControl
	  (System,*RefObj,"Reflector",*H2PCylMod,*H2PCylMod);
    }

  if (CH4PMod)
    {
      if (H2PMod)
	attachSystem::addToInsertSurfCtrl(System,*CH4PMod,*H2PMod);  
      else if (H2PCylMod)
	attachSystem::addToInsertSurfCtrl(System,*CH4PMod,*H2PCylMod);  

      attachSystem::addToInsertSurfCtrl(System,*CH4PMod,
					H2FL->getCC("outer"));  
      attachSystem::addToInsertSurfCtrl(System,*CH4PMod,
				      CH4FLA->getCC("outer"));  
      attachSystem::addToInsertSurfCtrl(System,*CH4PMod,
					CH4FLA->getCC("outer"));  
      attachSystem::addToInsertSurfCtrl(System,*CH4PMod,
					CH4FLB->getCC("outer"));  
    }

  attachSystem::addToInsertSurfCtrl(System,*RefObj,*CH4Mod);

  H2FL->processIntersectMajor(System,*CH4FLB,"inner","outer");
  CH4FLB->processIntersectMinor(System,*H2FL,"inner","outer");

  buildCH4Pipe(System,CH4ModName);
  //  buildH2Pipe(System,H2ModName);
  buildWaterPipe(System,WaterModName);

  return;
}


}   // NAMESPACE ts1System

