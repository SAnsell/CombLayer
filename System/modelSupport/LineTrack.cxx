/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/LineTrack.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "ObjSurfMap.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "particle.h"
#include "eTrack.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineUnit.h"
#include "LineTrack.h"


namespace ModelSupport
{

std::ostream&
operator<<(std::ostream& OX,const LineTrack& A)
  /*!
    Write out to a stream
    \param OX :: Output stream
    \param A :: LineTrack to write
    \return Stream
  */
{
  A.write(OX);
  return OX;
}

LineTrack::LineTrack(const Geometry::Vec3D& IP,
		     const Geometry::Vec3D& EP) :
  InitPt(IP),EndPt(EP),uVec((EndPt-InitPt).unit()),
  aimDist((EP-IP).abs()),
  TDist(0.0)
  /*! 
    Constructor 
    \param IP :: Initial point
    \param EP :: End point
  */
{}

LineTrack::LineTrack(const Geometry::Vec3D& IP,
		     const Geometry::Vec3D& UVec,
		     const double ADist) :
  InitPt(IP),EndPt(IP+UVec),uVec(UVec.unit()),
  aimDist(ADist>=0 ? ADist : 1e10),
  TDist(0.0)
  /*! 
    Constructor 
    \param IP :: Initial point
    \param EP :: End point
    \param ADist :: Aim dist 
  */
{}

LineTrack::LineTrack(const LineTrack& A) : 
  InitPt(A.InitPt),EndPt(A.EndPt),
  uVec(A.uVec),
  aimDist(A.aimDist),TDist(A.TDist),
  trackPts(A.trackPts)

