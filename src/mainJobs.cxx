/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/mainJobs.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "TallyCreate.h"
#include "MeshCreate.h"
#include "MatMD5.h"
#include "MD5sum.h"
#include "Visit.h"
#include "mainJobs.h"

int
createMeshTally(const mainSystem::inputParam& IParam,
		Simulation* SimPtr)
  /*!
    Create Mesh tallies
    \param IParam :: Inpup parameters
    \param SimPtr :: Simulation
    \retval +ve : successfull completion 
    \retval -ve : Error of attempted completion
    \retval 0 : No action
  */

{

  // MESH (other tally removal)
  if (IParam.flag("mesh"))
    {
      ELog::EM<<"Removing point tallies:"<<ELog::endDiag;
      tallySystem::deleteTallyType(*SimPtr,5);
      if (IParam.flag("MA") && IParam.flag("MB") && IParam.flag("MN"))
	{
	  int MPTS[3]={
	    IParam.getValue<int>("meshNPS",0),
	    IParam.getValue<int>("meshNPS",1),
	    IParam.getValue<int>("meshNPS",2) };
	  
	  tallySystem::rectangleMesh(*SimPtr,1,
				     IParam.getValue<std::string>("doseCalc"),
				     IParam.getValue<Geometry::Vec3D>("meshA"),
				     IParam.getValue<Geometry::Vec3D>("meshB"),
				     MPTS);
	}

      return 1;
    }	  
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

  if (IParam.flag("md5") || IParam.flag("vtk"))
    {
      if (!IParam.flag("meshA") || !IParam.flag("meshB") || !IParam.flag("MN"))
	{
	  ELog::EM<<"Failed to process VTK/MD5 since mesh and nps "
		  <<"not definded"<<ELog::endErr;
	  return -1;
	}
      size_t meshPts[3]={
	IParam.getValue<size_t>("meshNPS",0),
	IParam.getValue<size_t>("meshNPS",1),
	IParam.getValue<size_t>("meshNPS",2) };
      Geometry::Vec3D MeshA=IParam.getValue<Geometry::Vec3D>("meshA");
      Geometry::Vec3D MeshB=IParam.getValue<Geometry::Vec3D>("meshB");
      if (IParam.flag("md5"))
	{
	  ELog::EM<<"Processing MD5:"<<ELog::endBasic;
	  MD5sum MM(60);
	  MM.setBox(MeshA,MeshB);
	  MM.setIndex(meshPts[0],meshPts[1],meshPts[2]);
	  MM.populate(SimPtr);
	  std::cout<<"MM == "<<MM<<std::endl;
	  return 1;
	}

      if (IParam.flag("vtk"))
	{
	  ELog::EM<<"Processing VTK:"<<ELog::endBasic;
	  Visit VTK;

	  VTK.setType(Visit::material);
	  std::set<std::string> Active;
	  for(size_t i=0;i<15;i++)
	    {
	      const std::string Item=IParam.getValue<std::string>("vmat",i);
	      if (!Item.empty())
		Active.insert(Item);
	      else
		i=15;
	    }
	  if (IParam.flag("vcell"))
	    VTK.setType(Visit::cellID);

	  // PROCESS VTK:
	  VTK.setBox(MeshA,MeshB);
	  VTK.setIndex(meshPts[0],meshPts[1],meshPts[2]);
	  VTK.populate(SimPtr,Active);
	  
	  VTK.writeVTK(Oname);
	  return 2;
	}
    }
  return 0;
}
