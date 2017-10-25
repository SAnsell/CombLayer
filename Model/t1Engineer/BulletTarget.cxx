/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Engineer/BulletTarget.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Goran Skoro
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
#include "ProtonVoid.h"
#include "BeamWindow.h"
#include "TargetBase.h"
#include "BulletVessel.h"
#include "BulletPlates.h"
#include "BulletDivider.h"
#include "BulletTarget.h"

namespace ts1System
{

BulletTarget::BulletTarget(const std::string& Key)  :
  TargetBase(Key,12),
  tIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  PlateTarObj(new BulletPlates("BPlates")),
  PressVObj(new BulletVessel("BVessel"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
   */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  DObj.push_back(std::shared_ptr<BulletDivider>(new BulletDivider("BDiv",0,1)));
  OR.addObject(DObj.back());
  DObj.push_back(std::shared_ptr<BulletDivider>(new BulletDivider("BDiv",1,0)));
  OR.addObject(DObj.back());
  //  DObj.push_back(new BulletDivider("BDiv",1))

  
  OR.addObject(PressVObj);
  OR.addObject(PlateTarObj);
}

BulletTarget::BulletTarget(const BulletTarget& A) : 
  constructSystem::TargetBase(A),
  tIndex(A.tIndex),PlateTarObj(A.PlateTarObj),
  PressVObj(A.PressVObj)
  /*!
    Copy constructor
    \param A :: BulletTarget to copy
  */
{}

BulletTarget&
BulletTarget::operator=(const BulletTarget& A)
  /*!
    Assignment operator
    \param A :: BulletTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::TargetBase::operator=(A);
      PlateTarObj=A.PlateTarObj;
      PressVObj=A.PressVObj;
    }
  return *this;
}


BulletTarget*
BulletTarget::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new BulletTarget(*this);
}

BulletTarget::~BulletTarget() 
 /*!
   Destructor
 */
{}
  
void
BulletTarget::addProtonLine(Simulation& ,
			     const attachSystem::FixedComp& ,
			     const long int )
  /*!
    Add a proton void cell
    \param  :: Simualation
    \param  :: reflector
    \param  :: Index of line
   */
{
  ELog::RegMethod RegA("BulletTarget","addProtonLine");

  //  PLine->createAll(System,*PressVObj,-7,refFC,index);
  //  createBeamWindow(System,7);
  
  return;
}

void
BulletTarget::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp for origin [World Origin]
  */
{
  ELog::RegMethod RegA("BulletTarget","createAll");

  PressVObj->setInsertCell(this->getInsertCells());
  PressVObj->createAll(System,FC);

  PlateTarObj->setInsertCell(PressVObj->getInnerCells());
  PlateTarObj->createAll(System,FC,0);

  for(std::shared_ptr<BulletDivider> DPtr : DObj)
    {
      DPtr->setInsertCell(PressVObj->getInnerCells());
      DPtr->createAll(System,*PlateTarObj,*PressVObj);
    }
  return;
}
 

  
}  // NAMESPACE ts1System
