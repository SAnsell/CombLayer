/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimMonte.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <memory>
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <stack>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>

#include "MersenneTwister.h"
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "BaseVisit.h"
#include "Vec3D.h"
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
#include "neutMaterial.h"

#include "Beam.h"
#include "particle.h"
#include "neutron.h"
#include "ParticleInObj.h"
#include "Detector.h"
#include "DetGroup.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineTrack.h"
#include "SimMonte.h"

extern MTRand RNG;

SimMonte::SimMonte() :
  Simulation(),TCount(0),MSActive(0),B(0),DUnit()
  /*!
    Start of simulation Object
    Initialise currentSample to Sample 
  */
{}

SimMonte::SimMonte(const SimMonte& A)  :
  Simulation(A),
  TCount(A.TCount),MSActive(A.MSActive),
  B((A.B) ? A.B->clone() : 0),
  DUnit(A.DUnit)
  /*!
    Copy constructor:: makes a deep copy of the SurMap 
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
  */
{}


SimMonte&
SimMonte::operator=(const SimMonte& A) 
  /*!
    Assignment operator :: makes a deep copy of the SurMap 
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      delete B;
      B=(A.B) ? A.B->clone() : 0;
      DUnit=A.DUnit;
    }
  return *this;
}

SimMonte::~SimMonte()
  /*!
    Destructor :: deletes the memory in the SurMap 
    list
  */
{
  delete B;
}

void
SimMonte::clearAll()
 /*!
   Thermonuclear distruction of the whole system
 */
{
  TCount=0;
  DUnit.clear();
  return;
}


void 
SimMonte::setBeam(const Transport::Beam& BObj) 
  /*!
    Set the beam pointer
    \param BObj :: Point to a beam implementation
  */
{
  delete B;
  B=BObj.clone();
  return;
}


void
SimMonte::setDetector(const Transport::Detector& DObj)
  /*!
    Set the detector
    \param DObj :: Detector object to add
  */
{
  ELog::RegMethod RegA("SimMonte","setDetector");
  DUnit.addDetector(DObj);
  return;
}

void
SimMonte::attenPath(const MonteCarlo::Object* startObj,
		    const double Dist,MonteCarlo::neutron& N) const
  /*!
    Calculate and update the particle path staring
    from N through a distance 
    \param startObj :: Initial object [for recording track]
    \param Dist :: Distance to travel
    \param N :: Particle to track
   */
{
  ELog::RegMethod RegA("SimMonte","attenPath");

  ModelSupport::LineTrack LT(N.Pos,N.Pos+N.uVec*Dist);
  LT.calculate(*this);

  const std::vector<double>& tLen=LT.getSegmentLen();
  const std::vector<MonteCarlo::Object*>& objVec=LT.getObjVec();
  
  for(size_t i=0;i<objVec.size();i++)
    {
      const MonteCarlo::Object* OPtr=objVec[i];
      const MonteCarlo::Material* matPtr=OPtr->getMatPtr();
      N.weight*=matPtr->calcAtten(N,tLen[i]);
    }
  N.setObject(startObj);
  N.moveForward(Dist);
  return;
}

 
void
SimMonte::runMonteNeutron(const size_t Npts)
  /*!
    Run a specific number 
    \param Npts :: number of points
  */
{
  static MonteCarlo::Object* defObj(0);

  ELog::RegMethod RegA("SimMonte","runMonte");
  
  //  const int aim((Npts>10) ? Npts/10 : 1);
  const Geometry::Surface* surfPtr;

  MonteCarlo::neutron Nout(0.0,Geometry::Vec3D(0,0,0),
			       Geometry::Vec3D(1,0,0));
  const ModelSupport::ObjSurfMap* OSMPtr =getOSM();

  //  double tDist;  // Track disnace 
  //  Geometry::Surface* SPtr;  // Exit surface


  const size_t Nten((Npts>10) ? Npts/10 : 1);
  for(size_t i=0;i<Npts;i++)
    {
      if (!(i % Nten))
	ELog::EM<<"i == "<<i<<ELog::endDiag;
      try
	{
	  // No material info at this point:
	  MonteCarlo::neutron n=B->generateNeutron();
	  // Note the double loop : 
	  //    -- A to track to scatter point [outer]
	  //    -- B to track to track length point [inner]

	  const MonteCarlo::Object* OPtr=this->findCell(n.Pos,defObj);
	  while (OPtr && !OPtr->isZeroImp())
	    {
	      Transport::ParticleInObj<MonteCarlo::neutron> Cell(OPtr);
	      double R=RNG.randExc();
	      // Calculate forward Track:
	      int surfN;
	      surfN=Cell.trackWeight(n,R,surfPtr);
	      if (surfN)  
		OPtr=OSMPtr->findNextObject(surfN,n.Pos,
					    OPtr->getName());
	      else         // Internal scatter : Get new R
		{
		  //Cell.selectEnergy(n,Nout);		  
		  // Internal scatter : process fraction to detector
		  if (!MSActive || (MSActive<0 && n.nCollision==0)
		      || (MSActive>0 && n.nCollision!=0))
		    {
		      for(size_t i=0;i<DUnit.NDet();i++)
			{
			  Transport::Detector* DPtr=DUnit.getDet(i);
			  // To sample you need : 
			  // Direction / solid angle / dsigma/domega
			  const double RDist=
			    DPtr->project(n,Nout);   
			  // Object
			  Nout.weight*=Cell.scatTotalRatio(n,Nout);
			  // ATTENUATE:
			  attenPath(OPtr,RDist,Nout);
			  DPtr->addEvent(Nout);
			}
		    }
		  n.weight*=Cell.scatTotalRatio(n,Nout);
		  //		  nMat->scatterNeutron(n);
		}
	    }
	}
      catch (ColErr::NumericalAbort& A)
	{
	  ELog::EM<<"Failed at point :"<<i<<ELog::endCrit;
	  ELog::EM<<"From :"<<A.what()<<ELog::endCrit;
	}
    }
  ELog::EM<<"Tcount == "<<TCount<<" "<<Npts<<ELog::endDiag;
  TCount+=Npts;
  return;
}

void
SimMonte::normalizeDetectors()
  /*!
    Normalize the detctors relative to the incident count
   */
{
  DUnit.normalizeDetectors(TCount);
  return;
}

void
SimMonte::writeDetectors(const std::string& outName,
			 const double ) const
  /*!
    Write out a summary of the SimMonte
    \param outName :: Output head name
    \param lambda :: Output wavelength bin
  */
{
  const std::string DetectorFile=outName+".det";
  std::ofstream OX;
  OX.open(DetectorFile.c_str());
  DUnit.write(OX);
  OX.close();
  return;
}

void
SimMonte::write(const std::string& outName) const
  /*!
    Write the informaton to a file
    \param outName :: head name
  */
{
  ELog::EM<<"SimMonte not writing .x file currently"<<ELog::endDiag;
  
  writeDetectors(outName,1.0);
  return;
}

