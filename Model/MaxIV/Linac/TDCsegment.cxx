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
#include <boost/format.hpp>

#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "Exception.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"
#include "Simulation.h"


#include "TDCsegment.h"

namespace tdcSystem
{

// Note currently uncopied:

TDCsegment::TDCsegment(const std::string& Key,const size_t NL) :
  attachSystem::FixedRotate(Key,NL),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  NCellInit(0),
  prevSegPtr(nullptr)
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
TDCsegment::registerSideSegment(const TDCsegment* SPtr)
  /*!
    Register a sideward segment
   */
{
  sideVec.push_back(SPtr);
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


void
TDCsegment::registerPrevSeg(const TDCsegment* PSPtr,
			    const size_t indexPoint)
  /*!
   Process previous segments [default version]
   This segment is register in previous segment by: joinItems
   It is set in createLinks. Manditory to set at least 1.
   It is captured in the next segment by
   TDCsegment::setFrontSurfaces -- it used firstItemVec
   which is set in segment constructor.
   \param PSPtr :: previous segment
   \param indexPoint :: Index point to use for HeadRule
  */
{
  ELog::RegMethod RegA("TDCsegment","registerPrevSeg");

  prevSegPtr=PSPtr;
  if (prevSegPtr)
    {
      const std::vector<HeadRule>& prevJoinItems=
	prevSegPtr->getJoinItems();
      if (!prevJoinItems.empty())
	{
	  if (buildZone && indexPoint && indexPoint<=prevJoinItems.size())
	    buildZone->setFront(prevJoinItems[indexPoint-1]);
	  
	  this->setFrontSurfs(prevJoinItems);

	}
    }
  
  return;
}

void
TDCsegment::removeSpaceFillers(Simulation& System) const
  /*!
    Remove space fillers
    \param System :: Simulation to remove cells from
   */
{
  if (prevSegPtr)
    {
      if (prevSegPtr->hasCell("SpaceFiller"))
	{
	  for(const int CN : prevSegPtr->getCells("SpaceFiller"))
	    System.removeCell(CN);
	}
    }
  return;
}

void
TDCsegment::createBeamLink(const FuncDataBase& Control)
  /*!
    Construct links that start at the beam
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("TDCsegment","createBeamLink");
  
  const size_t NLink=std::max<size_t>(8,this->NConnect());
  FixedComp::setNConnect(NLink);
  FixedComp::nameSideIndex(6,"Beam");
  setLinkCopy(6,*this,-1);    // copy surface and correct direction

  attachSystem::FixedRotateUnit BPoint(0,"BeamPoint");
  Geometry::Vec3D BeamOrg=this->getLinkPt(1);
  Geometry::Vec3D BeamAxis=this->getLinkAxis(-1);

  if (Control.hasVariable(keyName+"BeamOrg"))
    {
      BeamOrg=
	Control.EvalVar<Geometry::Vec3D>(keyName+"BeamOrg");
    }
  if (Control.hasVariable(keyName+"BeamAxis"))
    {
      BeamAxis=
	Control.EvalVar<Geometry::Vec3D>(keyName+"BeamAxis");
    }
  
  //  FixedComp::setConnect(6,BeamOrg,BeamAxis);


  if (Control.hasVariable(keyName+"BeamDelta"))
    {
      const Geometry::Vec3D BeamDelta=
	Control.EvalVar<Geometry::Vec3D>(keyName+"BeamDelta");
      BPoint.setOffset(BeamDelta[0],BeamDelta[1],BeamDelta[2]);
    }
  
      
  const double beamXY=
    Control.EvalDefVar<double>(keyName+"BeamXYAngle",0.0);
  const double beamX=
    Control.EvalDefVar<double>(keyName+"BeamXAngle",0.0);
  const double beamY=
    Control.EvalDefVar<double>(keyName+"BeamYAngle",0.0);      
  const double beamZ=
    Control.EvalDefVar<double>(keyName+"BeamZAngle",beamXY);
  
  BPoint.setRotation(beamX,beamY,beamZ);
  BPoint.createUnitVector(BeamOrg,BeamAxis,Z);
  FixedComp::setConnect(6,BPoint.getLinkPt(0),
			BPoint.getLinkAxis(0));

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

  bool retFlag(0);

  const double tolDist=
    Control.EvalDefVar<double>(keyName+"Tolerance",errDist);
  size_t testNum(0);
  const std::string Letters=" ABCDEF";
  for(size_t i=0;i<Letters.size();i++)
    {
      // main OffsetA / EndAOffset
      // link point FrontALink / BackALink
      std::string AKey=keyName+"Offset"+Letters[i];
      std::string BKey=keyName+"EndOffset"+Letters[i];
      std::string CKey=keyName+"EndAngle"+Letters[i];

      // right trim
      if (Letters[i] == ' ')
	{
	  AKey.erase(AKey.find_last_not_of(" ")+1);
	  BKey.erase(BKey.find_last_not_of(" ")+1);
	  CKey.erase(CKey.find_last_not_of(" ")+1);
	}

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
	  const Geometry::Vec3D modelAY=FixedComp::getLinkAxis(startLink);
	  const Geometry::Vec3D modelBY=FixedComp::getLinkAxis(endLink);
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

	      ELog::EM<<"corrected End   "<<vEnd<<"\n\n";
	      ELog::EM<<"ERROR dist   "<<D<<ELog::endWarn;

	      if (Control.hasVariable(CKey))
		{
		  const double aAngle=acos(std::abs(modelAY.Y()))*180/M_PI;
		  const double bAngle=acos(std::abs(modelBY.Y()))*180/M_PI;
		  const double cadEndAngle=Control.EvalVar<double>(CKey);
		  ELog::EM<<"A   = "<<aAngle<<ELog::endDiag;
		  ELog::EM<<"B   = "<<bAngle<<ELog::endDiag;
		  ELog::EM<<"CAD = "<<cadEndAngle<<ELog::endDiag;
		}
		  

	      retFlag=1;
	    }
	}

    }
  return retFlag;
}



void
TDCsegment::writeBasicItems
(const std::vector<std::shared_ptr<attachSystem::FixedComp>>& Items) const
 /*!
   Write out basic items to a list for debug
   \param :: Items :: Ordered list of items in the model
 */
{
  
  // NO ELog so the calling function segment name is used:

  boost::format iFMT("%1$-20s%|30t| == %2$=12.6g %|60t| Len == %3$12.6g \n");

  if (!Items.empty())
    {
      ELog::EM<<"\n";
      const Geometry::Vec3D Org(Items[0]->getLinkPt(1)*10.0);
      for(const std::shared_ptr<attachSystem::FixedComp>& FCptr : Items)
	{
	  const Geometry::Vec3D endPt(FCptr->getLinkPt(2)*10.0);
	  const Geometry::Vec3D DV=endPt-Org;
	  ELog::EM<<(iFMT % FCptr->getKeyName() % DV % DV.abs());
	}
      ELog::EM<<ELog::endDiag;
    }	  
  return;
   
}


}   // NAMESPACE tdcSystem
