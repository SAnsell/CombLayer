/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/VolSum.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MersenneTwister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "ObjSurfMap.h"
#include "neutron.h"
#include "Simulation.h"
#include "volUnit.h"
#include "VolSum.h"

extern MTRand RNG;

namespace ModelSupport
{

VolSum::VolSum(const Geometry::Vec3D& OPt,
	       const double R) : 
  Origin(OPt),radius(R),fullVol(M_PI*R*R),
  totalDist(0),nTracks(0)
  /*!
    Constructor
    \param OPt :: Centre
    \param R :: Radius
  */
{}

VolSum::VolSum(const VolSum& A) : 
  Origin(A.Origin),radius(A.radius),
  fullVol(A.fullVol),totalDist(A.totalDist),
  nTracks(A.nTracks),tallyVols(A.tallyVols)
  /*!
    Copy constructor
    \param A :: VolSum to copy
  */
{}

VolSum&
VolSum::operator=(const VolSum& A)
  /*!
    Assignment operator
    \param A :: VolSum to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Origin=A.Origin;
      radius=A.radius;
      fullVol=A.fullVol;
      totalDist=A.totalDist;
      nTracks=A.nTracks;
      tallyVols=A.tallyVols;
    }
  return *this;
}


VolSum::~VolSum()
  /*!
    Destructor
   */
{}

void
VolSum::populateTally(const Simulation& System)
  /*!
    The big population call
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("VolSum","populate");
  
  const Simulation::TallyTYPE& TM=System.getTallyMap();
  Simulation::TallyTYPE::const_iterator mc;
  for(mc=TM.begin();mc!=TM.end();mc++)
    {
      const tallySystem::cellFluxTally* cellPtr=
	dynamic_cast<const tallySystem::cellFluxTally*>(mc->second);
      if (cellPtr)
	{
	  const int TN=cellPtr->getKey();
	  const std::vector<int> cells=cellPtr->getCells();
	  if (!cells.empty())
	    {
	      ELog::EM<<"Cells == "<<cells[0]<<ELog::endDebug;

	      const int matN=System.getCellMaterial(cells.front());
	      const std::string& comment=mc->second->getComment();
	      tallyVols.insert(tvTYPE::value_type
			       (TN,volUnit(matN,comment,cells)));	  
	    }
	}
    }
  return;
}

void
VolSum::addTally(const int TN,const int MatN,
		 const std::string& CM,const std::vector<int>& cells)
  /*!
    Add a tally
    \param TN :: Tally number
    \param MatN :: Material number
    \param CM :: Comment for tally
    \param cells :: Cell list
  */
{
  tallyVols.insert(tvTYPE::value_type(TN,volUnit(MatN,CM,cells)));	  
  return;
}

void
VolSum::addTallyCell(const int TN,const int CN)
  /*!
    Add a tally based on cell and tally number
    \param TN :: Tally name
    \param CN :: Cell number
   */
{
  ELog::RegMethod RegA("VolSum","addTallyCell");

  tvTYPE::iterator mc=tallyVols.find(TN);
  if (mc!=tallyVols.end())
    {
      mc->second.addCell(CN);
      return;
    }
  std::pair<tvTYPE::iterator,bool> PItem=
    tallyVols.insert(tvTYPE::value_type(TN,volUnit()));
  PItem.first->second.addCell(CN);
  return;
}

void
VolSum::addDistance(const int ObjN,const double D)
  /*!
    Adds distance to all the objects
    \param ObjN :: Object number
    \param D :: Distance to add to tally calc
   */
{
  // Change to a boost::bind
  tvTYPE::iterator mc;
  for(mc=tallyVols.begin();mc!=tallyVols.end();mc++)
    mc->second.addUnit(ObjN,D);
  return;
}

void
VolSum::addFlux(const int ObjN,const double& R,const double& D)
  /*!
    Adds distance to all the objects
    \param ObjN :: Object Number
    \param R :: Inital R distance
    \param D :: Distance to add to tally calc
   */
{
  // Change to a boost::bind
  tvTYPE::iterator mc;
  for(mc=tallyVols.begin();mc!=tallyVols.end();mc++)
    mc->second.addFlux(ObjN,R,D);
  return;
}

void
VolSum::reset()
  /*!
    Reset everthing
   */
{
  // Change to a boost::bind
  std::map<int,volUnit>::iterator mc;
  for(mc=tallyVols.begin();mc!=tallyVols.end();mc++)
    mc->second.reset();
  nTracks=0;
  totalDist=0.0;
  return;
}

void
VolSum::pointRun(const Simulation& System,const size_t N) 
  /*!
    Calculate the tracking
    \param System :: Simulation to use
    \param N :: Number of points to test
  */
{
  ELog::RegMethod RegA("VolSum","run");
  
  MonteCarlo::Object* OPtr(0);
  reset();
  fullVol=4.0*M_PI*(radius*radius*radius)/3.0;
  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron
  double phi,theta;
  Geometry::Vec3D XPt;
  for(size_t i=0;i<N;i++)
    {
      // Get random starting point on edge of volume
      phi=RNG.rand()*M_PI;
      theta=2.0*RNG.rand()*M_PI;
      Geometry::Vec3D Pt(cos(theta)*sin(phi),
			 sin(theta)*sin(phi),
			 cos(phi));
      // Deal with scaling
      Pt*=radius*pow(RNG.rand(),0.3333333333);

      OPtr=System.findCell(Pt,OPtr);      
      addDistance(OPtr->getName(),1.0);
    }
  nTracks+=N;
  return;
}

void
VolSum::run(const Simulation& System,const size_t N) 
  /*!
    Calculate the tracking
    \param System :: Simulation to use
    \param N :: Number of points to test
  */
{
  ELog::RegMethod RegA("VolSum","run");
  
  const ModelSupport::ObjSurfMap* OSMPtr =System.getOSM();
  MonteCarlo::Object* InitObj(0);
  reset();
  fullVol=M_PI*radius*radius;
  const Geometry::Surface* SPtr;          // Output surface
  double aDist;       

  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron
  double phi,theta;
  Geometry::Vec3D XPt;
  for(size_t i=0;i<N;i++)
    {
      // Get random starting point on edge of volume
      phi=RNG.rand()*M_PI;
      theta=2.0*RNG.rand()*M_PI;
      Geometry::Vec3D Pt(cos(theta)*sin(phi),
			 sin(theta)*sin(phi),
			 cos(phi));
      // Random point on sphere:
      // Choose an direction (make it normal to the sphere):
      phi=RNG.rand()*M_PI;
      theta=2.0*RNG.rand()*M_PI;
      XPt=Geometry::Vec3D(cos(theta)*sin(phi),
			  sin(theta)*sin(phi),
			  cos(phi));
      double trackDistance=radius*Pt.Distance(XPt);
      XPt-=Pt;

      // track length to go == [Max]
      // A is on the sphere and lambda then is l=-2(A.u)
      // Note aDv is negative

      totalDist+=trackDistance;
      
      MonteCarlo::neutron TNeut(1,Origin+Pt*radius,XPt);

      // Find Initial cell [Store for next time]
      InitObj=System.findCell(TNeut.Pos,InitObj);      
      MonteCarlo::Object* OPtr=InitObj;
      int SN(0);

      while(OPtr)
	{
	  // Note: Need OPPOSITE Sign on exiting surface
	  SN= -OPtr->trackOutCell(TNeut,aDist,SPtr,-SN);
	  trackDistance-=aDist;
	  if (trackDistance > 0.0)
	    {
	      addDistance(OPtr->getName(),aDist);
	      trackDistance-=aDist;
	      TNeut.moveForward(aDist+2.0*Geometry::zeroTol);

	      OPtr=(SN) ?
		OSMPtr->findNextObject
		(SN,TNeut.Pos,OPtr->getName()) : 0;		
	    }
	  else
	    {
	      //	      ELog::EM<<"Adding "<<
	      //		trackDistance-TNeut.travel<<ELog::endDebug;
	      //	      addDistance(OPtr->getName(),
	      //			  trackDistance-TNeut.travel);
	      OPtr=0;
	    }
	}
    }
  ELog::EM<<"Total Dist == "<<totalDist<<ELog::endTrace;  
  nTracks+=N;
  return;
}

double
VolSum::calcVolume(const int TN) const
  /*!
    Calcuate the volume of a tally unit
    \param TN :: Tally number
    \return Volume
   */
{
  ELog::RegMethod RegA("VolSum","calcVolume");

  if (nTracks<1) return 0.0;
  std::map<int,volUnit>::const_iterator mc;
  mc=tallyVols.find(TN);
  if (mc!=tallyVols.end())
    return fullVol*mc->second.calcVol(1.0/nTracks);
  ELog::EM<<"No tally of value "<<TN<<ELog::endErr;
  return 0.0;
}


void 
VolSum::write(const std::string& OFile) const
  /*!
    Write out the data				
    \param OFile :: Output File
  */
{
  ELog::RegMethod RegA("VolSum","write");
  boost::format FMTI3("%3d  %11.5e %c  mat%3d %s");
  
  std::ofstream OX(OFile.c_str());
  
  OX<<"FluxName   Volume(cc)  Sf Matrl  Description"<<std::endl;
  OX<<"========  ============ == ====== "
    <<"================================================ "<<std::endl;

  char sf='a';  
  tvTYPE::const_iterator mc;
  for(mc=tallyVols.begin();mc!=tallyVols.end();mc++)
    {
      OX<<"tally"<<(FMTI3 % mc->first % 
		    (fullVol*mc->second.calcVol(1.0/nTracks)) %
		    sf % mc->second.getMat() % 
		    mc->second.getComment())<<std::endl;
      sf++;
    }
  OX.close();
  return;
}

} // NAMESPACE ModelSupport
