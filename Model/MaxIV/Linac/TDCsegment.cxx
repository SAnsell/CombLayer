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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "InnerZone.h"

#include "TDCsegment.h"

namespace tdcSystem
{

// Note currently uncopied:
  
TDCsegment::TDCsegment(const std::string& Key,const size_t NL) :
  attachSystem::FixedRotate(Key,NL),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(nullptr),
  lastFlag(0),firstItemPtr(nullptr)
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
TDCsegment::setFirstItem
  (const std::shared_ptr<attachSystem::FixedComp>& FCptr)
  /*!
    Allocate the first pointer
    \param FCptr :: FixedComp Point to pass
   */
{
  firstItemPtr=dynamic_cast<attachSystem::ExternalCut*>(FCptr.get());
  return;
}

void
TDCsegment::setFrontSurf(const HeadRule& HR)
  /*!
    Set the front surface if need to join
    \param HR :: Front head rule
  */
{
  
  if (firstItemPtr)
    firstItemPtr->setCutSurf("front",HR);
  return;
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

  const Geometry::Vec3D startPoint=
    Control.EvalVar<Geometry::Vec3D>(keyName+"Offset");

  const Geometry::Vec3D endPoint=
    Control.EvalVar<Geometry::Vec3D>(keyName+"EndOffset");

  const Geometry::Vec3D sndEndPoint=
    Control.EvalDefVar<Geometry::Vec3D>(keyName+"SndEndOffset",endPoint);

  //
  // Note that this is likely different from true start point:
  // as we can apply initial offset to the generation object
  //
  const Geometry::Vec3D realStart=FixedComp::getLinkPt(1);
  const Geometry::Vec3D realEnd=FixedComp::getLinkPt(2);

  const Geometry::Vec3D vEnd(realEnd-(realStart-startPoint));

  const double D=vEnd.Distance(endPoint);

  bool retFlag(0);
  if (D>0.1)
    {
      ELog::EM<<"WARNING Segment:: "<<keyName<<" has wrong track \n\n";
      ELog::EM<<"Start Point  "<<startPoint<<"\n";
      ELog::EM<<"End Point    "<<endPoint<<"\n";
      ELog::EM<<"length ==    "<<startPoint.Distance(endPoint)<<"\n";
      const double cosA=(realEnd-realStart).unit().
	      dotProd((startPoint-endPoint).unit());
      double angleError=acos(cosA)*180.0/M_PI;
      if (angleError>90.0) angleError-=180.0;
      ELog::EM<<"Angle error ==    "<<angleError<<"\n\n";

      ELog::EM<<"model Start     "<<realStart<<"\n";
      ELog::EM<<"model End       "<<realEnd<<"\n";
      ELog::EM<<"model length == "<<realEnd.Distance(realStart)<<"\n\n";

      ELog::EM<<"corrected Start End   "<<vEnd<<"\n\n";

      ELog::EM<<"ERROR dist   "<<D<<ELog::endWarn;
      retFlag=1;
    }
  
  if (sndEndPoint.Distance(endPoint)>0.1)
    {
      const Geometry::Vec3D sndRealEnd=FixedComp::getLinkPt(3);
      const Geometry::Vec3D wEnd(sndRealEnd-(realStart-startPoint));
	
      const double D=wEnd.Distance(sndEndPoint);
      if (D>0.1)
	{
	  const double cosA=(sndRealEnd-realStart).unit().
	    dotProd((startPoint-sndEndPoint).unit());
	  double angleError=acos(cosA)*180.0/M_PI;
	  if (angleError>90.0) angleError-=180.0;
	  
	  
	  ELog::EM<<"WARNING Segment:: "<<keyName<<" has wrong track \n\n";
	  ELog::EM<<"Start Point          "<<startPoint<<"\n";
	  ELog::EM<<"Second  End Point    "<<sndEndPoint<<"\n";
	  ELog::EM<<"length ==    "<<startPoint.Distance(sndEndPoint)<<"\n";
	  ELog::EM<<"Angle error ==    "<<angleError<<"\n\n";
	    
	  ELog::EM<<"model Start    "<<realStart<<"\n";
	  ELog::EM<<"model End      "<<sndRealEnd<<"\n";
	  ELog::EM<<"model length == "<<sndRealEnd.Distance(realStart)<<"\n\n";
	  
	  ELog::EM<<"expected End   "<<wEnd<<"\n\n";
	  
	  ELog::EM<<"ERROR dist   "<<D<<ELog::endWarn;
	  retFlag=1;
	}
    }
  return retFlag;
}
    
void
TDCsegment::setLastSurf(const HeadRule& HR)
  /*!
    Set the last surface rule
    \param HR :: Head rule to use
   */
{
  lastFlag=1;
  lastRule=HR;
  return;
}


}   // NAMESPACE tdcSystem

