/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/TDCsegment.cxx
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

#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Exception.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"

#include "VirtualTube.h"
#include "BlankTube.h"
#include "portItem.h"


#include "TDCsegment.h"

namespace tdcSystem
{

// Note currently uncopied:

TDCsegment::TDCsegment(const std::string& Key,const size_t NL) :
  attachSystem::FixedRotate(Key,NL),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(nullptr),NCellInit(0),
  sideSegment(nullptr)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param NL :: number of links
  */
{}

TDCsegment::~TDCsegment()
  /*!
    Destructor
   */
{}

void
TDCsegment::setFirstItems
  (const std::shared_ptr<attachSystem::FixedComp>& FCptr)
  /*!
    Allocate the first pointer
    \param FCptr :: FixedComp Point to pass
   */
{
  attachSystem::ExternalCut* EPtr=
    dynamic_cast<attachSystem::ExternalCut*>(FCptr.get());
  if (!EPtr)
    throw ColErr::DynamicConv("ExternalCut","FixedComp","FCPtr");

  firstItemVec.push_back(EPtr);
  
  return;
}

void
TDCsegment::setFirstItems(attachSystem::FixedComp* FCptr)
  /*!
    Allocate the first pointer
    \param FCptr :: FixedComp Point to pass
   */
{
  attachSystem::ExternalCut* EPtr=
    dynamic_cast<attachSystem::ExternalCut*>(FCptr);

  // This can be Null
  firstItemVec.push_back(EPtr);
  
  return;
}


void
TDCsegment::setFrontSurfs(const std::vector<HeadRule>& HRvec)
  /*!
    Set the front surface if need to join
    \param HR :: Front head rule
  */
{
  ELog::RegMethod RegA("TDCsegement","setFrontSurfs");

  for(size_t i=0;i<HRvec.size() && i<firstItemVec.size();i++)
    {
      attachSystem::ExternalCut* FPtr=firstItemVec[i];
      if (FPtr)
	FPtr->setCutSurf("front",HRvec[i]);
    }
  return;
}

const constructSystem::portItem&
TDCsegment::buildIonPump2Port(Simulation& System,
			      attachSystem::InnerZone& buildZone,
			      MonteCarlo::Object *masterCell,
			      const attachSystem::FixedComp& linkUnit,
			      const std::string& sideName,
			      constructSystem::BlankTube& ionPump) const
/*!
  Build 2 port ion pump
 */
{
  ionPump.addAllInsertCell(masterCell->getName());
  ionPump.setPortRotation(3, Geometry::Vec3D(1,0,0));
  ionPump.createAll(System,linkUnit,sideName);

  const constructSystem::portItem& port=ionPump.getPort(1);
  const int outerCell=
    buildZone.createOuterVoidUnit(System,
				  masterCell,
				  port,
				  port.getSideIndex("OuterPlate"));
  ionPump.insertAllInCell(System,outerCell);

  return port;
}

bool
TDCsegment::totalPathCheck(const FuncDataBase& Control,
			   const double errDist) const
  /*!
    Returns true if the last unit is in error
    \param Control :: DataBase for start/end variables
    \param errDist :: Error distance [default 0.1 (1mm)]
    \return 1 if error by more than errDist
  */
{
  ELog::RegMethod RegA("TDCsegment","totalPathCheck");

  bool retFlag(0);

  const double tolDist=
    Control.EvalDefVar<double>(keyName+"Tolerance",errDist);
  size_t testNum(0);
  const std::string Letters=" ABCDEF";
  for(size_t i=0;i<Letters.size();i++)
    {
      // main OffsetA / EndAOffset
      // link point FrontALink / BackALink
      const std::string AKey=keyName+"Offset"+Letters[i];
      const std::string BKey=keyName+"EndOffset"+Letters[i];

      if (Control.hasVariable(AKey) && Control.hasVariable(BKey))
	{
	  testNum++;
	  const Geometry::Vec3D cadStart=
	    Control.EvalVar<Geometry::Vec3D>(AKey);
	  const Geometry::Vec3D cadEnd=
	    Control.EvalVar<Geometry::Vec3D>(BKey);
	  const std::string startLink=
	    Control.EvalDefVar<std::string>
	    (keyName+"FrontLink"+Letters[i],"front");
	  const std::string endLink=
	    Control.EvalDefVar<std::string>
	    (keyName+"BackLink"+Letters[i],"back");

	  //
	  // Note that this is likely different from true start point:
	  // as we can apply initial offset to the generation object
	  //
	  const Geometry::Vec3D modelStart=FixedComp::getLinkPt(startLink);
	  const Geometry::Vec3D modelEnd=FixedComp::getLinkPt(endLink);
	  const Geometry::Vec3D vEnd(modelEnd-(modelStart-cadStart));
	  
	  const double D=vEnd.Distance(cadEnd);
	  
	  if (D>tolDist)
	    {
	      ELog::EM<<"WARNING Segment:: "<<keyName<<" has wrong track \n\n";
	      ELog::EM<<"Test number "<<testNum<<"\n";
	      ELog::EM<<"--------------------\n\n";
	      ELog::EM<<"CAD Start Point  "<<cadStart<<"\n";
	      ELog::EM<<"CAD End Point    "<<cadEnd<<"\n";
	      ELog::EM<<"CAD length ==    "<<cadStart.Distance(cadEnd)<<"\n";

	      const Geometry::Vec3D modelDVec((modelEnd-modelStart).unit());
	      const Geometry::Vec3D cadDVec((cadEnd-cadStart).unit());

	      const double cosA=modelDVec.dotProd(cadDVec);
	      double angleError=(cosA>(1.0-1e-7)) ? 0.0 : acos(cosA);
	      angleError*=180.0/M_PI;
	      if (angleError>90.0) angleError-=180.0;
	      if (std::abs(angleError)>tolDist/10.0)
		{
		  const Geometry::Vec3D YY(0,1,0);
		  const double angModel=180.0*acos(modelDVec.dotProd(YY))/M_PI;
		  const double angCAD=180.0*acos(cadDVec.dotProd(YY))/M_PI;
		  
		  ELog::EM<<"Model Angle   ==    "<<angModel<<"\n";
		  ELog::EM<<"CAD Angle     ==    "<<angCAD<<"\n";
		  ELog::EM<<"Angle error    ==    "<<angleError<<"\n\n";
		}
	      
	      ELog::EM<<"model Start     "<<modelStart<<"\n";
	      ELog::EM<<"model End       "<<modelEnd<<"\n";
	      ELog::EM<<"model length == "<<modelEnd.Distance(modelStart)<<"\n\n";
	      
	      ELog::EM<<"corrected Start End   "<<vEnd<<"\n\n";
	      
	      ELog::EM<<"ERROR dist   "<<D<<ELog::endWarn;
	      retFlag=1;
	    }
	}
  
    }
  return retFlag;
}

void
TDCsegment::initCellMap()
  /*!
    Set inital value
   */
{
  ELog::RegMethod RegA("TDCsegment","initCellMap");
  
  const attachSystem::CellMap* CPtr=
    (buildZone) ? buildZone->getCellMap() :  0;

  NCellInit=(CPtr && CPtr->hasCell("OuterVoid"))
    ? CPtr->getNCells("OuterVoid") : 0;
  
  return;
}

void
TDCsegment::captureCellMap()
  /*!
    Set inital value
   */
{
  ELog::RegMethod RegA("TDCsegment","captureCellMap");
  
  const attachSystem::CellMap* CPtr=(buildZone) ? buildZone->getCellMap() : 0;
  const size_t NCells=(CPtr) ?CPtr->getNCells("OuterVoid") : 0;

  for(size_t i=NCellInit;i<NCells;i++)
    CellMap::addCell("BuildVoid",CPtr->getCell("OuterVoid",i));

  NCellInit=NCells;
		     
  return;
}

}   // NAMESPACE tdcSystem
