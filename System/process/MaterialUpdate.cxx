/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/Materialupdate.cxx
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
#include <iterator>
#include <memory>

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
#include "support.h"
#include "surfRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "objectRegister.h"

#include "MaterialUpdate.h"

namespace ModelSupport
{

void
materialUpdateHelp()
  /*!
    Write out the material update help
   */
{
  ELog::EM<<"voidObject Help "<<ELog::endBasic;
  ELog::EM<<
    " -- FixedComp :: voids all cells in FixedComp \n "
    " -- FixedName::CellMap :: voids all CellMap Name \n"
    " -- FixedName::CellMap::<Index> ::  "
    "  voids all CellMap Name at index \n"
	  <<ELog::endBasic;
  ELog::EM<<ELog::endErr;
  return;
}

void
materialUpdate(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*
    Sets a component to all void if set
    \param System :: Simulation to get objects from
    \param IParam :: Parameters
  */
{
  ELog::RegMethod RegA("MaterialUpdate[F]","materialUpdate");
  
  const size_t nP=IParam.setCnt("voidObject");
  for(size_t index=0;index<nP;index++)
    {
      const size_t nItem=IParam.itemCnt("voidObject",index);
      for(size_t iName=0;iName<nItem;iName++)
	{
	  const std::string key=
	    IParam.getValue<std::string>("voidObject",index,iName);
	  
	  if (!iName && key=="help")
	    {
	      materialUpdateHelp();
	      return;
	    }
	  const std::vector<int> cellN=
	    System.getObjectRange(objName);

      std::string FName,LName;
      Geometry::Vec3D VPos,YAxis,ZAxis;
      if (key=="Plate" || key=="plate")
	{
	  size_t ptI;
	  const std::string PName="insertPlate"+std::to_string(index);
	  if (PType=="object")
	    {
	      ptI=4;
	      FName=IParam.getValueError<std::string>("OAdd",index,2,eMess);
	      LName=IParam.getValueError<std::string>("OAdd",index,3,eMess);
	      VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	    }
	  else if (PType=="free")
	    {
	      ptI=2;
              VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
              YAxis=IParam.getCntVec3D("OAdd",index,ptI,eMess);
              ZAxis=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	    }
	  else
	    throw ColErr::InContainerError<std::string>(PType,"plate type");
	  
	  const double XW=
	    IParam.getValueError<double>("OAdd",index,ptI,eMess);
	  const double ZH=
	    IParam.getValueError<double>("OAdd",index,ptI+1,eMess);
	  const double YT=
	    IParam.getDefValue<double>(0.1,"OAdd",index,ptI+2);
	  const std::string mat=
            IParam.getDefValue<std::string>("Void","OAdd",index,ptI+3);
	  
	  if (PType=="object")
	    constructSystem::addInsertPlateCell
	      (System,PName,FName,LName,VPos,XW,YT,ZH,mat);
	  else
	    constructSystem::addInsertPlateCell
	      (System,PName,VPos,YAxis,ZAxis,XW,YT,ZH,mat);
	  
	}
      else if (key=="Grid" || key=="Grid")
	{
	  size_t ptI;
	  const std::string PName="insertGrid"+std::to_string(index);
	  if (PType=="object")
	    {
	      ptI=4;
	      FName=IParam.getValueError<std::string>("OAdd",index,2,eMess);
	      LName=IParam.getValueError<std::string>("OAdd",index,3,eMess);
	      VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	    }
	  else if (PType=="free")
	    {
	      ptI=2;
              VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
              YAxis=IParam.getCntVec3D("OAdd",index,ptI,eMess);
              ZAxis=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	    }
	  else
	    throw ColErr::InContainerError<std::string>(PType,"plate type");
	  
	  const size_t NL=
	    IParam.getValueError<size_t>("OAdd",index,ptI,eMess);
	  const double length=
	    IParam.getValueError<double>("OAdd",index,ptI+1,eMess);
	  const double thick=
	    IParam.getDefValue<double>(0.1,"OAdd",index,ptI+2);
	  const double gap=
	    IParam.getDefValue<double>(0.1,"OAdd",index,ptI+3);
	  const std::string mat=
            IParam.getDefValue<std::string>("Void","OAdd",index,ptI+4);
	  
	  if (PType=="object")
	    constructSystem::addInsertGridCell
	      (System,PName,FName,LName,VPos,NL,length,thick,gap,mat);
	  else
	    constructSystem::addInsertGridCell
	      (System,PName,VPos,YAxis,ZAxis,NL,length,thick,gap,mat);
	  
	}
      else if (key=="Sphere" || key=="sphere")
	{
	  const std::string PName="insertSphere"+std::to_string(index);
	  size_t ptI;
	  if (PType=="object")
	    {
	      FName=IParam.getValueError<std::string>("OAdd",index,2,eMess);
	      LName=IParam.getValueError<std::string>("OAdd",index,3,eMess);
	      ptI=4;
	    }
	  else
	    ptI=2;
	  
	  VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	  const double radius=
	    IParam.getValueError<double>("OAdd",index,ptI,eMess);
	  const std::string mat=
	    IParam.getDefValue<std::string>("Void","OAdd",index,ptI+1);

	  if (PType=="object")
	    constructSystem::addInsertSphereCell
	      (System,PName,FName,LName,VPos,radius,mat);
	  else if (PType=="free")
	    {
	      constructSystem::addInsertSphereCell
		(System,PName,VPos,radius,mat);
	    }
	  else
	    throw ColErr::InContainerError<std::string>(PType,"sphere type");
	}

      else if (key=="Cylinder" || key=="cylinder")
	{
	  const std::string PName="insertCylinder"+std::to_string(index);

          size_t ptI;
	  if (PType=="object")
	    {
	      ptI=4;
	      FName=IParam.getValueError<std::string>("OAdd",index,2,eMess);
	      LName=IParam.getValueError<std::string>("OAdd",index,3,eMess);
	      VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	    }
	  else if (PType=="free")
	    {
	      ptI=2;
              VPos=IParam.getCntVec3D("OAdd",index,ptI,eMess);
              YAxis=IParam.getCntVec3D("OAdd",index,ptI,eMess);
	    }
	  else
	    throw ColErr::InContainerError<std::string>(PType,"cylinder type");
	  
	  const double radius=
	    IParam.getValueError<double>("OAdd",index,ptI,eMess);
	  const double length=
	    IParam.getValueError<double>("OAdd",index,ptI+1,eMess);
	  const std::string mat=
	    IParam.getDefValue<std::string>("Void","OAdd",index,ptI+2);
          
	  if (PType=="object")
	    constructSystem::addInsertCylinderCell
	      (System,PName,FName,LName,VPos,radius,length,mat);
	  else if (PType=="free")
	    {
	      constructSystem::addInsertCylinderCell
		(System,PName,VPos,YAxis,radius,length,mat);
	    }
	  else
	    throw ColErr::InContainerError<std::string>(PType,"sphere type");
	}
      else
        throw ColErr::InContainerError<std::string>
          (key,"key not known in OAdd");
    }
  
  return;
}

}  // NAMESPACE ModelSupport
