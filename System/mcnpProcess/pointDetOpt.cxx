/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/pointDetOpt.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <list>
#include <vector>
#include <set> 
#include <map> 
#include <string>
#include <algorithm>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "Tally.h"
#include "pointTally.h"
#include "vertexCalc.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "PDControl.h"
#include "pointDetOpt.h"


namespace ModelSupport
{

pointDetOpt::pointDetOpt(const Geometry::Vec3D& Pt) : 
  energy(1.0),OA(Pt)
  /*!
    Constructor
    \param Pt :: target point
  */
{}

pointDetOpt::pointDetOpt(const pointDetOpt& A) : 
  energy(A.energy),OA(A.OA)
  /*!
    Copy Constructor
    \param A :: pointDetOpt to copy
  */
{}

pointDetOpt&
pointDetOpt::operator=(const pointDetOpt& A)   
  /*!
    Assignement operator
    \param A :: pointDetOpt to copy
    \return *this
  */
{
  if (this!=&A)
    {
      energy=A.energy;
      OA=A.OA;
    }
  return *this;
}

void
pointDetOpt::createObjAct(const Simulation& ASim) 
  /*!
    Create a set of vertex points
    -- Note: routine tested in testObjectTrackAct
    \param ASim : Simulation object
  */
{
  ELog::RegMethod RegA("pointDetOpt","createObjAct");

  const Simulation::OTYPE& Cells=ASim.getCells();
  for(const auto& [cellNum,objPtr] : Cells)
    {
      const Geometry::Vec3D CofM=
	ModelSupport::calcCOFM(*objPtr);
      OA.addUnit(ASim,cellNum,CofM);
    }
  return;
}

void
pointDetOpt::addTallyOpt(SimMCNP& System,const int tallyN)
			 
  /*!
    Adds an importance card to the physics of type PD
    with the corresponding weights for the distance
    \param System :: Simulation to add component to
    \param tallyN :: tally nubmer
  */
{
  ELog::RegMethod RegA("pointDetOpt","addTallyOpt");

  // First get ally
  tallySystem::pointTally* pointPTR=
    dynamic_cast<tallySystem::pointTally*>(System.getTally(tallyN));
  if (!pointPTR)
    throw ColErr::InContainerError<int>(tallyN,"Tally Number");
  
  // This should be in tally ??
  std::unique_ptr<tallySystem::PDControl> PDptr=
    std::make_unique<tallySystem::PDControl>(tallyN);
  
  // First loop to find minimum distance:
  double minV(1e38);
  double D;

  const Simulation::OTYPE& Cells=System.getCells();
  for(const auto& [cellNum,objPtr] : Cells)
    {
      (void) objPtr;
      D=OA.getMatSum(cellNum);
      if (D<minV && D>0.0)
	{
	  minV=D;
	  if (minV<1.0)
	    break;
	}
    }

  // Second loop to create Pd values [in an PhysImp]
  const double scale((minV<1.0) ? 1.0 : minV);
  for(const auto& [cellNum,objPtr] : Cells)
    {
      (void) objPtr;
      const double D=OA.getMatSum(cellNum);

      if (D>1.0)
	PDptr->setImp(cellNum,scale/D);
      else
	PDptr->setImp(cellNum,1.0);
    }

  // move assignment
  pointPTR->setPDControl(std::move(PDptr));

  return;
}

} // Namespace ModelSupport