  /*!
    Copy constructor
    \param A :: LineTrack to copy
  */
{}

LineTrack&
LineTrack::operator=(const LineTrack& A)
  /*!
    Assignment operator
    \param A :: LineTrack to copy
    \return *this
  */
{
  if (this!=&A)
    {
      InitPt=A.InitPt;
      EndPt=A.EndPt;
      uVec=A.uVec;
      aimDist=A.aimDist;
      TDist=A.TDist;
      trackPts=A.trackPts;
    }
  return *this;
}

void
LineTrack::clearAll()
  /*!
    Clears all the data
  */
{
  TDist=0.0;
  trackPts.clear();
  return;
}


int
LineTrack::calculate(const Simulation& System,
		     MonteCarlo::Object* initCell)
  /*!
    Check the track (faster than calculate)
    \param System :: Simulation to use
    \return error state
  */
{
  ELog::RegMethod RegA("LineTrack","calculate");

  double aDist(0);                         // Length of track
  const Geometry::Surface* SPtr(0);           // Surface
  const ModelSupport::ObjSurfMap* OSMPtr=System.getOSM();

  Geometry::Vec3D Org(InitPt);
  // Find Initial cell [no default]
  
  MonteCarlo::Object* OPtr=System.findCell(InitPt,initCell);

  if (!OPtr)
    throw ColErr::InContainerError<Geometry::Vec3D>
      (InitPt,"Initial point not in model");

  // processing being on a side
  int SN(0);
  const std::set<int> SSet=OPtr->isOnSide(InitPt);
  if (!SSet.empty())
    {
      int TD(0);
      std::set<int>::const_iterator sc;
      for(sc=SSet.begin();!TD && sc!=SSet.end();sc++)
	{
	  SN=*sc;
	  TD=OPtr->trackDirection(SN,Org,uVec);
	  SN*=TD;
	  if (TD<0)
	    {
	      OPtr=OSMPtr->findNextObject(-SN,Org,OPtr->getName());
	      SN=0;
	    }
	}
    }

  // problem is that SN will be on TWO objects
  // so which one is it? [it doesn't matter much]
    
  while(OPtr && !OPtr->isZeroImp())
    {
      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackCell(Org,uVec,aDist,SPtr,SN);
      // Update Track : returns 1 on excess of distance
      if (SN && updateTrack(OPtr,SPtr,SN,aDist))
	{
	  Org+=uVec*aDist;
	  OPtr=OSMPtr->findNextObject(SN,Org,OPtr->getName());
	  if (!OPtr)
	    {
	      ELog::EM<<"INIT POINT[error] == "<<InitPt<<ELog::endCrit;
	      //	      calculateError(System);
	      return -1;
	    }
	  // if (!OPtr || aDist<Geometry::zeroTol)
	  //   OPtr=System.findCell(nOut.Pos,0);
	}
      else
	OPtr=0;	
    }
  //
  // remove last object if point does not reach it:
  //
  if (trackPts.size()>1 && OPtr) 
    {
      if (OPtr->trackDirection(EndPt,uVec)==-1)
	trackPts.pop_back();
    }

  return 0;
}

void
LineTrack::calculateError(const Simulation& ASim)
  /*!
    Calculate the track [assuming everything has gone wrong]
    \param ASim :: Simulation to use						
  */
{
  ELog::RegMethod RegA("LineTrack","calculateError");
  
  ELog::EM<<"START OF ERROR CODE"<<ELog::endDiag;
  ELog::EM<<"-------------------"<<ELog::endDiag;
  
  double aDist(0);                         // Length of track
  const Geometry::Surface* SPtr;           // Surface
  const ModelSupport::ObjSurfMap* OSMPtr =ASim.getOSM();

  MonteCarlo::eTrack nOut(InitPt,uVec);

  // Find Initial cell [no default]
  MonteCarlo::Object* OPtr=ASim.findCell(InitPt,0);
  if (!OPtr)
    throw ColErr::InContainerError<Geometry::Vec3D>
      (InitPt,"Initial point not in model");

  int SN(0);
  const std::set<int> SSet=OPtr->isOnSide(InitPt);
  if (!SSet.empty())
    {
      ELog::EM<<"Updating to previous object / surface "
	      <<OPtr->getName()<<ELog::endDiag;

      int TD(0);
      std::set<int>::const_iterator sc;
      for(sc=SSet.begin();!TD && sc!=SSet.end();sc++)
	TD=OPtr->trackDirection(*sc,nOut.Pos,nOut.uVec);
      SN=*sc;
      OPtr=OSMPtr->findNextObject(TD* *sc,nOut.Pos,OPtr->getName());
      ELog::EM<<"New object ::"<<OPtr->getName()<<ELog::endDiag;
    }

  const MonteCarlo::Object* prevOPtr(0);
  ELog::EM<<"OPtr "<<OPtr->getHeadRule().display(InitPt)<<ELog::endDiag;
  ELog::EM<<"OPtr "<<OPtr->isValid(InitPt)<<ELog::endDiag;
  while(OPtr)
    {
      ELog::EM<<std::setprecision(12)<<ELog::endDiag;
      ELog::EM<<"== Tracking in cell == "<<*OPtr;
      ELog::EM<<"Track == "<<nOut<<" "<<aDist<<ELog::endDiag;
      ELog::EM<<"SN at start== "<<SN<<ELog::endDiag;
      
      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackCell(nOut.Pos,nOut.uVec,aDist,SPtr,SN);
      ELog::EM<<"Found exit Surf == "<<SN<<" "<<aDist<<ELog::endDiag;

      // Update Track : returns 1 on excess of distance
      if (SN && updateTrack(OPtr,SPtr,SN,aDist))
	{
	  ELog::EM<<"AA Dist == "<<aDist<<ELog::endDiag;
		  
	  prevOPtr=OPtr;
	  nOut.moveForward(aDist);
	  ELog::EM<<"AA NEUT == "<<nOut<<ELog::endDiag;
	  OPtr=OSMPtr->findNextObject(SN,nOut.Pos,OPtr->getName());

	  ELog::EM<<"Track == "<<nOut<<" "<<ELog::endDiag;
	  ELog::EM<<" ============== "<<ELog::endDiag;

	  if (OPtr==0)
	    {
	      // ALL FAILURE CODE:
	      const MonteCarlo::Object* OPtr=ASim.findCell(nOut.Pos,0);
	      ELog::EM<<"Common surf "<<SN<<ELog::endDiag;
	      if (OPtr)
		ELog::EM<<"Found CEll: "<<*OPtr<<ELog::endDiag;

	      ELog::EM<<"Initial point of line error:"<<InitPt<<ELog::endErr;
	      OPtr=OSMPtr->findNextObject(SN,nOut.Pos,prevOPtr->getName());
	      if (OPtr)
		{
		  nOut.moveForward(-1e-5);
		  ELog::EM<<"Object = "<<*OPtr<<ELog::endDiag;
		  const std::set<const Geometry::Surface*>& SV=
		    OPtr->getSurfPtrSet();
		  for(const Geometry::Surface* SPtr : SV)
		    if (SPtr->onSurface(nOut.Pos))
		      ELog::EM<<"Surf == "<<*SPtr<<ELog::endDiag;
		}
	      
	      ELog::EM<<"PrevObject = "<<*prevOPtr<<ELog::endDiag;
              
	      ELog::EM<<"Point = "<<nOut.Pos<<ELog::endDiag;
	      ELog::EM<<"DIR = "<<nOut.uVec<<ELog::endDiag;
	      ELog::EM<<"Init = "<<InitPt<<ELog::endDiag;
	      ELog::EM<<"Final = "<<EndPt<<ELog::endDiag;
              
	      ELog::EM<<ELog::endErr;
	    }

	  if (!OPtr || aDist<Geometry::zeroTol)
	    {
	      ELog::EM<<"ZERO CELL "<<ELog::endCrit;
	      OPtr=ASim.findCell(nOut.Pos,0);
	    }
	}
      else
	OPtr=0;
    }

  ELog::EM<<"Finished "<<ELog::endDiag;
  ELog::EM<<ELog::endCrit;
  return;
}

bool
LineTrack::updateTrack(MonteCarlo::Object* OPtr,
		       const Geometry::Surface* SPtr,
		       const int SN,
		       const double D) 
  /*!
    Add the distance, register cell etc
    \param OPtr :: Object points
    \param SPtr :: Surface pointer
    \param SN :: Surface number [pointing out]
    \param D :: Distance
    \return 1 if distance insufficient / 0 if at end of line
   */
{
  ELog::RegMethod RegA("LineTrack","updateTrack");

  if (aimDist-(TDist+D) < -Geometry::zeroTol)
    {
      LineUnit AUnit(OPtr,SN,SPtr,EndPt,aimDist-TDist);
      TDist=aimDist;
      trackPts.push_back(AUnit);
      return 0;
    }
  
  TDist+=D;
  if (trackPts.empty())
    trackPts.push_back(LineUnit(OPtr,SN,SPtr,
				InitPt+uVec*D,
				D));  
  else
    {
      trackPts.push_back
	(LineUnit(OPtr,SN,SPtr,
		  trackPts.back().exitPoint+uVec*D,
		  D));
    }
  return 1;
}

  
const Geometry::Vec3D& 
LineTrack::getPoint(const size_t Index) const
  /*!
    Get a point from the track
    \param Index :: Index point
    \return point that line crosses a cell boundary
  */
{
  ELog::RegMethod RegA("LineTrack","getPoint");

  if (Index>trackPts.size())
    throw ColErr::IndexError<size_t>
      (Index,trackPts.size()+1,"TrackPts / Index");

  if (!Index) return InitPt;

  return trackPts[Index-1].exitPoint;
}

void
LineTrack::createMatPath(std::vector<int>& mVec,
			 std::vector<double>& aVec) const
  /*!
    Calculate track components
    \param mVec :: Material vector
    \param aVec :: Attenuation  distance
  */
{
  ELog::RegMethod RegA("LineTrack ","createMatPath");

  for(const LineUnit& lu : trackPts)
    {
      const int matN=(!lu.objPtr) ? -1 : lu.objPtr->getMatID();
      mVec.push_back(matN);
      aVec.push_back(lu.segmentLength);
    }
  return;
}


void
LineTrack::populateObjMap(std::map<int,MonteCarlo::Object*>& OMap) const
/*!
  Populate a map with the cell number and the Object Points
  \param OMap :: map refrence
 */
{

  for(const LineUnit& lu : trackPts)
    {
      const int ONum=lu.cellNumber;
      if (OMap.find(ONum)==OMap.end())
	OMap.emplace(ONum,lu.objPtr);
    }
  return;
}

  
void
LineTrack::write(std::ostream& OX) const
  /*!
    Write out the track
    \param OX :: Output stream
  */
{

  OX<<"Start/End Pts == "<<InitPt<<"::"<<EndPt<<"\n";
  OX<<"------------------------------------- "<<"\n";
  double sumSigma(0.0);
  for(const LineUnit& lu : trackPts)
    {
      const MonteCarlo::Object* OPtr=lu.objPtr;
      const MonteCarlo::Material* MPtr=OPtr->getMatPtr();
      const double density=MPtr->getAtomDensity();
      const double A=MPtr->getMeanA();
      const double sigma=lu.segmentLength*density*std::pow(A,0.66);
      OX<<"  cell:"<<lu.cellNumber<<" == "<<lu.exitPoint
	<<" : "<<lu.segmentLength<<"\n";
      sumSigma+=sigma;
    }
  OX<<"------------------------------------- "<<"\n";
  OX<<"Total Len/sigma == "<<TDist<<" "<<sumSigma<<std::endl;
  return;
}

  
} // Namespace ModelSupport
