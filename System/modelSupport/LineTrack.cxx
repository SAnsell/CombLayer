/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/LineTrack.cxx
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
  InitPt(IP),EndPt(EP),aimDist((EP-IP).abs()),
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
  InitPt(IP),EndPt(IP+UVec),
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
  InitPt(A.InitPt),EndPt(A.EndPt),aimDist(A.aimDist),TDist(A.TDist),
  Cells(A.Cells),ObjVec(A.ObjVec),segmentLen(A.segmentLen)
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
      ObjVec=A.ObjVec;
      segmentLen=A.segmentLen;
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
  SurfVec.clear();
  SurfIndex.clear();
  segmentLen.clear();
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
  const Geometry::Surface* SPtr(0);           // Surface
  const ModelSupport::ObjSurfMap* OSMPtr =ASim.getOSM();
  
  MonteCarlo::eTrack nOut(InitPt,EndPt-InitPt);
  // Find Initial cell [no default]
  MonteCarlo::Object* OPtr=ASim.findCell(InitPt,0);
  if (!OPtr)
    throw ColErr::InContainerError<Geometry::Vec3D>
      (InitPt,"Initial point not in model");
  

  int SN=OPtr->isOnSide(InitPt);
  if (SN && OPtr->trackDirection(InitPt,nOut.uVec)<0)
    {
      MonteCarlo::Object* prevOPtr(OPtr);
      OPtr = OSMPtr->findNextObject(-SN,nOut.Pos,OPtr->getName());
    }

  // problem is that SN will be on TWO objects
  // so which one is it? [it doesn't matter much]
  while(OPtr)
    {
      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackCell(nOut,aDist,SPtr,SN);
      // Update Track : returns 1 on excess of distance
      if (SN && updateDistance(OPtr,SPtr,SN,aDist))
	{
	  nOut.moveForward(aDist);
	  OPtr=OSMPtr->findNextObject(SN,nOut.Pos,OPtr->getName());
	  if (!OPtr)
	    {
	      ELog::EM<<"INIT POINT[error] == "<<InitPt<<ELog::endCrit;
	      calculateError(ASim);
	    }
	  if (!OPtr || aDist<Geometry::zeroTol)
	    OPtr=ASim.findCell(nOut.Pos,0);
	}
      else
	OPtr=0;	
    }
  //
  // remove last object if point does not reach it:
  //
  OPtr=(ObjVec.empty()) ? 0 : ObjVec.back();
  if (OPtr)
    {
      if (OPtr->trackDirection(EndPt,nOut.uVec)==-1)
	{
	  ObjVec.pop_back();
	  Cells.pop_back();
	}
    }
  return;
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

  MonteCarlo::eTrack nOut(InitPt,EndPt-InitPt);
  // Find Initial cell [no default]
  MonteCarlo::Object* OPtr=ASim.findCell(InitPt+
					 (EndPt-InitPt).unit()*1e-5,0);

  if (!OPtr)
    ColErr::InContainerError<Geometry::Vec3D>
      (InitPt,"Initial point not in model");

  const MonteCarlo::Object* prevOPtr(0);
  int SN=OPtr->isOnSide(InitPt);
  
  while(OPtr)
    {
      ELog::EM<<std::setprecision(12)<<ELog::endDiag;
      ELog::EM<<"== Tracking in cell == "<<*OPtr;
      ELog::EM<<"Track == "<<nOut<<" "<<aDist<<ELog::endDiag;
      ELog::EM<<"SN at start== "<<SN<<ELog::endDiag;

      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackCell(nOut,aDist,SPtr,SN);
      ELog::EM<<"Found exit Surf == "<<SN<<" "<<aDist<<ELog::endDiag;

      // Update Track : returns 1 on excess of distance
      if (SN && updateDistance(OPtr,SPtr,SN,aDist))
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
		  const std::vector<const Geometry::Surface*>& SV=
		    OPtr->getSurList();
		  for(size_t i=0;i<SV.size();i++)
		    if (SV[i]->onSurface(nOut.Pos))
		      ELog::EM<<"Surf == "<<*SV[i]<<ELog::endDiag;
		}
	      
	      ELog::EM<<"PrevObject = "<<*prevOPtr<<ELog::endDiag;
              
	      ELog::EM<<"Point = "<<nOut.Pos<<ELog::endDiag;
	      ELog::EM<<"DIR = "<<nOut.uVec<<ELog::endDiag;
	      ELog::EM<<"Init = "<<InitPt<<ELog::endDiag;
	      ELog::EM<<"Final = "<<EndPt<<ELog::endDiag;
              
	      ELog::EM<<ELog::endErr;
	    }

	  if (aDist<Geometry::zeroTol)
	    {
	      ELog::EM<<"ZERO CELL "<<ELog::endErr;
	      OPtr=ASim.findCell(nOut.Pos,0);
	    }
	}
      else
	OPtr=0;
    }

  ELog::EM<<"Finished "<<ELog::endDiag;
  ELog::EM<<ELog::endErr;
  return;
}

