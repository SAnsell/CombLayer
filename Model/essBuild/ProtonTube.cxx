/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/ProtonTube.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "AttachSupport.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"

#include "PBW.h"
#include "TelescopicPipe.h"
#include "ProtonTube.h"
#include "World.h"

namespace essSystem
{

ProtonTube::ProtonTube(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedGroup(),
  attachSystem::FixedRotateUnit(newName,2),
  attachSystem::FrontBackCut(),
  tube(new TelescopicPipe(newName+"Pipe")),
  pbw(new PBW(newName+"PBW"))
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("ProtonTube","ProtonTube(const std::string&)");
  
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(tube);
  OR.addObject(pbw);
  
  return;
}

ProtonTube::ProtonTube(const ProtonTube& A) :
  attachSystem::CopiedComp(A),
  attachSystem::ContainedGroup(A),
  attachSystem::FixedRotateUnit(A),
  attachSystem::FrontBackCut(A),
  engActive(A.engActive),
  tube(A.tube->clone()),
  pbw(A.pbw->clone())
  /*!
    Copy constructor
    \param A :: ProtonTube to copy
  */
{}

ProtonTube&
ProtonTube::operator=(const ProtonTube& A)
  /*!
    Assignment operator
    \param A :: ProtonTube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      engActive=A.engActive;
      *tube=*A.tube;
      *pbw=*A.pbw;
    }
  return *this;
}

ProtonTube::~ProtonTube()
  /*!
    Destructor
   */
{}

void
ProtonTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("ProtonTube","populate");
  engActive=Control.EvalTail<int>(keyName,"","EngineeringActive");

  return;
}

void
ProtonTube::createAll(Simulation& System,
		      const attachSystem::FixedComp& originFC,
		      const long int originIndex,
		      const attachSystem::FixedComp& SB,
		      const long int sbIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param originFC :: FixedComp for origin
    \param originIndex :: Target plate surface [signed]
    \param SB :: FixedComp for Monolith Shielding (shutter bay object)
    \param sbIndex :: ShutterBay roof link point
  */
{
  ELog::RegMethod RegA("ProtonTube","createAll");

  tube->setFront(frontRule());
  tube->setBack(backRule());
  tube->createAll(System,originFC,originIndex);

  attachSystem::ContainedGroup::operator=(*tube);

  populate(System.getDataBase());
  
  if (engActive)
    {
      pbw->createAll(System, World::masterOrigin(), 0, SB,sbIndex);
      attachSystem::addToInsertSurfCtrl(System,SB,*pbw);
      attachSystem::addToInsertLineCtrl(System,*tube, *pbw);
    }

  return;
}

}  // NAMESPACE essSystem
