/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1PlateTarget.cxx
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "channel.h"
#include "PressVessel.h"
#include "PlateTarget.h"
#include "WaterDividers.h"
#include "ProtonVoid.h"
#include "BeamWindow.h"
#include "TargetBase.h"
#include "t1PlateTarget.h"


namespace ts1System
{

t1PlateTarget::t1PlateTarget(const std::string& Key)  :
  TargetBase(Key,12),
  tIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  PressVObj(new PressVessel("PVessel")),
  PlateTarObj(new PlateTarget("TPlate")),
  DivObj(new WaterDividers("WDivide"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
   */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(PressVObj);
  OR.addObject(PlateTarObj);
  OR.addObject(DivObj);
}

t1PlateTarget::t1PlateTarget(const t1PlateTarget& A) : 
  TargetBase(A),tIndex(A.tIndex),PressVObj(new PressVessel(*A.PressVObj)),
  PlateTarObj(new PlateTarget(*A.PlateTarObj)),
  DivObj(new WaterDividers(*A.DivObj))
  /*!
    Copy constructor
    \param A :: t1PlateTarget to copy
  */
{
  // if (A.BWPtr)
  //   BWPtr=std::shared_ptr<ts1System::BeamWindow>
  //     (new ts1System::BeamWindow(*A.BWPtr));
}

t1PlateTarget&
t1PlateTarget::operator=(const t1PlateTarget& A)
  /*!
    Assignment operator
    \param A :: t1PlateTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      TargetBase::operator=(A);
      *PressVObj = *A.PressVObj;
      *PlateTarObj = *A.PlateTarObj;
      *DivObj = *A.DivObj;
    }
  return *this;
}

t1PlateTarget*
t1PlateTarget::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new t1PlateTarget(*this);
}

t1PlateTarget::~t1PlateTarget() 
 /*!
   Destructor
 */
{}
  

void
t1PlateTarget::addProtonLine(Simulation& System,
			     const attachSystem::FixedComp& refFC,
			     const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector
    \param index :: Index of line
   */
{
  ELog::RegMethod RegA("t1PlateTarget","addProtonLine");

  PLine->createAll(System,*PressVObj,-7,refFC,index);
  createBeamWindow(System,7);
  
  return;
}

void
t1PlateTarget::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp for origin [World Origin]
  */
{
  ELog::RegMethod RegA("t1PlateTarget","createAll");

  PlateTarObj->populate(System);
  PressVObj->setTargetLength(PlateTarObj->getTargetLength());
  PressVObj->createAll(System,FC,0);

  FixedComp::copyLinkObjects(*PressVObj);
  ContainedComp::copyRules(*PressVObj);

  PlateTarObj->addInsertCell(PressVObj->getInnerVoid());
  PlateTarObj->addInsertCell(PressVObj->getOuterWall());
  PlateTarObj->createAll(System,*PressVObj);

  DivObj->addInsertCell(PressVObj->getInnerVoid());
  DivObj->createAll(System,*PlateTarObj,*PressVObj);

  insertObjects(System);
  PlateTarObj->buildFeedThrough(System);
  PressVObj->buildFeedThrough(System);

  return;
}
 

  
}  // NAMESPACE ts1System
