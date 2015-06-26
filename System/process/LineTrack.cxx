/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/LineTrack.cxx
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
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
#include "ObjTrackItem.h"
#include "neutron.h"
#include "Simulation.h"
#include "LineTrack.h"

#include "debugMethod.h"

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
  InitPt(IP),EndPt(EP),aimDist((EP-IP).abs()),
  TDist(0.0)
  /*! 
    Constructor 
    \param IP :: Initial point
    \param EP :: End point
  */
{}

LineTrack::LineTrack(const LineTrack& A) : 
  InitPt(A.InitPt),EndPt(A.EndPt),aimDist(A.aimDist),TDist(A.TDist),
  Cells(A.Cells),ObjVec(A.ObjVec),Track(A.Track)
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
      TDist=A.TDist;
      Cells=A.Cells;
      Track=A.Track;
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
  Cells.clear();
  ObjVec.clear();
  Track.clear();
  return;
}

void
LineTrack::calculate(const Simulation& ASim)
  /*!
    Calculate the track
    \param ASim :: Simulation to use						
  */
{
  ELog::RegMethod RegA("LineTrack","calculate");

  double aDist(0);                         // Length of track
  const Geometry::Surface* SPtr;           // Surface
  const ModelSupport::ObjSurfMap* OSMPtr =ASim.getOSM();

  MonteCarlo::neutron nOut(1.0,InitPt,EndPt-InitPt);
  // Find Initial cell [no default]
  MonteCarlo::Object* OPtr=ASim.findCell(InitPt+
					 (EndPt-InitPt).unit()*1e-5,0);

  if (!OPtr)
    ELog::EM<<"Initial point not in model:"<<InitPt<<ELog::endErr;
  int SN=OPtr->isOnSide(InitPt);
  
  const MonteCarlo::Object* prevOPtr(0);
  while(OPtr)
    {
      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackOutCell(nOut,aDist,SPtr,abs(SN));
      // Update Track : returns 1 on excess of distance
      if (SN && updateDistance(OPtr,aDist))
	{
	  prevOPtr=OPtr;
	  nOut.moveForward(aDist);
	  
	  OPtr=OSMPtr->findNextObject(SN,nOut.Pos,OPtr->getName());
	  //	  if (OPtr==0)
	  //	    calculateError(ASim);
	  if (!OPtr || aDist<Geometry::zeroTol)
	    OPtr=ASim.findCell(nOut.Pos,0);
	}
      else
	OPtr=0;	
    }
  return;
}

