/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/makeT1Upgrade.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

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
#include "TS2target.h"
#include "TS2moly.h"
#include "InnerTarget.h"
#include "Cannelloni.h"
#include "t1PlateTarget.h"
#include "SideCoolTarget.h"
#include "OpenBlockTarget.h"
#include "CylReflector.h"
#include "TriangleMod.h"
#include "ModBase.h"
#include "H2Section.h"
#include "LayerInfo.h"
#include "CH4Layer.h"
#include "HPreMod.h"
#include "CH4PreModBase.h"
#include "CH4PreMod.h"
#include "CH4PreFlat.h"
#include "ConicInfo.h"
#include "CylMod.h"
#include "CylPreSimple.h"
#include "MonoPlug.h"
#include "World.h"
#include "AttachSupport.h"

#include "makeT1Upgrade.h"

#include "Debug.h"

namespace ts1System
{

makeT1Upgrade::makeT1Upgrade() :
  RefObj(new ts1System::CylReflector("CylRefl")),
  TriMod(new moderatorSystem::TriangleMod("TriMod")),
  ColdCentObj(new constructSystem::GroupOrigin("ColdCent")),
  VoidObj(new shutterSystem::t1CylVessel("t1CylVessel")),
  BulkObj(new shutterSystem::t1BulkShield("t1Bulk")),
  MonoTopObj(new shutterSystem::MonoPlug("MonoTop")),
  MonoBaseObj(new shutterSystem::MonoPlug("MonoBase")),

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
  OR.addObject(TriMod);
  OR.addObject(TriFLA);
  OR.addObject(TriFLB);

  OR.addObject(H2FL);
  OR.addObject(CH4FLA);
  OR.addObject(CH4FLB);

}

