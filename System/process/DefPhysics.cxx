/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/DefPhysics.cxx
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
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PStandard.h"
#include "PSimple.h"
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
      const attachSystem::FixedComp* ItemPtr=
	OR.getObjectThrow<attachSystem::FixedComp>(ItemName,"FixedComp");  
      
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

void
procAngle(const mainSystem::inputParam& IParam,
          const size_t index)
  /*!
    Process an angle unit
    \param IParam :: Input param
    \param index :: set index
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","procAngle");
  
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  masterRotate& MR = masterRotate::Instance();

  const std::string AItem=
    IParam.getValue<std::string>("angle",index,0);
  const std::string BItem=(IParam.itemCnt("angle",index)>1) ?
    IParam.getValue<std::string>("angle",index,1) : "";

  if (AItem=="object" || AItem=="Object")
    {
      const attachSystem::FixedComp* GIPtr=
        OR.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("2","angle",index,2);
      const int ZFlag=IParam.getDefValue<int>(1,"angle",index,3);
      const long int axisIndex=attachSystem::getLinkIndex(CItem);

      const Geometry::Vec3D AxisVec=
        GIPtr->getLinkAxis(axisIndex);

      // Align item such that we put the object linkPt at +ve X
      const Geometry::Vec3D ZRotAxis=GIPtr->getZ();

      const double angle=180.0*acos(AxisVec[0])/M_PI;
      MR.addRotation(GIPtr->getZ(),
                     Geometry::Vec3D(0,0,0),ZFlag*angle);
      // Z rotation.
      const double angleZ=90.0-180.0*acos(-AxisVec[2])/M_PI;
      MR.addRotation(GIPtr->getX(),Geometry::Vec3D(0,0,0),-angleZ);
      ELog::EM<<"ROTATION AXIS["<<ZFlag<<"] == "
              <<AxisVec<<ELog::endDiag;

    }
  else  if (AItem=="objPoint" || AItem=="ObjPoint")
    {
      const attachSystem::FixedComp* GIPtr=
        OR.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("2","angle",index,2);

      const long int sideIndex=attachSystem::getLinkIndex(CItem);
          
      Geometry::Vec3D LP=GIPtr->getLinkPt(sideIndex);
      LP=LP.cutComponent(Geometry::Vec3D(0,0,1));
      LP.makeUnit();

      double angleZ=180.0*acos(LP[0])/M_PI;
      if (LP[1]>0.0) angleZ*=-1;
      MR.addRotation(Geometry::Vec3D(0,0,1),
                     Geometry::Vec3D(0,0,0),angleZ);
    }
  else  if (AItem=="objAxis" || AItem=="ObjAxis")
    {
      const attachSystem::FixedComp* GIPtr=
        OR.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("2","angle",index,2);
      
      const long int sideIndex=attachSystem::getLinkIndex(CItem);
      
      Geometry::Vec3D XRotAxis,YRotAxis,ZRotAxis;
      GIPtr->selectAltAxis(sideIndex,XRotAxis,YRotAxis,ZRotAxis);
      
      const Geometry::Quaternion QR=Geometry::Quaternion::calcQVRot
	(Geometry::Vec3D(1,0,0),YRotAxis,ZRotAxis);
      
      MR.addRotation(QR.getAxis(),Geometry::Vec3D(0,0,0),
		     -180.0*QR.getTheta()/M_PI);
    }
  else if (AItem=="free" || AItem=="FREE")
    {
      const double rotAngle=
        IParam.getValue<double>("angle",index,1);
      MR.addRotation(Geometry::Vec3D(0,0,1),Geometry::Vec3D(0,0,0),
                     -rotAngle);
      ELog::EM<<"ADDING ROTATION "<<rotAngle<<ELog::endDiag;
    }
  else if (AItem=="freeAxis" || AItem=="FREEAXIS")
    {
      size_t itemIndex(1);
      const Geometry::Vec3D rotAxis=
        IParam.getCntVec3D("angle",index,itemIndex,"Axis need [Vec3D]");
      const double rotAngle=
        IParam.getValue<double>("angle",index,itemIndex);
      MR.addRotation(rotAxis,Geometry::Vec3D(0,0,0),
                     -rotAngle);		  
    }
  else if (AItem=="help" || AItem=="Help")
    {
      ELog::EM<<"Angle help ::\n"
              <<"  free rotAngle :: Rotate about Z axis \n"
              <<"  freeAxis Vec3D rotAngle :: Rotate about Axis \n"
              <<"  objPoint  FC link :: Rotate linkPt to (X,0,0) \n"
              <<"  objAxis  FC link :: Rotate link-axit to X \n"
              <<"  object  FC link :: Rotate Axis about Z to "
              <<ELog::endDiag;
    }
  else
    throw ColErr::InContainerError<std::string>(AItem,"angle input error");
      
  return;
}


void
procOffset(const mainSystem::inputParam& IParam,
           const size_t index)
  /*!
    Process an offset unit
    \param IParam :: Input param
    \param index :: set index
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","procOffset");
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  masterRotate& MR = masterRotate::Instance();  

  const std::string AItem=
    IParam.getValue<std::string>("offset",index);
  const std::string BItem=(IParam.itemCnt("offset",index)>1) ?
    IParam.getValue<std::string>("offset",index,1) : "";

  if (AItem=="object" || AItem=="Object")
    {
      const attachSystem::FixedComp* GIPtr=
        OR.getObjectThrow<attachSystem::FixedComp>(BItem,"FixedComp");
      const std::string CItem=
        IParam.getDefValue<std::string>("0","offset",index,2);
      const long int linkIndex=attachSystem::getLinkIndex(CItem);
      ELog::EM<<"Offset at "<<GIPtr->getLinkPt(linkIndex)
              <<ELog::endDiag;
      MR.addDisplace(-GIPtr->getLinkPt(linkIndex));
    }
  else if (AItem=="free" || AItem=="FREE")
    {
      size_t itemIndex(1);
      const Geometry::Vec3D OffsetPos=
        IParam.getCntVec3D("offset",index,itemIndex,"Offset need vec3D");
      MR.addDisplace(-OffsetPos);
    }
  else
    throw ColErr::InContainerError<std::string>(AItem,"offset: input error");

  return;
}
  
void
setDefRotation(const mainSystem::inputParam& IParam)
  /*!
    Apply a standard rotation to the simulation
    \param IParam :: Parameter set
   */
{
  ELog::RegMethod RegA("DefPhysics[F]","setDefRotation");

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

  if (IParam.flag("offset"))
    {
      const size_t nP=IParam.setCnt("offset");
      for(size_t i=0;i<nP;i++)
        procOffset(IParam,i);
    }
  if (IParam.flag("angle"))
    {
      const size_t nP=IParam.setCnt("angle");
      for(size_t i=0;i<nP;i++)
        procAngle(IParam,i);
    }
  return;
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
    {
      lea.setValues("lca","2 1 1 0023 1 1 0 1 1 0");  // CEM
      lea.setValues("lea","1 4 1 0 1 0 0 1");
    }
  else if (PModel=="IA")
    {
      lea.setValues("lca","2 1 0 0023 1 1 2 1 2 0");  // INCL4 - ABLA
      lea.setValues("lea","1 4 1 0 1 0 2 1");
    }
  else if (PModel=="BD")
    {
      lea.setValues("lca","2 1 1 0023 1 1 0 1 0 0");  // Bertini -
                                                      // DrAnnesner
      lea.setValues("lea","1 4 1 0 1 0 0 1");
    }
  else if (PModel=="BA")
    {
      lea.setValues("lca","2 1 1 0023 1 1 2 1 0 0");  // Bertini - ABLA
      lea.setValues("lea","1 4 1 0 1 0 2 1");
    }
  else
    {
      ELog::EM<<"physModel :\n"
	"CEM03 :: CEM03 model \n"
	"IA :: INCL4 - ABLA model \n"
	"BD :: Bertini - Dresner model \n"
	"BA :: Bertini - ABLA model"<<ELog::endBasic;
      throw ColErr::ExitAbort("No model");
    }

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

  // LCA ielas ipreq iexisa ichoic jcoul nexite npidk noact icem ilaq 
  // LEA ipht icc nobalc nobale ifbrk ilvden ievap nofis
  physicsSystem::PhysicsCards& PC=System.getPC();
  physicsSystem::LSwitchCard& lea=PC.getLEA();

  const std::string PModel=IParam.getValue<std::string>("physModel");
  setPhysicsModel(lea,PModel);

  PC.setNPS(IParam.getValue<size_t>("nps"));
  PC.setRND(IParam.getValue<long int>("random"));	
  PC.setVoidCard(IParam.flag("void"));
  // Default:   10 20 40 50 110 120"
  PC.setPrintNum(IParam.getValue<std::string>("printTable"));

  // If Reactor stuff set and void
  if (IParam.hasKey("kcode") && IParam.dataCnt("kcode"))
    {
      setReactorPhysics(System,IParam);
      return;
    }

  if (IParam.hasKey("neutronOnly"))
    {
      setNeutronPhysics(System);
      return;
    }

  const FuncDataBase& Control=System.getDataBase();
  
  std::string PList=
    IParam.getDefValue<std::string>("h / d t s a z / * k ?","mode",0);
  if (PList=="empty" || PList=="Empty")
    {
      ELog::EM<<"WARNING:: plist empty"<<ELog::endWarn; 
      PC.addPhysCard<physicsSystem::PSimple>(std::string("mphys"),std::string(""));
      PList=" ";
    }
  
  const double maxEnergy=Control.EvalDefVar<double>("sdefEnergy",2000.0);
  const double cutUp=IParam.getValue<double>("cutWeight",0);  // [1keV
  const double cutMin=IParam.getValue<double>("cutWeight",1);  // [1keV def]
  const double cutTime=IParam.getDefValue<double>(1e8,"cutTime",0); 
  
  const double elcEnergy=IParam.getValue<double>("electron");
  const double phtEnergy=IParam.getValue<double>("photon");  // [1keV def]
  const double phtModel=IParam.getValue<double>("photonModel");
  const std::string elcAdd((elcEnergy>0 ? " e" : ""));

  if (std::abs(cutUp)<=std::abs(cutMin))
    throw ColErr::NumericalAbort
      ("CutUp<=cutMin: "+StrFunc::makeString(cutUp)+
       "<="+StrFunc::makeString(cutMin));
  
  PC.setMode("n p "+PList+elcAdd);
  System.processCellsImp();


  PC.setCells("imp",1,0);            // Set a zero cell	  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,cutTime,0.0,0.4,-0.1);
  
  physicsSystem::PStandard* allCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut",PList);
  allCut->setValues(4,cutTime,0.0,cutUp,cutMin);
  physicsSystem::PStandard* photonCut=
     PC.addPhysCard<physicsSystem::PStandard>("cut","p");
  photonCut->setValues(4,cutTime,phtEnergy,cutUp,cutMin);

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

  
  if (!PList.empty())
    {
      std::string::size_type hpos=PList.find("h");
      if (PList.find("h")!=std::string::npos)
        {
          physicsSystem::PStandard* ph=
            PC.addPhysCard<physicsSystem::PStandard>("phys","h");
          ph->setValues(EMax);
          PList.erase(hpos,1);
        }
      physicsSystem::PStandard* pa=
        PC.addPhysCard<physicsSystem::PStandard>("phys",PList+elcAdd);
      pa->setValues(EMax);
    }

  
  return; 
}


