/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/Volumes.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "support.h"
#include "volUnit.h"
#include "VolSum.h"
#include "Volumes.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "World.h"
#include "Importance.h"
#include "Object.h"

namespace ModelSupport
{

void
calcVolumes(Simulation* SimPtr,const mainSystem::inputParam& IParam)
  /*!
    Calculate the volumes for all f4 tallies
    \param SimPtr :: Simulation to use
    \param IParam :: Simulation to use
  */
{
  ELog::RegMethod RegA("Volumes[F]","calcVolumes");

  if (SimPtr && IParam.flag("volume"))
    {
      SimPtr->createObjSurfMap();
      Geometry::Vec3D Org=IParam.getValue<Geometry::Vec3D>("volume");

      const Geometry::Vec3D XYZ=IParam.getValue<Geometry::Vec3D>("volume",1);

      const size_t NP=IParam.getValue<size_t>("volNum");
      VolSum VTally(Org,XYZ);
      if (IParam.flag("volCells") )
	populateCells(*SimPtr,IParam,VTally);
      else
	{
	  const SimMCNP* SMPtr=dynamic_cast<const SimMCNP*>(SimPtr);
	  VTally.populateTally(*SMPtr);
	}

      VTally.pointRun(*SimPtr,NP);
      ELog::EM<<"Volume == "<<Org<<" : "<<XYZ<<" : "<<NP<<ELog::endDiag;
      VTally.write("volumes");
    }

  return;
}

void
generateCut(const std::string& fName,
	    const Geometry::Vec3D& centPoint,
	    const Geometry::Vec3D& xAxis,
	    const Geometry::Vec3D& yAxis,
	    const size_t nX,
	    const size_t nY,
	    const std::vector<Geometry::Vec3D> Pts)
  /*!
    Generate a simple plane plot from a mesh of data points
    \param centPoint :: Origin
    \param xAxis :: extent (half) about center
    \param yAxis :: extent (half) about center
    \param nX :: Number of x points
    \param nY :: Number of y points
  */
{
  ELog::RegMethod RegA("Volumes[F]","generatePlot");

  std::ofstream OX(fName.c_str());
  const double xLen=xAxis.abs();
  const double yLen=yAxis.abs();

  const Geometry::Vec3D lowCorner=Pt-xAxis*0.5-yAxis*0.5;
  multiData<int> AreaMap(nX,nY);
  for(Geometry::Vec3D Pt : Pts)
    {
      Pt-=lowCorner
      const double LX=xAxis.dotProd(Pt)/xLen;
      const double LY=yAxis.dotProd(Pt)/yLen;
      if (LX>=0.0 && LX<=xLen &&
	  LY>=0.0 && LY<=yLen)
	{
	  const size_t iX=LX/xLen;
	  const size_t iY=LX/xLen;
	  AreaMap.get()[iX][iY]++;
	}
    }

  std::ofstream OX;
  
  

  return;
}
  
void
materialCheck(const Simulation& System,
	      const mainSystem::inputParam& IParam)
  /*!
    Check an external file for material values in the model
    \param SimPtr :: Simulation to use
    \param IParam :: Simulation to use
  */
{
  ELog::RegMethod RegA("Volumes[F]","materialCheck");

  if (IParam.flag("materialCheck"))
    {
      const std::string fileName=IParam.getValue<std::string>("materialCheck");
      const std::string objName=
	IParam.getDefValue<std::string>("","materialCheck",1);
      const std::string linkName=
	IParam.getDefValue<std::string>("Origin","materialCheck",2);
      const Geometry::Vec3D linkOffset=
	IParam.getDefValue<Geometry::Vec3D>(Geometry::Vec3D(0,0,0),
					    "materialCheck",3);

      const attachSystem::FixedComp& FC=
	(objName.empty()) ?  World::masterOrigin() :
	*(System.getObjectThrow<attachSystem::FixedComp>
	  (objName,"Object not found"));
      
      
      const long int linkIndex=(linkName.empty()) ?  0 :
	FC.getSideIndex(linkName);

      const Geometry::Vec3D centre=FC.getLinkPt(linkIndex);
      ELog::EM<<"Center = "<<centre<<ELog::endDiag;
      Geometry::Vec3D X;
      Geometry::Vec3D Y;
      Geometry::Vec3D Z;
      FC.calcLinkAxis(linkIndex,X,Y,Z)
;
  return;
}

void
populateCells(const Simulation& System,
	      const mainSystem::inputParam& IParam,
	      VolSum& VTally)
  /*!
    Populate the Volume Tallys with cells defined of the input
    stream.
    \param System :: Simulation 
    \param IParam :: Input stream
    \param VTally :: Tally to populate
   */
{
  ELog::RegMethod RegA("Volumes[F]","populateCells");


  for(size_t i=0;i<IParam.setCnt("volCells");i++)
    {
      const size_t NItems=IParam.itemCnt("volCells",i);
      std::vector<std::string> CStr;
      for(size_t j=0;j<NItems;j++)
	CStr.push_back(IParam.getValue<std::string>("volCells",i,j));
      
      if (NItems && (CStr[0]=="all" || CStr[0]=="All"))
	VTally.populateAll(System);
      else if (NItems>=2 && (CStr[0]=="object" || CStr[0]=="Object"))
	{
	  std::vector<int> CNumbers;
	  for(size_t j=1;j<NItems;j++)
	    {
	      const std::set<int> CNumPlus=
		System.getObjectRange(CStr[j]);
	      if (CNumPlus.empty())
		throw ColErr::InContainerError<std::string>
		  (CStr[j]," Cell object not known");

	      ELog::EM<<"CNum == "<<CNumPlus.size()<<ELog::endDiag;
	      CNumbers.insert(CNumbers.end(),CNumPlus.begin(),CNumPlus.end());
	    }
	  ELog::EM<<"Cell size = "<<CNumbers.size()<<ELog::endDiag;
	  VTally.populateVSet(System,CNumbers);
	}
    }
  return;
}
  


} // NAMESPACE ModelSupport