bool
LineTrack::updateDistance(MonteCarlo::Object* OPtr,
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
  ELog::RegMethod RegA("LineTrack","updateDistance");

  Cells.push_back(OPtr->getName());
  ObjVec.push_back(OPtr);
  SurfVec.push_back(SPtr);
  SurfIndex.push_back(SN);
  TDist+=D;
  if (aimDist-TDist < -Geometry::zeroTol)
    {
      segmentLen.push_back(D-TDist+aimDist);
      return 0;
    }
  segmentLen.push_back(D);
  return 1;
}


const Geometry::Surface*
LineTrack::getSurfPtr(const size_t Index) const
  /*!
    Get a surface from the track
    \param Index :: Index point
    \return surface index number
  */
{
  ELog::RegMethod RegA("LineTrack","getSurfPtr");

  if (Index>=SurfVec.size())
    throw ColErr::IndexError<size_t>(Index,SurfVec.size(),
				     "Index in SurfVec");
  return SurfVec[Index];
}

int
LineTrack::getSurfIndex(const size_t Index) const
  /*!
    Get a point from the track
    \param Index :: Index point
    \return surface index number
  */
{
  ELog::RegMethod RegA("LineTrack","getSurfIndex");

  if (Index>=SurfIndex.size())
    throw ColErr::IndexError<size_t>(Index,SurfIndex.size(),
				     "Index in SurfIndex");
  return SurfIndex[Index];
}
  
Geometry::Vec3D 
LineTrack::getPoint(const size_t Index) const
  /*!
    Get a point from the track
    \param Index :: Index point
    \return point that line crosses a cell boundary
  */
{
  ELog::RegMethod RegA("LineTrack","getPoint");

  if (Index>segmentLen.size())
    throw ColErr::IndexError<size_t>(Index,segmentLen.size(),"Index");

  if (!Index) return InitPt;

  double Len=0.0;
  for(size_t i=0;i<Index;i++)
    Len+=segmentLen[i];

  return InitPt+(EndPt-InitPt).unit()*Len;
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
  
  for(size_t i=0;i<Cells.size();i++)
    {
      const int matN=(!ObjVec[i]) ? -1 : ObjVec[i]->getMatID();
      mVec.push_back(matN);
      aVec.push_back(segmentLen[i]);
    }
  return;
}

void
LineTrack::createCellPath(std::vector<MonteCarlo::Object*>& cellVec,
			  std::vector<double>& aVec) const
  /*!
    Calculate track components
    \param cellVec :: Object Ptr vector
    \param aVec :: Track distance
  */
{
  for(size_t i=0;i<Cells.size();i++)
    {
      cellVec.push_back(ObjVec[i]);
      aVec.push_back(segmentLen[i]);
    }
  return;
}

void
LineTrack::createAttenPath(std::vector<long int>& cVec,
			   std::vector<double>& aVec) const
  /*!
    Calculate track components
    \param cVec :: Cell vector
    \param aVec :: Attenuation 
  */
{

  for(size_t i=0;i<ObjVec.size();i++)
    {
      const MonteCarlo::Object* OPtr=ObjVec[i];
      const MonteCarlo::Material* MPtr=OPtr->getMatPtr();
      if (!MPtr->isVoid())
	{
	  const double density=MPtr->getAtomDensity();
	  const double A=MPtr->getMeanA();
	  const double sigma=segmentLen[i]*density*std::pow(A,0.66);
	  cVec.push_back(OPtr->getName());
	  aVec.push_back(sigma);
	}
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
  for(size_t i=0;i<Cells.size();i++)
    {
      const MonteCarlo::Object* OPtr=ObjVec[i];
      const MonteCarlo::Material* MPtr=OPtr->getMatPtr();
      const double density=MPtr->getAtomDensity();
      const double A=MPtr->getMeanA();
      const double sigma=segmentLen[i]*density*std::pow(A,0.66);
      OX<<"  "<<Cells[i]<<" == "<<getPoint(i)
	<<" : "<<segmentLen[i]<<"\n";
      //	MPtr->getID()<<" "<<sigma<<" ("<<density*std::pow(A,0.66)<<")"<<std::endl;
      sumSigma+=sigma;
    }
  OX<<"------------------------------------- "<<"\n";
  OX<<"Total Len/sigma == "<<TDist<<" "<<sumSigma<<std::endl;
  return;
}

  
} // Namespace ModelSupport
