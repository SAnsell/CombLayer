/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuild/TargetBase.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ProtonVoid.h"
#include "BeamWindow.h"
#include "TargetBase.h"


namespace TMRSystem
{
  
TargetBase::TargetBase(const std::string& Key,const size_t NLink)  : 
  attachSystem::FixedRotate(Key,NLink),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  PLine(new ts1System::ProtonVoid("ProtonVoid"))
  /*!
    Constructor
    \param Key :: Keyname 
    \param NLink :: Link number
  */
{
   ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(PLine);
}

TargetBase::TargetBase(const TargetBase& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  BWPtr((A.BWPtr) ? new ts1System::BeamWindow(*A.BWPtr) : nullptr),
  PLine(new ts1System::ProtonVoid(*A.PLine))
  /*!
    Copy constructor
    \param A :: TargetBase to copy
  */
{}

TargetBase&
TargetBase::operator=(const TargetBase& A)
  /*!
    Assignment operator
    \param A :: TargetBase to copy
    \return *this
  */
{

  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      
      if (A.BWPtr) 
	*BWPtr = *A.BWPtr;
      else
	BWPtr=A.BWPtr;

      *PLine = *A.PLine;
    }
  return *this;
}


void
TargetBase::createBeamWindow(Simulation& System,
			     const long int sideIndex)
  /*!
    Create the beamwindow if present
    \param System :: Simulation to build into
    \param sideIndex :: link poin in this to use
  */
{
  ELog::RegMethod RegA("TargetBase","createBeamWindow");
  if (PLine->hasCell("VoidCell"))
    {
      ModelSupport::objectRegister& OR=
	ModelSupport::objectRegister::Instance();
      
      if (!BWPtr)
	{
	  BWPtr=std::shared_ptr<ts1System::BeamWindow>
	  (new ts1System::BeamWindow("BWindow"));
	  OR.addObject(BWPtr);
	}
      BWPtr->copyCutSurf("Boundary",*PLine,"Boundary");
      BWPtr->setInsertCell(PLine->getCell("VoidCell"));
      BWPtr->createAll(System,*this,sideIndex);
    }
  return;
}

  
void
TargetBase::addProtonLineInsertCell(const int CN)
  /*!
    Adds an insert cell to the protonLine object
    \param CN :: Cell number
   */
{
  PLine->addInsertCell(CN);
  return;
}

void
TargetBase::addProtonLineInsertCell(const std::vector<int>& cellVec)
  /*!
    Adds an insert cell to the protonLine object
    \param cellVec :: Cell numbers
   */
{
  ELog::RegMethod RegA("TargetBase","addProtonLineInsertCell(vec)");

  std::for_each(cellVec.begin(),cellVec.end(),
		[this](int cv)
		{
		  this->PLine->addInsertCell(cv);
		});
  return;
}

}  // NAMESPACE TMRSystem
