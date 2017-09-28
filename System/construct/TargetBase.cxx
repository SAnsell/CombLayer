/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TargetBase.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "generateSurf.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "World.h"
#include "ProtonVoid.h"
#include "BeamWindow.h"
#include "TargetBase.h"


namespace constructSystem
{
  
TargetBase::TargetBase(const std::string& Key,const size_t NLink)  : 
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,NLink),
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
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),  
  BWPtr((A.BWPtr) ? new ts1System::BeamWindow(*A.BWPtr) : 0),
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
      attachSystem::FixedOffset::operator=(A);
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
  */
{
  ELog::RegMethod RegA("TargetBase","createBeamWindow");
  if (PLine->getVoidCell())
    {
      ModelSupport::objectRegister& OR=
	ModelSupport::objectRegister::Instance();
      
      if (!BWPtr)
	{
	  BWPtr=std::shared_ptr<ts1System::BeamWindow>
	  (new ts1System::BeamWindow("BWindow"));
	  OR.addObject(BWPtr);
	}      
      BWPtr->addBoundarySurf(PLine->getCompContainer());
      BWPtr->setInsertCell(PLine->getVoidCell());
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

std::vector<int>  
TargetBase::getInnerCells() const
  /*!
    Return those cells that consititue the inner cells
    and can be modified
    \return vector of cell numbers
  */
{
  return std::vector<int>();
}

}  // NAMESPACE constructSystem
