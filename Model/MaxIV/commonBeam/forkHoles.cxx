/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/forkHoles.cxx
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
#include <sstream>
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"

#include "PortChicane.h"
#include "forkHoles.h"


namespace xraySystem
{

forkHoles::forkHoles(const std::string& Key) :
  masterName(Key),nForks(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

forkHoles::~forkHoles()
  /*!
    Destructor
  */
{}

void
forkHoles::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("forkHoles","populate");
  // Fork holes
  nForks=Control.EvalDefVar<size_t>(masterName+"NHoles",0);

  forkWall="None";
  if (nForks)
    {
      forkWall=Control.EvalDefVar<std::string>
	(masterName+"ForkWall","Back");
      if (forkWall!="Back" && forkWall!="Outer" && forkWall!="Ring")
	ELog::EM<<"ForkWall : "<<forkWall<<ELog::endErr;

      ELog::EM<<"Found "<<forkWall<<ELog::endDiag;
      forkXStep=Control.EvalDefVar<double>(masterName+"XStep",0.0);
      forkYStep=Control.EvalDefVar<double>(masterName+"YStep",0.0);
      forkLength=Control.EvalDefVar<double>(masterName+"Length",60.0);
      forkHeight=Control.EvalDefVar<double>(masterName+"Height",10.0);
      for(size_t i=0;i<nForks;i++)
	fZStep.push_back(Control.EvalVar<double>
			 (masterName+"ZStep"+std::to_string(i)));
    }
  return;
}

void
forkHoles::createSurfaces(ModelSupport::surfRegister& SMap,
			  const attachSystem::FixedComp& FC,
			  const int buildIndex)
  /*!
    Create fork surfaces if needed
    \param SMap :: Surface from container class 
    \param FC :: FixedComp for basis set
    \param buildIndex :: offset number [typically buildIndex +3000]
  */
{
  ELog::RegMethod RegA("forkHoles","createForkSurfaces");
  
  if(!fZStep.empty())
    {
      const Geometry::Vec3D Origin=FC.getCentre();
      const Geometry::Vec3D X=FC.getX();
      const Geometry::Vec3D Y=FC.getY();
      const Geometry::Vec3D Z=FC.getZ();
      
      if (forkWall=="Back")
	{
	  ModelSupport::buildPlane
	    (SMap,buildIndex+3,Origin+X*(forkXStep-forkLength/2),X);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+4,Origin+X*(forkXStep+forkLength/2),X);
	  forkWallBack=ModelSupport::getHeadRule
	    (SMap,buildIndex,"(-3:4)");
	}
      else if (forkWall=="Outer" || forkWall=="Ring")
	{
	  ModelSupport::buildPlane
	    (SMap,buildIndex+3001,Origin+Y*(forkYStep-forkLength/2),Y);
	  ModelSupport::buildPlane
	    (SMap,buildIndex+3002,Origin+Y*(forkYStep+forkLength/2),Y);
	  const HeadRule HR=ModelSupport::getHeadRule
	       (SMap,buildIndex,"(-1:2)");
	  if (forkWall=="Outer")
	    forkWallOuter=HR;
	  else
	    forkWallRing=HR;
	}
	
      int BI(buildIndex);
      for(size_t i=0;i<fZStep.size();i++)
	{
	  ModelSupport::buildPlane
	    (SMap,BI+5,Origin+Z*(fZStep[i]-forkHeight/2.0),Z);
	  ModelSupport::buildPlane
	    (SMap,BI+6,Origin+Z*(fZStep[i]+forkHeight/2.0),Z);
	  BI+=10;
	}
    }
  return;
}

}  // NAMESPACE xraySystem
