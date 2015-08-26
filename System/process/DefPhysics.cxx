/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/DefPhysics.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "Object.h"
#include "Qhull.h"
#include "Source.h"
#include "KCode.h"
#include "Simulation.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PStandard.h"
#include "ModeCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "DefPhysics.h"

namespace ModelSupport
{

void
setItemRotate(const attachSystem::FixedComp& WMaster,
	      const std::string& ItemName)
  /*!
    Add a specific rotation
    \param WMaster :: ??
    \param ItemName :: Item name for fixedComp
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","setItemRotate");

  if (ItemName.empty())
    return;

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  masterRotate& MR = masterRotate::Instance();

  Geometry::Vec3D newOrigin;
  Geometry::Quaternion QA;

  const attachSystem::SecondTrack*
    TwinPtr=OR.getObject<attachSystem::SecondTrack>(ItemName);  
  if (!TwinPtr)
    {
      const attachSystem::FixedComp* 
	ItemPtr=OR.getObject<attachSystem::FixedComp>(ItemName);  
      if (!ItemPtr)
	throw ColErr::InContainerError<std::string>(ItemName,
						    "Object not found");
      
      newOrigin=ItemPtr->getCentre();
      QA=Geometry::Quaternion::basisRotate(WMaster.getX(),
					   WMaster.getY(),
					   WMaster.getZ(),
					   ItemPtr->getX(),
					   ItemPtr->getY(),
					   ItemPtr->getZ());
    }
  else 
    {
      //      const attachSystem::FixedComp* 
      //	ItemPtr=OR.getObject<attachSystem::FixedComp>(ItemName);  
      newOrigin=TwinPtr->getBeamStart();
      QA=Geometry::Quaternion::basisRotate(WMaster.getX(),
					   WMaster.getY(),
					   WMaster.getZ(),
					   TwinPtr->getBZ(),
					   TwinPtr->getBY(),
					   TwinPtr->getBX());
    }
  
  MR.reset();
  MR.addRotation(QA.getAxis(),
		 newOrigin,
		 180.0*QA.getTheta()/M_PI);
  MR.addDisplace(-newOrigin);
  
  return;
}


std::string
setDefRotation(const mainSystem::inputParam& IParam)
  /*!
    Apply a standard rotation to the simulation
    \param IParam :: Parameter set
    \return Post work required
   */
{
  ELog::RegMethod RegA("DefPhysics[F]","setDefRotation");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::string retFlag;
  masterRotate& MR = masterRotate::Instance();
  if (IParam.flag("axis"))
    {

      // Move X to Z:
      MR.addRotation(Geometry::Vec3D(0,1,0),
		     Geometry::Vec3D(0,0,0),
		     90.0);
      //Move XY to -X-Y 
      MR.addRotation(Geometry::Vec3D(0,0,1),
		     Geometry::Vec3D(0,0,0),
		     -90.0);
      MR.addMirror(Geometry::Plane
		   (1,0,Geometry::Vec3D(0,0,0),
		    Geometry::Vec3D(1,0,0)));

    }
  if (IParam.flag("angle"))
    {
      std::string AItem=
	IParam.getValue<std::string>("angle");
      if (AItem=="chipIR" || AItem=="ChipIR")
	MR.addRotation(Geometry::Vec3D(0,0,1),
		       Geometry::Vec3D(0,0,0),
		       -45.85);
      else if (AItem=="zoom" || AItem=="Zoom")
	{
	  MR.addRotation(Geometry::Vec3D(0,0,1),
			 Geometry::Vec3D(0,0,0),
			 45.00-180.0);
	}
      else if (AItem=="ODIN" || AItem=="odin" ||
	       AItem=="LOKI" || AItem=="loki" ||
	       AItem=="VOR" || AItem=="vor") 
	{
	  std::transform(AItem.begin(),AItem.end(),
	    AItem.begin(),::tolower);
	  
	  const attachSystem::FixedComp* GIPtr=
	    OR.getObject<attachSystem::FixedComp>(AItem+"Axis");
	  if (!GIPtr)
	    throw ColErr::InContainerError<std::string>
	      (AItem+"Axis","Fixed component");
	  // Y is beam direction -- Alignment along X
	  const double angle=180.0*acos(GIPtr->getY()[0])/M_PI;
	  MR.addRotation(GIPtr->getZ(),Geometry::Vec3D(0,0,0),
			 -angle);
	}
      else 
	retFlag=AItem;
    }

  if (IParam.compValue("isolate",std::string("chipIR")))
    {
      MR.addRotation(Geometry::Vec3D(1,0,0),
		     Geometry::Vec3D(0,0,0),
		     -90);
      MR.addMirror(Geometry::Plane
		   (1,0,Geometry::Vec3D(0,0,0),
		    Geometry::Vec3D(0,0,1)));
      MR.addRotation(Geometry::Vec3D(0,0,1),
		     Geometry::Vec3D(0,0,0),
		     180);
      MR.addMirror(Geometry::Plane
		   (19,0,Geometry::Vec3D(0,0,0),
		    Geometry::Vec3D(0,1,0)));

    }

      //   MR.addDisplace(Geometry::Vec3D(-1175.0,0,0));

  return retFlag;
}

void
setPhysicsModel(physicsSystem::LSwitchCard& lea,
		const std::string& PModel)
  /*!
    Set the physics model based on the input parameter set
    \param lea :: Physics system
    \param PModel :: Physics model to choose 
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","setPhysicsModel");

// Goran

  ELog::EM<<"Physics Model == "<<PModel<<ELog::endBasic;

  if (PModel=="CEM03")
    lea.setValues("lca","2 1 1 0023 1 1 0 1 1 0");  // CEM
  else if (PModel=="IA")
    lea.setValues("lca","2 1 0 0023 1 1 2 1 2 0");  // INCL4 - ABLA
  else if (PModel=="BD")
    lea.setValues("lca","2 1 1 0023 1 1 0 1 0 0");  // Bertini - DrAnnesner   
  else if (PModel=="BA")
    lea.setValues("lca","2 1 1 0023 1 1 2 1 0 0");  // Bertini - ABLA  
  else
    {
      ELog::EM<<"physModel :\n"
	"CEM03 :: CEM03 model \n"
	"IA :: INCL4 - ABLA model \n"
	"BD :: Bertini - Dresner model \n"
	"BA :: Bertini - ABLA model"<<ELog::endBasic;
      throw ColErr::ExitAbort("No model");
    }
  lea.setValues("lea","1 4 1 0 1 0 0 1");
  return;
}

void 
setDefaultPhysics(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Set the default Physics
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("DefPhysics","setDefaultPhysics");

  // If Reactor stuff set and void
  if (IParam.hasKey("kcode") && IParam.dataCnt("kcode"))
    {
      setReactorPhysics(System,IParam);
      return;
    }

  const FuncDataBase& Control=System.getDataBase();
  
  std::string PList("h / d t s a");
  const double maxEnergy=Control.EvalDefVar<double>("sdefEnergy",800.0);
  const double elcEnergy=IParam.getValue<double>("electron");
  const double phtEnergy=IParam.getValue<double>("photon");
  const double phtModel=IParam.getValue<double>("photonModel");
  const std::string elcAdd((elcEnergy>0 ? " e" : ""));

  physicsSystem::PhysicsCards& PC=System.getPC();
  PC.setMode("n p "+PList+elcAdd);
  PC.setPrintNum("10 110");
  System.processCellsImp();

  PC.setCells("imp",1,0);            // Set a zero cell	  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,1.0e+8,0.0,0.4,-0.1);
  
  physicsSystem::PStandard* allCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut",PList);
  allCut->setValues(2,1e+8,0.0);
  physicsSystem::PStandard* photonCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut","p");
  photonCut->setValues(2,1e+8,phtEnergy);

  if (elcEnergy>=0.0)
    {
      physicsSystem::PStandard* elcCut=
	PC.addPhysCard<physicsSystem::PStandard>("cut","e");
      elcCut->setValues(2,1e+8,elcEnergy);
    }
  
  const std::string EMax=StrFunc::makeString(maxEnergy);
  const std::string PHMax=StrFunc::makeString(phtModel);
  // Process physics
  physicsSystem::PStandard* pn=
	PC.addPhysCard<physicsSystem::PStandard>("phys","n");
  pn->setValues(EMax+" 0.0 j j j");

  physicsSystem::PStandard* pp=
	PC.addPhysCard<physicsSystem::PStandard>("phys","p");
  if (elcEnergy>=0.0)
    pp->setValues(PHMax+" j j -1");
  else
    pp->setValues(PHMax);


  physicsSystem::PStandard* pa=
	PC.addPhysCard<physicsSystem::PStandard>("phys","/ d t s a "+elcAdd);
  pa->setValues(EMax);
  
  physicsSystem::PStandard* ph=
	PC.addPhysCard<physicsSystem::PStandard>("phys","h");
  ph->setValues(EMax);

  // LCA ielas ipreq iexisa ichoic jcoul nexite npidk noact icem ilaq 
  // LEA ipht icc nobalc nobale ifbrk ilvden ievap nofis
  physicsSystem::LSwitchCard& lea=PC.getLEA();

  const std::string PModel=IParam.getValue<std::string>("physModel");
  setPhysicsModel(lea,PModel);

  PC.setNPS(IParam.getValue<int>("nps"));
  PC.setRND(IParam.getValue<long int>("random"));	
  PC.setVoidCard(IParam.flag("void"));
  
  return; 
}



void 
setReactorPhysics(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Set the default Physics
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("DefPhysics","setDefaultPhysics");

  const FuncDataBase& Control=System.getDataBase();
  
  std::string PList("");
  const double maxEnergy=Control.EvalDefVar<double>("sdefEnergy",20.0);
  const double elcEnergy=IParam.getValue<double>("electron");
  const double phtEnergy=IParam.getValue<double>("photon");
  const double phtModel=IParam.getValue<double>("photonModel");
  const std::string elcAdd((elcEnergy>0 ? " e" : ""));

  System.getPC().setMode("n p "+PList+elcAdd);
  System.getPC().setPrintNum("10 110");
  System.processCellsImp();
  physicsSystem::PhysicsCards& PC=System.getPC();

  PC.setCells("imp",1,0);            // Set a zero cell
  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,1.0e+8,0.0,0.4,-0.1);
  
  physicsSystem::PStandard* allCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut",PList);
  allCut->setValues(2,1e+8,0.0);
  physicsSystem::PStandard* photonCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut","p");
  photonCut->setValues(2,1e+8,phtEnergy);

  if (elcEnergy>=0.0)
    {
      physicsSystem::PStandard* elcCut=
	PC.addPhysCard<physicsSystem::PStandard>("cut","e");
      elcCut->setValues(2,1e+8,elcEnergy);
    }

  
  const std::string EMax=StrFunc::makeString(maxEnergy);
  const std::string PHMax=StrFunc::makeString(phtModel);
  physicsSystem::PStandard* pn=
	PC.addPhysCard<physicsSystem::PStandard>("phys","n");
  pn->setValues(EMax+" 0.0 j j j");
  
  physicsSystem::PStandard* pp=
	PC.addPhysCard<physicsSystem::PStandard>("phys","p");
  if (elcEnergy>=0.0)
    pp->setValues(PHMax+" j j 1");
  else
    pp->setValues(PHMax);

  physicsSystem::PStandard* pa=
    PC.addPhysCard<physicsSystem::PStandard>("phys","/ d t s a "+elcAdd);
  pa->setValues(EMax);
  
  physicsSystem::PStandard* ph=
	PC.addPhysCard<physicsSystem::PStandard>("phys","h");
  ph->setValues(EMax);


  // LCA ielas ipreq iexisa ichoic jcoul nexite npidk noact icem ilaq 
  // LEA ipht icc nobalc nobale ifbrk ilvden ievap nofis
  physicsSystem::LSwitchCard& lea=PC.getLEA();
  // GORAN
  const std::string PModel=IParam.getValue<std::string>("physModel");
  setPhysicsModel(lea,PModel);

  PC.setNPS(IParam.getValue<int>("nps"));
  PC.setRND(IParam.getValue<long int>("random"));	
  PC.setVoidCard(IParam.flag("void"));
  
  return; 
}

} // NAMESPACE ModelSupport
