/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/TallySelector.cxx
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
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "inputParam.h"
#include "addInsertObj.h"
#include "Simulation.h"

#include "ObjectAddition.h"

namespace ModelSupport
{

void
objectAddition(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*
    Adds components to the model -- should NOT be here
    \param System :: Simulation to get tallies from 
    \param IParam :: Parameters
  */
{
  ELog::RegMethod RegA("ObjectAddtion[F]","objectAddition");

const size_t nP=IParam.setCnt("OAdd");
  for(size_t index=0;index<nP;index++)
    {
      const std::string eMess
	("Insufficient item for OAdd["+std::to_string(index)+"]");
      const std::string key=
	IParam.getValueError<std::string>("OAdd",index,0,eMess);
      
      if(key=="help")
	{
	  ELog::EM<<"OAdd Help "<<ELog::endBasic;
	  ELog::EM<<
	    " -- cylinder object FixedComp linkPt +Vec3D(x,y,z) radius \n";
	  ELog::EM<<
	    " -- cylinder free Vec3D(x,y,z) Vec3D(nx,ny,nz) radius \n";
	  ELog::EM<<
	    " -- plate object fixedComp linkPt +Vec3D(x,y,z) "
	    "xSize zSize {ySize=0.1} {mat=Void}\n";
	  ELog::EM<<
	    " -- plate free Vec3D(x,y,z) Vec3D(yAxis) Vec3D(zAxis) "
	    "xSize zSize {ySize=0.1} {mat=Void}\n";
	  ELog::EM<<
	    " -- sphere object FixedComp linkPt +Vec3D(x,y,z) radius \n";
	  ELog::EM<<
	    " -- sphere free Vec3D(x,y,z) radius \n";
	  ELog::EM<<
	    " -- grid object FixedComp linkPt +Vec3D(x,y,z) "
	    " NL length thick gap mat \n";
	  ELog::EM<<
	    " -- grid free Vec3D(x,y,z) Vec3D(yAxis) Vec3D(zAxis)"
	    " NL length thick gap mat \n";
	  ELog::EM<<ELog::endBasic;
	  ELog::EM<<ELog::endErr;
          return;
	}
      const std::string PType=
	IParam.getValueError<std::string>("OAdd",index,1,eMess);

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
	      const std::string FName=
		IParam.getValueError<std::string>("OAdd",index,2,eMess);
	      const std::string LName=
		IParam.getValueError<std::string>("OAdd",index,3,eMess);
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
