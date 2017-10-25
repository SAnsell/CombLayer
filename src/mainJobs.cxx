/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/mainJobs.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Quaternion.h"
#include "Triple.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "TallyCreate.h"
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "tmeshTally.h"
#include "MatMD5.h"
#include "MD5sum.h"
#include "Visit.h"
#include "mainJobs.h"


int
createVTK(const mainSystem::inputParam& IParam,
	  const Simulation* SimPtr,
	  const std::string& Oname)
  /*!
    Run the VTK box
    \param IParam :: Inpup parameters
    \param SimPtr :: Simulation
    \param Oname :: Output name
    \retval +ve : successfull completion 
    \retval -ve : Error of attempted completion
    \retval 0 : No action
  */
{
  ELog::RegMethod RegA("createVTK","createVTK");

  if (IParam.flag("md5") || IParam.flag("vtk"))
    {
      const tallySystem::tmeshTally* MPtr=
	dynamic_cast<const tallySystem::tmeshTally*>(SimPtr->getTally(1));
      if (!MPtr)
	MPtr=dynamic_cast<const tallySystem::tmeshTally*>(SimPtr->getTally(3));
      if (!MPtr)
	{
	  ELog::EM<<"Tally == "<<SimPtr->getTally(1)<<ELog::endCrit;
	  ELog::EM<<"No mesh tally for Contruction of VTK mesh:"<<ELog::endErr;
	  
	  return -1;
	}

      const std::array<size_t,3>& MPts=MPtr->getNPt();
      const Geometry::Vec3D& MeshA=MPtr->getMinPt();
      const Geometry::Vec3D& MeshB=MPtr->getMaxPt();
      
      if (IParam.flag("md5"))
	{
	  ELog::EM<<"Processing MD5:"<<ELog::endBasic;
	  MD5sum MM(60);
	  MM.setBox(MeshA,MeshB);
	  MM.setIndex(MPts[0],MPts[1],MPts[2]);
	  MM.populate(SimPtr);
	  return 1;
	}

      if (IParam.flag("vtk"))
	{
	  ELog::EM<<"Processing VTK:"<<ELog::endBasic;
	  Visit VTK;

	  if (IParam.flag("vcell"))
	    VTK.setType(Visit::VISITenum::cellID);
	  else
	    VTK.setType(Visit::VISITenum::material);
	  
	  std::set<std::string> Active;
	  for(size_t i=0;i<15;i++)
	    {
	      const std::string Item=IParam.getValue<std::string>("vmat",i);
	      if (Item.empty()) break;
	      Active.insert(Item);
	    }

	  // PROCESS VTK:
	  VTK.setBox(MeshA,MeshB);
	  VTK.setIndex(MPts[0],MPts[1],MPts[2]);
	  VTK.populate(SimPtr,Active);
	  
	  VTK.writeVTK(Oname);
	  return 2;
	}
    }
  return 0;
}
