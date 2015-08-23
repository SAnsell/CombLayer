/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/Volumes.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "inputParam.h"
#include "objectRegister.h"
#include "volUnit.h"
#include "VolSum.h"
#include "Volumes.h"

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
	VTally.populateTally(*SimPtr);

      VTally.pointRun(*SimPtr,NP);
      ELog::EM<<"Volume == "<<Org<<" : "<<XYZ<<" : "<<NP<<ELog::endDiag;
      VTally.write("volumes");
    }

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

  const ModelSupport::objectRegister& OR= 
    ModelSupport::objectRegister::Instance();

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
	      const int cellOffset=OR.getCell(CStr[j]);
	      const int cellRange=OR.getRange(CStr[j]);
	      if (!cellOffset)
		throw ColErr::InContainerError<std::string>
		  (CStr[j]," Cell object not known");
	      const std::vector<int> CNumPlus=
		System.getCellVectorRange(cellOffset,cellRange);
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
