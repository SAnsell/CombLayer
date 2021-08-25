/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/SimInput.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <array>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "LineTrack.h"
#include "SimValid.h"
#include "LinkUnit.h"
#include "surfRegister.h"
#include "FixedComp.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "SimInput.h"

namespace SimProcess
{

int
processExitChecks(Simulation& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Check the validity of the simulation
    \param System :: Simuation object 
    \param IParam :: Inpute parameters
    \return error code for exit
  */
{
  ELog::RegMethod RegA("SimInput[F]","processExitChecks");

  System.populateCells();
  System.createObjSurfMap();

  if (IParam.flag("noThermal"))
    {
      ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
      DB.removeAllThermal();
      // avoids the game of renaming H1 to H4 so we have both ortho/para
      ELog::EM<<"Warning : ALL thermal treatments removed"<<ELog::endWarn;  
    }


  int errFlag(0);
  if (IParam.flag("validCheck"))
    {
      ELog::EM<<"SIMVALID TRACK "<<ELog::endDiag;
      ELog::EM<<"-------------- "<<ELog::endDiag;
      ModelSupport::SimValid SValidCheck;

      if (IParam.flag("validPoint"))
	{
	  const Geometry::Vec3D CPoint=
	    IParam.getValue<Geometry::Vec3D>("validPoint");

	  if (!SValidCheck.runPoint(System,CPoint,
				    IParam.getValue<size_t>("validCheck")))
	    errFlag += -1;
	}
      else if (IParam.flag("validFC"))
	{
	  const std::string FCObject=
	    IParam.getValueError<std::string>("validFC",0,0,"No FC-object");
	  const std::string linkPos=
	    IParam.getValueError<std::string>("validFC",0,1,"No FC-link Pos");
	  
	  const attachSystem::FixedComp* FC=
	    System.getObjectThrow<attachSystem::FixedComp>
	    (FCObject,"FixedComp");
	  
          const long int sideIndex=FC->getSideIndex(linkPos);
	  const Geometry::Vec3D CPoint=FC->getLinkPt(sideIndex)+
	    Geometry::Vec3D(0.001,0.001,0.001);
	  ELog::EM<<"Validation point "<<CPoint<<ELog::endDiag;
	  ELog::EM<<"NEEDS TO BE RE-WRITTEN SO WORKS STARTING"
	    " ON A SURFACE"<<ELog::endCrit;

	  if (!SValidCheck.runPoint
	      (System,CPoint,IParam.getValue<size_t>("validCheck")))
	    errFlag += -1;
	}
      else if (IParam.flag("validAll"))
	{
	  // This should work BUT never does
	  const size_t NPts=IParam.getValue<size_t>("validCheck");
	  typedef objectGroups::cMapTYPE CM;
	  const CM& mapFC=System.getComponents();
	  for(const CM::value_type& mc : mapFC)
	    {
	      const attachSystem::FixedComp& FC = *(mc.second);
	      const Geometry::Vec3D& CP=FC.getCentre();
	      if (SValidCheck.runPoint(System,CP,NPts))
		  errFlag += -1;
	    }
	}
      else 
	{
	  if (!SValidCheck.runPoint(System,Geometry::Vec3D(0,0,0),
				    IParam.getValue<size_t>("validCheck")))
	    errFlag += -1;
	}

    }

  const size_t NLine = IParam.setCnt("validLine");
  for(size_t i=0;i<NLine;i++)
    {
      ELog::EM<<"Processing "<<i<<" / "<<NLine<<ELog::endDiag;
      size_t cnt(0);
      const Geometry::Vec3D C=
	IParam.getCntVec3D("validLine",i,cnt,"Start point");
      Geometry::Vec3D D=
	IParam.getCntVec3D("validLine",i,cnt,"End point/Direction");
      if (D.abs()>1.5 || D.abs()<0.5)
	D-=C;
      D.makeUnit();
      ELog::EM<<std::setprecision(12)<<"C == "<<C<<":"<<D<<ELog::endDiag;
      ModelSupport::LineTrack LT(C,D,10000.0);
      ModelSupport::LineTrack LTR(C,-D,10000.0);
      LT.calculate(System);
      //      LTR.calculate(System);
      ELog::EM<<std::setprecision(12)<<"Line == "<<LT<<ELog::endDiag;
    }
  
  if (IParam.flag("cinder")) System.writeCinder();

  return errFlag;
}

void
inputProcessForSim(Simulation& System,
		   const mainSystem::inputParam& IParam)
  /*!
    Check the validity of the simulation
    \param System :: Simuation object 
    \param IParam :: Inpute parameters
  */
{
  ELog::RegMethod RegA("SimInput[F]","inputProcessForSim");

  // Cut energy tallies:
  if (IParam.flag("ECut"))
    System.setEnergy(IParam.getValue<double>("ECut"));

  return;
}

void
inputProcessForSimMCNP(SimMCNP& System,
		       const mainSystem::inputParam& IParam)
  /*!
    Check the validity of the simulation
    \param System :: Simuation object 
    \param IParam :: Inpute parameters
  */
{
  ELog::RegMethod RegA("SimInput[F]","inputProcessForSimMCNP");

  inputProcessForSim(System,IParam);
  
  if (IParam.flag("cinder"))
    System.setForCinder();
  
  if (IParam.flag("endf"))
    System.setENDF7();

  if (IParam.flag("ptrac"))
    SimProcess::processPTrack(IParam,System.getPC());

  if (IParam.flag("event"))
    SimProcess::processEvent("event",IParam,System.getPC());

  if (IParam.flag("dbcn"))
    SimProcess::processEvent("dbcn",IParam,System.getPC());
  
  return;
}

void
processPTrack(const mainSystem::inputParam& IParam,
              physicsSystem::PhysicsCards& PCard)
  /*!
    Process the input
    \param IParam :: Input deck
    \param PCard :: Physics card
   */
{
  ELog::RegMethod RegA("SimInput[F]","processPTrack");

  const size_t NItems=IParam.itemCnt("ptrac",0);
  if (NItems && IParam.getValue<std::string>("ptrac",0,0)=="help")
    {
      PCard.writeHelp("ptrac");
      return;
    }
  
  for(size_t index=1;index<NItems;index+=2)
    {
      const std::string key=IParam.getValue<std::string>("ptrac",0,index-1);
      const std::string Val=IParam.getValue<std::string>("ptrac",0,index);
      PCard.setPTRAC(key,Val);
    }
  PCard.setPTRACactive(1);
  return;
}

void
processEvent(const std::string& typeName,
             const mainSystem::inputParam& IParam,
             physicsSystem::PhysicsCards& PCard)
  /*!
    Process the input
    \param typeName :: name [event/dbcn]
    \param IParam :: Input deck
    \param PCard :: Physics card
  */
{
  ELog::RegMethod RegA("SimInput[F]","processEvent");

  const size_t NItems=IParam.itemCnt(typeName,0);
  if (NItems && IParam.getValue<std::string>(typeName,0,0)=="help")
    {
      PCard.writeHelp(typeName);
      return;
    }
  
  for(size_t index=1;index<NItems;index+=2)
    {
      const std::string key=IParam.getValue<std::string>(typeName,0,index-1);
      const std::string Val=IParam.getValue<std::string>(typeName,0,index);
      PCard.setDBCN(key,Val);
    }
  PCard.setDBCNactive(1);
  return;
}

  
}  // NAMESPACE SimProcess
