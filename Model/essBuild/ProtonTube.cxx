/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/ProtonTube.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "AttachSupport.h"

#include "PBW.h"
#include "TelescopicPipe.h"
#include "ProtonTube.h"
#include "World.h"

namespace essSystem
{

ProtonTube::ProtonTube(const std::string& Key) :
  TelescopicPipe(Key),
  pbw(new PBW(Key+"PBW"))
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("ProtonTube","ProtonTube(const std::string&)");
  
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(pbw);
  
  return;
}

ProtonTube::ProtonTube(const ProtonTube& A) :
  TelescopicPipe(A),
  engActive(A.engActive),
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
      TelescopicPipe::operator=(A);
      engActive=A.engActive;
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

    // Master values
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  return;
}

void
ProtonTube::createAll(Simulation& System,
		      const attachSystem::FixedComp& TargetFC,const long int tIndex,
		      const attachSystem::FixedComp& BulkFC,const long int bIndex,
		      const attachSystem::FixedComp& SB,const long int sbIndex,
		      const attachSystem::FixedComp& Bulk)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param TargetFC :: FixedComp for origin and target outer surf (tube start)
    \param tIndex :: Target plate surface [signed]
    \param BulkFC :: FixedComp for tube end (not used if next arg is 0)
    \param bIndex :: Tube end link point (not used if 0 - then the tube ends at its max length)
    \param SB :: FixedComp for Monolith Shielding (shutter bay object)
    \param sbIndex :: ShutterBay roof link point
    \param Bulk :: Bulk object (to remove BeamMonitor from)						
  */
{
  ELog::RegMethod RegA("ProtonTube","createAll");

  TelescopicPipe::createAll(System,TargetFC,tIndex,BulkFC,bIndex,SB);
  
  //if (engActive)
    {
      pbw->createAll(System, World::masterOrigin(), 0, SB,sbIndex);
      attachSystem::addToInsertSurfCtrl(System,SB,*pbw);
      attachSystem::addToInsertLineCtrl(System,*this, *pbw);
    }


  return;
}

}  // NAMESPACE essSystem
