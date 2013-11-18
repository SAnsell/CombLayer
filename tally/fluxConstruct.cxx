/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/fluxConstruct.cxx
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

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
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MeshGrid.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "Collimator.h"
#include "ColBox.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "InnerWall.h"
#include "Hutch.h"
#include "Simulation.h"
#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "BulkShield.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "FlightLine.h"
#include "PreMod.h"
#include "HWrapper.h"
#include "Decoupled.h"
#include "VacVessel.h"
#include "RefCutOut.h"
#include "Bucket.h"
#include "CoolPad.h"
#include "Reflector.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "bendSection.h"
#include "ZoomBend.h"
#include "ZoomChopper.h"
#include "ZoomStack.h"
#include "ZoomCollimator.h"
#include "ZoomPrimary.h"
#include "ZoomTank.h"
#include "ZoomHutch.h"
#include "inputParam.h"
#include "FBBlock.h"
#include "makeChipIR.h"
#include "makeZoom.h"

#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "fluxConstruct.h" 

namespace tallySystem
{

fluxConstruct::fluxConstruct() 
  /// Constructor
{}

fluxConstruct::fluxConstruct(const fluxConstruct&) 
  /// Copy Constructor
{}

fluxConstruct&
fluxConstruct::operator=(const fluxConstruct&) 
  /// Assignment operator
{
  return *this;
}

int
fluxConstruct::processFlux(Simulation& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index,
			   const bool renumberFlag) const
  /*!
    Add heat tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
    \param renumberFlag :: Is this a renumber call
  */
{
  ELog::RegMethod RegA("fluxConstruct","processFlux");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");

  const std::string PType(IParam.getCompValue<std::string>("tally",Index,1)); 
  
  if (PType=="help")  // 
    {
      if (!renumberFlag)
	{
	  ELog::EM<<ELog::endBasic;
	  ELog::EM<<
	    "Flux tally :\n"
	    "particles material(int) objects \n"
	    "particles material(int) Range(low-high)\n"
		  <<ELog::endBasic;
	}
      return 0;
    }
  const std::string MType(IParam.getCompValue<std::string>("tally",Index,2)); 
  
  // Process a Ranged Heat:
 
  boost::format Cmt("tally: %d Mat %d Range(%d,%d)");
  int matN(0);

  // Get Material number:
  if (!StrFunc::convert(MType,matN))
    {
      ELog::EM<<"No material number for flux tally (particle "
	      <<PType<<")"<<ELog::endErr;
      return 0;
    }

  int RA,RB;
  // if flag true : then valid range
  const int flag=
    convertRegion(IParam,"tally",Index,3,RA,RB);
  // work for later:

  if (flag<0 && !renumberFlag) return 1;

  ELog::EM<<"Cells == "<<RA<<" "<<RB<<ELog::endDebug;
  const int nTally=System.nextTallyNum(4);
  // Find cells  [ Must handle -ve / 0 ]
  const std::vector<int> cells=getCellSelection(System,matN,RA,RB);

  tallySystem::addF4Tally(System,nTally,PType,cells);
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  const std::string Comment=(Cmt % nTally % matN % RA % RB ).str();
  TX->setComment(Comment);
  return 0;
}

}  // NAMESPACE tallySystem
