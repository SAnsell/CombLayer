/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: maxpeem/TDC.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"

// #include "VacuumPipe.h"
// #include "SplitFlangePipe.h"
// #include "Bellows.h"
// #include "LeadPipe.h"
// #include "VacuumBox.h"
// #include "portItem.h"
// #include "PipeTube.h"
// #include "PortTube.h"

#include "InjectionHall.h"
#include "L2SPFsegment1.h"

#include "TDC.h"

namespace tdcSystem
{

TDC::TDC(const std::string& KN) :
  attachSystem::FixedOffset(KN,6),
  injectionHall(new InjectionHall("InjectionHall")),
  l2spf1(new L2SPFsegment1("L2SPF1"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(injectionHall);
  OR.addObject(l2spf1);
}

TDC::~TDC()
  /*!
    Destructor
   */
{}

void 
TDC::createAll(Simulation& System,
	       const attachSystem::FixedComp& FCOrigin,
	       const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
   */
{
  // For output stream
  ELog::RegMethod RControl("TDC","build");

  static const std::map<std::string,std::string> segmentLinkMap
    ({
      {"L2SPFsegment1","Origin"}
    });
  const int voidCell(74123);
  
  // build injection hall first:
  injectionHall->addInsertCell(voidCell);
  injectionHall->createAll(System,FCOrigin,sideIndex);
    
  for(const std::string& BL : activeINJ)
    {
      if (BL=="L2SPFsegment1")  
	{
	  std::unique_ptr<L2SPFsegment1> BLPtr;
	  BLPtr.reset(new L2SPFsegment1("L2SPFseg1"));
	  BLPtr->setCutSurf("floor",injectionHall->getSurf("Floor"));
	  BLPtr->setCutSurf("front",injectionHall->getSurf("Front"));

	  BLPtr->addInsertCell(injectionHall->getCell("LinearVoid"));
	  BLPtr->createAll
	    (System,*injectionHall,
	     injectionHall->getSideIndex(segmentLinkMap.at(BL)));
	}
    }

  return;
}


}   // NAMESPACE xraySystem

