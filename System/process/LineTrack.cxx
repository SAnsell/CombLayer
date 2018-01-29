/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/LineTrack.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
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
      ObjVec=A.ObjVec;
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
  SurfVec.clear();
  SurfIndex.clear();
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
  
  while(OPtr)
    {
      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackOutCell(nOut,aDist,SPtr,abs(SN));
      // Update Track : returns 1 on excess of distance
      if (SN && updateDistance(OPtr,SPtr,SN,aDist))
	{
	  nOut.moveForward(aDist);
	  
	  OPtr=OSMPtr->findNextObject(SN,nOut.Pos,OPtr->getName());
	  if (!OPtr)
	    {
	      ELog::EM<<"INIT POINT[error] == "<<InitPt<<ELog::endDiag;
	      calculateError(ASim);
	    }
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
      ELog::EM<<std::setprecision(12)<<ELog::endDiag;
      ELog::EM<<"== Tracking in cell == "<<*OPtr;
      ELog::EM<<"Neutron == "<<nOut<<" "<<aDist<<ELog::endDiag;
      ELog::EM<<"SN at start== "<<SN<<ELog::endDiag;

      // Note: Need OPPOSITE Sign on exiting surface
      SN= OPtr->trackOutCell(nOut,aDist,SPtr,abs(SN));
      ELog::EM<<"Found exit Surf == "<<SN<<" "<<aDist<<ELog::endDiag;

      // Update Track : returns 1 on excess of distance
      if (SN && updateDistance(OPtr,SPtr,SN,aDist))
	{
	  prevOPtr=OPtr;
	  nOut.moveForward(aDist);
	  
	  OPtr=OSMPtr->findNextObject(SN,nOut.Pos,OPtr->getName());

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
	      if (prevOPtr)
		ELog::EM<<"PrevObject = "<<*prevOPtr<<ELog::endDiag;
              
	      ELog::EM<<"Point = "<<MR.calcRotate(nOut.Pos)<<ELog::endDiag;
	      ELog::EM<<"DIR = "<<nOut.uVec<<ELog::endDiag;
	      ELog::EM<<"Init = "<<MR.calcRotate(InitPt)<<ELog::endDiag;
	      ELog::EM<<"Final = "<<MR.calcRotate(EndPt)<<ELog::endDiag;
              
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
      Track.push_back(D-TDist+aimDist);
      return 0;
    }
  Track.push_back(D);
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
  ELog::RegMethod RegA("LineTrack","getPoint");

  if (Index>SurfVec.size())
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
  ELog::RegMethod RegA("LineTrack","getPoint");

  if (Index>SurfIndex.size())
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

  if (Index>Track.size())
    throw ColErr::IndexError<size_t>(Index,Track.size(),"Index");

  if (!Index) return InitPt;

  double Len=0.0;
  for(size_t i=0;i<Index;i++)
    Len+=Track[i];

  return InitPt+(EndPt-InitPt).unit()*Len;
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
  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  for(size_t i=0;i<Cells.size();i++)
    {
      const int matN=(!ObjVec[i]) ? -1 : ObjVec[i]->getMat();
      if (matN>0)
	{
	  const MonteCarlo::Material& matInfo=DB.getMaterial(matN);
	  const double density=matInfo.getAtomDensity();
	  const double A=matInfo.getMeanA();
	  const double sigma=Track[i]*density*std::pow(A,0.66);
	  cVec.push_back(ObjVec[i]->getName());
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

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  OX<<"Pts == "<<InitPt<<"::"<<EndPt<<std::endl;

  double sumSigma(0.0);
  for(size_t i=0;i<Cells.size();i++)
    {
      const int matN=(!ObjVec[i]) ? -1 : ObjVec[i]->getMat();
      const MonteCarlo::Material& matInfo=DB.getMaterial(matN);
      const double density=matInfo.getAtomDensity();
      const double A=matInfo.getMeanA();
      const double sigma=Track[i]*density*std::pow(A,0.66);
      OX<<"  "<<Cells[i]<<" : "<<Track[i]<<" "<<
	matN<<" "<<sigma<<" ("<<density*std::pow(A,0.66)<<")"<<std::endl;
      sumSigma+=sigma;
    }
  OX<<"Len == "<<TDist<<" "<<sumSigma<<std::endl;
  return;
}

  
} // Namespace ModelSupport
