/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/mainJobs.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "SimMCNP.h"
#include "TallyCreate.h"
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "meshConstruct.h"
#include "tmeshTally.h"
#include "MatMD5.h"
#include "MD5sum.h"
#include "Visit.h"

#include "mainJobs.h"


bool
getTallyMesh(const Simulation* SimPtr,
	     Geometry::Vec3D& MeshA,
	     Geometry::Vec3D& MeshB,
	     std::array<size_t,3>& MPts)
  /*!
    Get a mesh from a tally 
    \param SimPtr :: Simulation
    \param MeshA :: Lower bound point
    \param MeshB :: Upper bound point
    \param MPts :: Points 
    \return true if point found
  */
{
  ELog::RegMethod RegA("mainJobs[F]","getTallyMesh");

  const SimMCNP* SimMCPtr=
    dynamic_cast<const SimMCNP*>(SimPtr);

  if (SimMCPtr)
    {
      const tallySystem::tmeshTally* MPtr;
      MPtr=dynamic_cast<const tallySystem::tmeshTally*>(SimMCPtr->getTally(1));

      // try tally 3 if tally 1 fails
      if (!MPtr)
	{
	  MPtr=dynamic_cast<const tallySystem::tmeshTally*>
	    (SimMCPtr->getTally(3));
	}
      // success!
      if (MPtr) 
	{
	  MeshA=MPtr->getMinPt();
	  MeshB=MPtr->getMaxPt();
	  MPts=MPtr->getNPt();
	  return 1;
	}
    }

  
  // failed
  return 0;
}
	     
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

  const SimMCNP* SimMCPtr=dynamic_cast<const SimMCNP*>(SimPtr);
  if (IParam.flag("vtk"))
    {
      const std::string vForm=
	IParam.getDefValue<std::string>("","vtk",0);

      std::array<size_t,3> MPts;
      Geometry::Vec3D MeshA;
      Geometry::Vec3D MeshB;
      
      if (IParam.flag("vtkMesh"))
	{
	  const std::string PType=
	    IParam.getValueError<std::string>("vtkMesh",0,0,"object/free");
	  if (PType=="object")
	      tallySystem::meshConstruct::getObjectMesh
		(IParam,"vtkMesh",0,1,MeshA,MeshB,MPts);
	  else if (PType=="free")
	      tallySystem::meshConstruct::getFreeMesh
		(IParam,"vtkMesh",0,1,MeshA,MeshB,MPts);
	  else
	    {
	      tallySystem::meshConstruct::getFreeMesh
	      (IParam,"vtkMesh",0,1,MeshA,MeshB,MPts);
	    }
	}
      else if (!getTallyMesh(SimPtr,MeshA,MeshB,MPts))
	{
	  ELog::EM<<"No (tally) mesh for vtk"<<ELog::endErr;
	  return 0;
	}

      ELog::EM<<"Processing VTK:"<<ELog::endBasic;
      Visit VTK;

      const std::string vType=
	IParam.getDefValue<std::string>("","vtkType",0);
      if (vType=="cell")
	VTK.setType(Visit::VISITenum::cellID);
      else
	VTK.setType(Visit::VISITenum::material);

      if (vForm=="line")
	VTK.setLineForm();
      
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
      VTK.populate(*SimPtr,Active);
      
      VTK.writeVTK(Oname);
      return 2;
    }

  return 0;
}
