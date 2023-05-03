/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/mainJobs.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "Vec3D.h"
#include "inputParam.h"
#include "Triple.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimFLUKA.h"
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "meshConstruct.h"
#include "tmeshTally.h"
#include "dataSlice.h"
#include "multiData.h"
#include "Visit.h"
#include "FEM.h"



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
	  const std::string& OName)
  /*!
    Run the VTK box
    \param IParam :: Inpup parameters
    \param SimPtr :: Simulation
    \param OName :: Output name
    \retval +ve : successfull completion 
    \retval -ve : Error of attempted completion
    \retval 0 : No action
  */
{
  ELog::RegMethod RegA("mainJobs[F]","createVTK");

  const bool vFlag=IParam.flag("vtk");
  const bool fFlag=IParam.flag("fem");
  if (vFlag || fFlag)
    {
      const std::string tagName=(fFlag) ? "fem" : "vtk";
      const std::string meshName=(fFlag) ? "femMesh" : "vtkMesh";

      const std::string vForm=
	IParam.getDefValue<std::string>("",tagName,0);

      std::array<size_t,3> MPts;
      Geometry::Vec3D MeshA;
      Geometry::Vec3D MeshB;
      
      if (IParam.flag(meshName))
	{
	  size_t itemIndex(0);
	  MeshA=mainSystem::getNamedPoint
	    (*SimPtr,IParam,meshName,0,itemIndex,
	     "MeshA Point");
	  MeshB=mainSystem::getNamedPoint
	    (*SimPtr,IParam,meshName,0,itemIndex,
	     "MeshB Point");
	  
	  MPts[0]=IParam.getValueError<size_t>
	    (meshName,0,itemIndex++,"NXpts");
	  MPts[1]=IParam.getValueError<size_t>
	    (meshName,0,itemIndex++,"NYpts");
	  MPts[2]=IParam.getValueError<size_t>
	    (meshName,0,itemIndex++,"NZpts");
	}
      else if (!getTallyMesh(SimPtr,MeshA,MeshB,MPts))
	{
	  ELog::EM<<"No (tally) mesh for vtk/fem"<<ELog::endErr;
	  return 0;
	}

      if (fFlag)
	{
	  FEM femUnit;
	  if (vForm=="line")
	    femUnit.setLineForm(); 

	  femUnit.setBox(MeshA,MeshB);
	  femUnit.setIndex(MPts[0],MPts[1],MPts[2]);
	  femUnit.populate(*SimPtr);
	  femUnit.writeFEM(OName);
	}

      else
	{	
	  ELog::EM<<"Processing VTK:"<<ELog::endBasic;
	  Visit VTK;

	  const std::string vType=
	    IParam.getDefValue<std::string>("","vtkType",0);
	  if (vType=="cell" || vType=="Cell")
	    VTK.setType(Visit::VISITenum::cellID);
	  else if (vType=="imp" || vType=="IMP")
	    VTK.setType(Visit::VISITenum::imp);
	  else if (vType=="dens" || vType=="density")
	    VTK.setType(Visit::VISITenum::density);
	  else if (vType=="weight" || vType=="wwg")
	    VTK.setType(Visit::VISITenum::weight);
	  else if (vType.empty())
	    VTK.setType(Visit::VISITenum::material);
	  else 
	    throw ColErr::InContainerError<std::string>
	      (vType,"vtkType unknown");
	  
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
	  
	  if (vFlag)
	    {
	      ELog::EM<<"VTK Type == "<<vType<<ELog::endDiag;
	      if (vType=="cell" || vType=="Cell")
		VTK.writeIntegerVTK(OName);
	      else
		VTK.writeVTK(OName);
	      return 2;
	    }
	}
    }

  return 0;
}

int
createFEM(const mainSystem::inputParam& IParam,
	  const Simulation* SimPtr,
	  const std::string& Oname)
/*!
    Run the FEM box (similar to VTK)
    \param IParam :: Inpup parameters
    \param SimPtr :: Simulation
    \param Oname :: Output name
    \retval +ve : successfull completion 
    \retval -ve : Error of attempted completion
    \retval 0 : No action
  */
{
  ELog::RegMethod RegA("mainJobs[F]","createVTK");

  if (IParam.flag("fem"))
    {
      const std::string vForm=
	IParam.getDefValue<std::string>("","fem",0);

      std::array<size_t,3> MPts;
      Geometry::Vec3D MeshA;
      Geometry::Vec3D MeshB;
      
      if (IParam.flag("vtkMesh"))
	{
	  size_t itemIndex(0);
	  MeshA=mainSystem::getNamedPoint
	    (*SimPtr,IParam,"femMesh",0,itemIndex,
	     "MeshA Point");
	  MeshB=mainSystem::getNamedPoint
	    (*SimPtr,IParam,"femMesh",0,itemIndex,
	     "MeshB Point");
	  
	  MPts[0]=IParam.getValueError<size_t>
	    ("femMesh",0,itemIndex++,"NXpts");
	  MPts[1]=IParam.getValueError<size_t>
	    ("femMesh",0,itemIndex++,"NYpts");
	  MPts[2]=IParam.getValueError<size_t>
	    ("femMesh",0,itemIndex++,"NZpts");
	}
      else if (!getTallyMesh(SimPtr,MeshA,MeshB,MPts))
	{
	  ELog::EM<<"No (tally) mesh for fem"<<ELog::endErr;
	  return 0;
	}

      ELog::EM<<"Processing VTK:"<<ELog::endBasic;
      

      Visit VTK;

      const std::string vType=
	IParam.getDefValue<std::string>("","vtkType",0);
      if (vType=="cell" || vType=="Cell")
	VTK.setType(Visit::VISITenum::cellID);
      else if (vType=="imp" || vType=="IMP")
	VTK.setType(Visit::VISITenum::imp);
      else if (vType=="dens" || vType=="density")
	VTK.setType(Visit::VISITenum::density);
      else if (vType=="weight" || vType=="wwg")
	VTK.setType(Visit::VISITenum::weight);
      else if (vType.empty())
	VTK.setType(Visit::VISITenum::material);
      else 
	throw ColErr::InContainerError<std::string>(vType,"vtkType unknown");

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

      ELog::EM<<"VTK Type == "<<vType<<ELog::endDiag;
      if (vType=="cell" || vType=="Cell")
	VTK.writeIntegerVTK(Oname);
      else
	VTK.writeVTK(Oname);
      return 2;
    }

  return 0;
}