void
LineTrack::calculateError(const Simulation& ASim)
  /*!
    Calculate the track
    \param ASim :: Simulation to use						
  */
{
  ELog::RegMethod RegA("LineTrack","calculate");
  ELog::debugMethod DegA;
  ELog::EM<<"START OF ERROR CODE"<<ELog::endDiag;
  ELog::EM<<"START OF ERROR CODE"<<ELog::endDiag;
  ELog::EM<<"-------------------"<<ELog::endDiag;
  
  double aDist(0);                         // Length of track
  const Geometry::Surface* SPtr;           // Surface
  const ModelSupport::ObjSurfMap* OSMPtr =ASim.getOSM();

  MonteCarlo::neutron nOut(1.0,InitPt,EndPt-InitPt);
  // Find Initial cell [no default]
  MonteCarlo::Object* OPtr=ASim.findCell(InitPt+
					 (EndPt-InitPt).unit()*1e-5,0);

  if (!OPtr)
    ELog::EM<<"Initial point not in model:"<<InitPt<<ELog::endErr;
  const MonteCarlo::Object* prevOPtr(0);
  int SN=OPtr->isOnSide(InitPt);
  
  while(OPtr)
    {
      ELog::EM<<"== Tracking cell == "<<*OPtr;
      ELog::EM<<"Neutron == "<<nOut<<" "<<aDist<<ELog::endDiag;
      ELog::EM<<"SN == "<<SN<<ELog::endDiag;

      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackOutCell(nOut,aDist,SPtr,abs(SN));
      ELog::EM<<"Found Surf == "<<SN<<" "<<aDist<<ELog::endDiag;

      // Update Track : returns 1 on excess of distance
      if (SN && updateDistance(OPtr,aDist))
	{
	  prevOPtr=OPtr;
	  nOut.moveForward(aDist);
	  
	  OPtr=OSMPtr->findNextObject(SN,nOut.Pos,OPtr->getName());

	  if (OPtr)
	    ELog::EM<<"Tracking cell == "<<*OPtr<<ELog::endDiag;
	  else
	    ELog::EM<<"void cell"<<ELog::endDiag;
	  ELog::EM<<"Neutron == "<<nOut<<" "<<ELog::endDiag;
	  ELog::EM<<" ============== "<<ELog::endDiag;


	  if (OPtr==0)
	    {
	      const masterRotate& MR=masterRotate::Instance();
	      // ALL FAILURE CODE:
	      const MonteCarlo::Object* OPtr=ASim.findCell(nOut.Pos,0);
	      ELog::EM<<"Common surf "<<SN<<ELog::endDiag;
	      if (OPtr)
		ELog::EM<<"Found CEll: "<<*OPtr<<ELog::endDiag;
	      OPtr=OSMPtr->findNextObject(SN,nOut.Pos,prevOPtr->getName());
	      if (OPtr)
		{
		  nOut.moveForward(-1e-5);
		  ELog::EM<<"Object = "<<*OPtr<<ELog::endDiag;
		  const std::vector<const Geometry::Surface*>& SV=
		    OPtr->getSurList();
		  for(size_t i=0;i<SV.size();i++)
		    if (SV[i]->onSurface(nOut.Pos))
		      ELog::EM<<"Surf == "<<*SV[i]<<ELog::endDiag;

		}
	      if (prevOPtr)
		ELog::EM<<"PrevObject = "<<*prevOPtr<<ELog::endDiag;
	      ELog::EM<<"Point = "<<MR.calcRotate(nOut.Pos)<<ELog::endDiag;
	      ELog::EM<<"DIR = "<<nOut.uVec<<ELog::endDiag;
	      ELog::EM<<"Init = "<<MR.calcRotate(InitPt)<<ELog::endDiag;
	      ELog::EM<<"Final = "<<MR.calcRotate(EndPt)<<ELog::endDiag;
	      ELog::EM<<ELog::endErr;
	    }

	  if (aDist<Geometry::zeroTol)
	    OPtr=ASim.findCell(nOut.Pos,0);
	}
      else
	OPtr=0;
	
    }
  return;
}

bool
LineTrack::updateDistance(MonteCarlo::Object* OPtr,const double D) 
  /*!
    Add the distance, register cell etc
    \param OPtr :: Object points
    \param D :: Distance
    \return 1 if distance insufficient / 0 if at end of line
   */
{
  ELog::RegMethod RegA("LineTrack","updateDistance");

  Cells.push_back(OPtr->getName());
  ObjVec.push_back(OPtr);
  TDist+=D;
  if (aimDist-TDist < -Geometry::zeroTol)
    {
      Track.push_back(D-TDist+aimDist);
      return 0;
    }

  Track.push_back(D);
  return 1;
}

Geometry::Vec3D 
LineTrack::getPoint(const size_t Index) const
  /*!
    Get a point from the track
    \param Index :: Index poitn
    \return point that line crosses a cell boundary
  */
{
  ELog::RegMethod RegA("LineTrack","getPoint");

  if (Index>Track.size())
    throw ColErr::IndexError<size_t>(Index,Track.size(),"Index");

  if (!Index) return InitPt;

  double Len=0.0;
  for(size_t i=1;i<Index;i++)
    {
      Len+=Track[Index-1];
      if (fabs(Track[Index-1])<Geometry::zeroTol)
	ELog::EM<<"Point "<<Index-1<<" == "<<Track[Index-1]<<ELog::endDebug;
    }

  return InitPt+(EndPt-InitPt).unit()*Len;
}

void
LineTrack::write(std::ostream& OX) const
  /*!
    Write out the track
    \param OX :: Output stream
  */
{
  OX<<"Pts == "<<InitPt<<"::"<<EndPt<<std::endl;
  for(size_t i=0;i<Cells.size();i++)
    OX<<"  "<<Cells[i]<<" : "<<Track[i]<<std::endl;
  return;
}

  
} // Namespace ModelSupport