void 
setNeutronPhysics(Simulation& System)
  /*!
    Set the neutron Physics
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("DefPhysics","setNeutronPhysics");

  const FuncDataBase& Control=System.getDataBase();
  
  const double maxEnergy=Control.EvalDefVar<double>("sdefEnergy",2000.0);
  const std::string EMax=StrFunc::makeString(maxEnergy);
  
  physicsSystem::PhysicsCards& PC=System.getPC();
  PC.setMode("n");
  PC.setPrintNum("10 20 50 110 120");
  System.processCellsImp();
  PC.setCells("imp",1,0);            // Set a zero cell
  
  physicsSystem::PStandard* NCut=
    PC.addPhysCard<physicsSystem::PStandard>("cut","n");
  NCut->setValues(4,1.0e+8,0.0,0.4,-0.1);
  // Process physics
  physicsSystem::PStandard* pn=
	PC.addPhysCard<physicsSystem::PStandard>("phys","n");
  pn->setValues(EMax+" 0.0 j j j");
  
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

  physicsSystem::PhysicsCards& PC=System.getPC();
  
  std::string PList("");
  const double maxEnergy=Control.EvalDefVar<double>("sdefEnergy",20.0);
  const double elcEnergy=IParam.getValue<double>("electron");
  const double phtEnergy=IParam.getValue<double>("photon");
  const double phtModel=IParam.getValue<double>("photonModel");
  const std::string elcAdd((elcEnergy>0 ? " e" : ""));

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
  
  return; 
}

} // NAMESPACE ModelSupport
