/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/pointDetOpt.cxx
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "BnId.h"
#include "Rules.h"
#include "neutron.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysicsCards.h"
#include "Simulation.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "pointDetOpt.h"

#include "debugMethod.h"

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
  Simulation::OTYPE::const_iterator vc;
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      if (!vc->second->isPlaceHold())
	{
	  OA.addUnit(ASim,vc->first,vc->second->getCofM());
	}

    }
  return;
}

void
pointDetOpt::addTallyOpt(const int tallyN,Simulation& ASim)
  /*!
    Adds an importance card to the physics of type PD
    with the corresponding weights for the distance
    \param tallyN :: tally nubmer
    \param ASim :: Simulation to add component to
  */
{
  ELog::RegMethod RegA("pointDetOpt","addTallyOpt");

  // First get the physcis
  physicsSystem::PhysicsCards& PC=ASim.getPC();
  std::ostringstream cx;
  cx<<"pd"<<tallyN;
  physicsSystem::PhysImp& PD=PC.addPhysImp(cx.str(),"");

  const Simulation::OTYPE& OCells=ASim.getCells(); 
  Simulation::OTYPE::const_iterator mc;

  // First loop to find minimum distance:
  double minV(1e38);
  double D;
  for(mc=OCells.begin();mc!=OCells.end();mc++)
    {
      if (!mc->second->isPlaceHold())
	{
	  D=OA.getMatSum(mc->first);
	  if (D<minV && D>0.0)
	    {
	      minV=D;
	      if (minV<1.0)
		break;
	    }
	}
    }

  // Second loop to create Pd values [in an importance card]
  const double scale((minV<1.0) ? 1.0 : minV);
  for(mc=OCells.begin();mc!=OCells.end();mc++)
    {
      if (!mc->second->isPlaceHold())
	{
	  const double D=OA.getMatSum(mc->first);
	  if (D>1.0)
	    PD.setValue(mc->first,scale/D);
	  else
	    PD.setValue(mc->first,1.0);
	}
    }
  return;
}

} // Namespace ModelSupport
