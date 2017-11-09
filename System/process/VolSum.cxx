/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/VolSum.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <memory>
#include <boost/format.hpp>

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
#include "LineTrack.h"
#include "volUnit.h"
#include "VolSum.h"

extern MTRand RNG;

namespace ModelSupport
{

VolSum::VolSum(const Geometry::Vec3D& OPt,
	       const Geometry::Vec3D& AxisRange) : 
  Origin(OPt),X(std::abs(AxisRange[0]),0,0),
  Y(0,std::abs(AxisRange[1]),0),Z(0,0,std::abs(AxisRange[2])),
  fullVol(0.0),totalDist(0),nTracks(0)
  /*!g
    Constructor
    \param OPt :: Centre
    \param AxisRange :: X/Y/Z extent
  */
{}

VolSum::VolSum(const VolSum& A) : 
  Origin(A.Origin),X(A.X),Y(A.Y),Z(A.Z),
  fracX(A.fracX),fracY(A.fracY),
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
      X=A.X;
      Y=A.Y;
      Z=A.Z;
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
VolSum::populateVSet(const Simulation& System,
		     const std::vector<int>& VSet)
  /*!
    Populate a set of cells
    \param System :: Simulation
    \param VSet :: Set of cells
   */
{
  ELog::RegMethod RegA("VolSum","populateVSet");
  typedef std::map<int,std::vector<int> > MTYPE;
  MTYPE MatSet;
  for(const int CN : VSet)
    {
      if (System.existCell(CN))
	{
	  const int matN=System.getCellMaterial(CN);
	  MTYPE::iterator mc=MatSet.find(matN);
	  if (mc!=MatSet.end())
	    mc->second.push_back(CN);
	  else
	    {
	      std::vector<int> Input;
	      Input.push_back(CN);
	      MatSet.insert(MTYPE::value_type(matN,Input));
	    }
	}
    }

  int cnt(1);
  for(const MTYPE::value_type& TC : MatSet)
    {  
      tallyVols.insert(tvTYPE::value_type
		       (cnt,volUnit(TC.first," A ",TC.second)));
      cnt++;
    }
  return;
}
  
void
VolSum::populateAll(const Simulation& System)
  /*!
    The big population call for all cell
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("VolSum","populateAll");
  const std::vector<int> CVec=System.getNonVoidCellVector();
  populateVSet(System,CVec);
  return;
}

  
void
VolSum::populateTally(const Simulation& System)
  /*!
    The big population call
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("VolSum","populateTally");
  
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
  fullVol=X.abs()*Y.abs()*Z.abs();
  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron

  for(size_t i=0;i<N;i++)
    { 
      Geometry::Vec3D Pt(Origin+
			 X*(RNG.rand()-0.5)+
			 Y*(RNG.rand()-0.5)+
			 Z*(RNG.rand()-0.5));
      OPtr=System.findCell(Pt,OPtr);
      addDistance(OPtr->getName(),1.0);
    }
  nTracks+=N;
  return;
}

Geometry::Vec3D
VolSum::getCubePoint() const
  /*!
    Get a random point on the cuboic
    \return surface point
  */
{
  double R=RNG.rand();
  double pm(0.5);
  if (R>0.5)   
    {
      pm=-0.5;
      R/=2.0;
    }
  Geometry::Vec3D Pt(Origin);
  if (R<fracX) // XY surface
    {
      Pt+=X*(RNG.rand()-0.5)+Y*(RNG.rand()-0.5);
      Pt+=Z*pm;
    }
  else if (R<fracY)
    {
      Pt+=X*(RNG.rand()-0.5)+Z*(RNG.rand()-0.5);
      Pt+=Y*pm;
    }
  else 
    {
      Pt+=Y*(RNG.rand()-0.5)+Z*(RNG.rand()-0.5);
      Pt+=X*pm;
    }
  return Pt;
}
  

  
void
VolSum::trackRun(const Simulation& System,const size_t N) 
  /*!
    Calculate the tracking 
    \param System :: Simulation to use
    \param N :: Number of points to test
  */
{
  ELog::RegMethod RegA("VolSum","run");
  
  reset();
  
  const double AreaXY(X[0]*Y[1]);
  const double AreaXZ(X[0]*Z[2]);
  const double AreaYZ(Y[1]*Z[2]);
  fullVol=X[0]*Y[1]*Z[2];

  const double totalArea(2.0*(AreaXY+AreaXZ+AreaYZ));

  fracX=AreaXY/totalArea;
  fracY=(AreaXZ+AreaXY)/totalArea;
  
  // Note for sphere that you can use X,Y,Z in any orthogonal 
  // directiron

  for(size_t i=0;i<N;i++)
    {
      const Geometry::Vec3D Pt=getCubePoint();
      const Geometry::Vec3D XPt=getCubePoint();


      LineTrack A(Pt,XPt);
      A.calculate(System);
      const std::vector<MonteCarlo::Object*>& OVec=A.getObjVec();
      const std::vector<double>& TVec=A.getTrack();
      for(size_t i=0;i<OVec.size();i++)
	{
	  const MonteCarlo::Object* OPtr=OVec[i];
	  if (OPtr)
	    addDistance(OPtr->getName(),TVec[i]);
	}

      const double trackDistance=Pt.Distance(XPt);
      totalDist+=trackDistance;
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
    return fullVol*mc->second.calcVol(1.0/static_cast<double>(nTracks));
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
  const double nT((nTracks) ? static_cast<double>(nTracks) : 1.0);
  for(mc=tallyVols.begin();mc!=tallyVols.end();mc++)
    {
      OX<<"tally"<<(FMTI3 % mc->first % 
		    (fullVol*mc->second.calcVol(1.0/nT)) %
		    sf % mc->second.getMat() % 
		    mc->second.getComment())<<std::endl;
      sf++;
    }
  OX.close();
  return;
}

} // NAMESPACE ModelSupport