makeT1Upgrade::makeT1Upgrade(const makeT1Upgrade& A) : 
  TarObj((A.TarObj) ? 
	 boost::shared_ptr<constructSystem::TargetBase>
	 (A.TarObj->clone()) : A.TarObj),  
  BWindowObj(new ts1System::BeamWindow(*A.BWindowObj)),
  RefObj(new CylReflector(*A.RefObj)),
  TriMod(new moderatorSystem::TriangleMod(*A.TriMod)),
  ColdCentObj(new constructSystem::GroupOrigin(*A.ColdCentObj)),
  H2Mod((A.H2Mod) ? 
	 boost::shared_ptr<constructSystem::ModBase>
	  (A.H2Mod->clone()) : A.H2Mod),  
  H2PMod((A.H2PMod) ? 
	 boost::shared_ptr<ts1System::HPreMod>
	 (A.H2PMod->clone()) : A.H2PMod),  
  CH4Mod((A.CH4Mod) ? 
	 boost::shared_ptr<constructSystem::ModBase>
	  (A.CH4Mod->clone()) : A.CH4Mod),  
  CH4PMod((A.CH4PMod) ? 
	 boost::shared_ptr<ts1System::CH4PreModBase>
	  (A.CH4PMod->clone()) : A.CH4PMod),  
  VoidObj(new shutterSystem::t1CylVessel(*A.VoidObj)),
  BulkObj(new shutterSystem::t1BulkShield(*A.BulkObj)),
  MonoTopObj(new shutterSystem::MonoPlug(*A.MonoTopObj)),
  MonoBaseObj(new shutterSystem::MonoPlug(*A.MonoBaseObj)),
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
      *TriMod = *A.TriMod;
      *ColdCentObj = *A.ColdCentObj;
      H2Mod=((A.H2Mod) ? 
	     boost::shared_ptr<constructSystem::ModBase>
	     (A.H2Mod->clone()) : A.H2Mod);  
      *H2PMod = *A.H2PMod;
      CH4Mod=((A.CH4Mod) ? 
	     boost::shared_ptr<constructSystem::ModBase>
	     (A.CH4Mod->clone()) : A.CH4Mod);  
      *CH4PMod = *A.CH4PMod;
      *VoidObj = *A.VoidObj;
      *BulkObj = *A.BulkObj;
      *MonoTopObj = *A.MonoTopObj;
      *MonoBaseObj = *A.MonoBaseObj;
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

  if (TType=="t1PlateTarget" || TType=="t1Plate")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new t1PlateTarget("t1PlateTarget"));
      OR.addObject(TarObj);
      TarObj->addInsertCell(voidCell);
      RefObj->addToInsertChain(*TarObj);
      TarObj->createAll(System,World::masterOrigin());
      return "PVessel";
    }
  else if (TType=="t1CylTarget" || TType=="t1Cyl")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new TMRSystem::TS2target("t1CylTarget"));
      OR.addObject(TarObj);
      RefObj->addToInsertChain(*TarObj);
      TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1CylTarget";
    }    
  else if (TType=="t1CylFluxTrap" || TType=="t1CylFluxTrapTarget")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new TMRSystem::TS2moly("t1CylFluxTrap","t1CylTarget"));
      OR.addObject("t1CylTarget",TarObj);
      OR.addObject("t1CylFluxTrap",TarObj);
      RefObj->addToInsertChain(*TarObj);
      TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1CylTarget";
    }    
  else if (TType=="t1InnerTarget" || TType=="t1Inner")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new ts1System::InnerTarget("t1Inner"));
      OR.addObject(TarObj);
      RefObj->addToInsertChain(*TarObj);
      TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1Inner";
    }    
  else if (TType=="t1Side" || TType=="t1SideTarget")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new ts1System::SideCoolTarget("t1EllCylTarget"));
      OR.addObject(TarObj);
      RefObj->addToInsertChain(*TarObj);
      TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1EllCylTarget";
    }    
  else if (TType=="t1Block" || TType=="t1BlockTarget")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new ts1System::OpenBlockTarget("t1BlockTarget"));
      OR.addObject(TarObj);
      RefObj->addToInsertChain(*TarObj);
      TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1BlockTarget";
    }    
  else if (TType=="t1Cannelloni" || TType=="t1CannelloniTarget")
    {
      TarObj=boost::shared_ptr<constructSystem::TargetBase>
	(new ts1System::Cannelloni("t1Cannelloni"));
      OR.addObject(TarObj);
      RefObj->addToInsertChain(*TarObj);
      TarObj->setRefPlates(-RefObj->getLinkSurf(2),0);
      TarObj->createAll(System,World::masterOrigin());
      return "t1Cannelloni";
    }    
  else if (TType=="Help" || TType=="help")
    {
      ELog::EM<<"Options = "<<ELog::endBasic;
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
      CH4PMod=boost::shared_ptr<ts1System::CH4PreModBase>
	(new CH4PreMod("CH4PreMod"));
      OR.addObject(CH4PMod);
      CH4PMod->createAll(System,*CH4Mod,0,3);
      return "CH4PreMod";
    }
  else if (TType=="Flat")
    {
      CH4PMod=boost::shared_ptr<ts1System::CH4PreModBase>
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
      CH4Mod=boost::shared_ptr<constructSystem::ModBase>
	(new CH4Layer("CH4Mod"));
      OR.addObject(CH4Mod);
      CH4Mod->createAll(System,FC);
      return "CH4Mod";
    }
  else if (MType=="Layer")
    {
      CH4Mod=boost::shared_ptr<constructSystem::ModBase>
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

std::string
makeT1Upgrade::buildH2Mod(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const std::string& MType)
  /*!
    Create a H2 moderator based on the input param
    \param System :: Simulation for target
    \param FC :: Fixed component for coldMod center
    \param MType :: Moderator Name
    \return Container for reflector
  */
{
  ELog::RegMethod RegA("makeT1Upgrade","buildH2Mod");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (MType=="Basic")
    {
      H2Mod=boost::shared_ptr<constructSystem::ModBase>
	(new H2Section("H2Mod"));
      OR.addObject(H2Mod);
      H2Mod->createAll(System,FC);

      H2PMod=boost::shared_ptr<ts1System::HPreMod>
	(new HPreMod("HPreMod"));      
      H2PMod->createAll(System,*H2Mod,1);
      RefObj->addToInsertControl(System,*H2PMod,*H2PMod);

      return "H2Mod";
    }
  if (MType=="CylMod")
    {
      H2Mod=boost::shared_ptr<constructSystem::ModBase>
	(new constructSystem::CylMod("H2CylMod"));
      OR.addObject(H2Mod);
      H2Mod->createAll(System,FC);
      H2PCylMod=boost::shared_ptr<ts1System::CylPreSimple>
	(new CylPreSimple("H2CylPre"));      
      OR.addObject(H2PCylMod);
      return "CylMod";
    }
  else if (MType=="Help" || MType=="help")
    {
      ELog::EM<<"Options = [H2ModType]"<<ELog::endBasic;
      ELog::EM<<"    Basic :: Standard TS1 Style"<<ELog::endBasic;
      ELog::EM<<"    CylMod :: Cylinder Moderator"<<ELog::endBasic;
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
    \param System :: 
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
makeT1Upgrade::build(Simulation* SimPtr,
		     const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeT1Upgrade","build");

  if (IParam.flag("help"))
    buildHelp(*SimPtr);

  const std::string TarName=
    IParam.getValue<std::string>("targetType",0);
  const std::string CH4PreName=
    IParam.getValue<std::string>("CH4PreType",0);
  const std::string CH4ModName=
    IParam.getValue<std::string>("CH4ModType",0);
  const std::string H2ModName=
    IParam.getValue<std::string>("H2ModType",0);

  int voidCell(74123);
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

  const std::string TarExcludeName=
    buildTarget(*SimPtr,TarName,voidCell);

  if (IParam.flag("exclude") &&
      IParam.compValue("E",std::string("Reflector")))
    return;

  TarObj->addProtonLineInsertCell(RefObj->getCells());
  TarObj->addProtonLine(*SimPtr,*RefObj,-3);

  TriMod->createAll(*SimPtr,World::masterOrigin());
  RefObj->addToInsertControl(*SimPtr,*TriMod,*TriMod);
  //  attachSystem::addToInsertControl(*SimPtr,*TriMod,*TriMod);

  // Cold centre
  ColdCentObj->createAll(*SimPtr,World::masterOrigin());

  const std::string H2ModExclude=
    buildH2Mod(*SimPtr,*ColdCentObj,H2ModName);

  RefObj->addToInsertControl(*SimPtr,*H2Mod,*H2Mod);

  const std::string CH4ModExclude=
    buildCH4Mod(*SimPtr,*ColdCentObj,CH4ModName);

  RefObj->addToInsertControl(*SimPtr,*CH4Mod,*CH4Mod);

  const std::string CH4PreExclude=
    buildCH4Pre(*SimPtr,CH4PreName);
  if (CH4PMod)
    RefObj->addToInsertControl(*SimPtr,*CH4PMod,*CH4PMod);

  if (H2PMod && CH4Mod)
    attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4Mod,*H2PMod);  

  // FLIGHTLINES:
  const std::string Out=RefObj->getLinkComplement(2);
  TriFLA->addBoundarySurf("inner",Out);  
  TriFLA->addBoundarySurf("outer",Out);  
  //  RefObj->addToInsertChain(TriFLA->getKey("outer"));
  //  TriFLA->createAll(*SimPtr,*TriMod,*TriMod);

  TriFLB->addBoundarySurf("inner",Out);  
  TriFLB->addBoundarySurf("outer",Out);  
  //  TriFLB->createAll(*SimPtr,*TriMod,*TriMod);
  // attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,TriFLB->getKey("outer"));  

  H2FL->addBoundarySurf("inner",Out);  
  H2FL->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(H2FL->getKey("outer"));
  H2FL->createAll(*SimPtr,1,1,*H2Mod);

  CH4FLA->addBoundarySurf("inner",Out);  
  CH4FLA->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(CH4FLA->getKey("outer"));
  CH4FLA->createAll(*SimPtr,*CH4Mod,*CH4Mod);

  CH4FLB->addBoundarySurf("inner",Out);  
  CH4FLB->addBoundarySurf("outer",Out);  
  RefObj->addToInsertChain(CH4FLB->getKey("outer"));

  CH4FLB->createAll(*SimPtr,*CH4Mod,*CH4Mod);

  if (H2PCylMod)
    {
      H2PCylMod->createAll(*SimPtr,*H2Mod,*H2FL);
      RefObj->addToInsertControl(*SimPtr,*H2PCylMod,*H2PCylMod);
    }

  if (CH4PMod)
    {
      if (H2PMod)
	attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,*H2PMod);  
      else if (H2PCylMod)
	attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,*H2PCylMod);  

      attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
					H2FL->getKey("outer"));  
      attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
				      CH4FLA->getKey("outer"));  
      attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
					CH4FLA->getKey("outer"));  
      attachSystem::addToInsertSurfCtrl(*SimPtr,*CH4PMod,
					CH4FLB->getKey("outer"));  
    }

  attachSystem::addToInsertSurfCtrl(*SimPtr,*RefObj,*CH4Mod);

  H2FL->processIntersectMajor(*SimPtr,*CH4FLB,"inner","outer");
  CH4FLB->processIntersectMinor(*SimPtr,*H2FL,"inner","outer");

  return;
}


}   // NAMESPACE ts1System

