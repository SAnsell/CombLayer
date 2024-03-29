/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/BandDetector.cxx
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
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "RefCon.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "Random.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "particle.h"
#include "Detector.h"
#include "BandDetector.h"

namespace Transport
{


BandDetector::BandDetector() : 
  Detector(0),nH(1),nV(1),nE(1),Cent(Geometry::Vec3D(0,0,0)),
  H(Geometry::Vec3D(1,0,0)),V(Geometry::Vec3D(0,0,1)),
  hSize(1.0),vSize(1.0),
  PlnNorm(Geometry::Vec3D(0,1,0)),PlnDist(0.0),
  EData(1,1,1)
  /*!
    Default constructor
    Note that Detector index is unused
  */
{
}

BandDetector::BandDetector(const size_t Hpts,
			   const size_t Vpts,
			   const size_t Epts,
			   Geometry::Vec3D  CV,
			   const Geometry::Vec3D& Hvec,
			   const Geometry::Vec3D& Vvec,
			   const double ES,const double EE) : 
  Detector(0),nH(Hpts),nV(Vpts),nE((Epts>0) ? Epts : 1),
  Cent(std::move(CV)),H(Hvec.unit()),V(Vvec.unit()),
  hSize(Hvec.abs()),vSize(Vvec.abs()),
  PlnNorm((V*H).unit()),PlnDist(Cent.dotProd(PlnNorm)),
  EData(nV,nH,nE)
 /*!
   Constructor 
   \param Hpts :: Number of horrizontal bins
   \param Vpts :: Number of vertical bins
   \param Epts :: Number of Energy binds [0 for all energies]
   \param CV :: Centre Point (middle)
   \param Vvec :: Vertical vector
   \param Hvec :: Horrizontal vector
   \param ES :: Energy start [-ve for wavelength ]
   \param EE :: Energy end [-ve for wavelength 
  */
{
  for(size_t i=0;i<nV;i++)
    for(size_t j=0;j<nH;j++)
      for(size_t k=0;k<nE;k++)
	EData.get()[i][j][k]=0.0;
  if (Epts>0)
    setEnergy(ES,EE);
}

BandDetector::BandDetector(const BandDetector& A) : 
  Detector(A),nH(A.nH),nV(A.nV),nE(A.nE),Cent(A.Cent),H(A.H),
  V(A.V),hSize(A.hSize),vSize(A.vSize),PlnNorm(A.PlnNorm),
  PlnDist(A.PlnDist),EGrid(A.EGrid),EData(A.EData)
  /*!
    Copy constructor
    \param A :: BandDetector to copy
  */
{}

BandDetector&
BandDetector::operator=(const BandDetector& A)
  /*!
    Assignment operator
    \param A :: BandDetector to copy
    \return *this
  */
{
  ELog::RegMethod RegA("BandDetector","operator=");
  if (this!=&A)
    {
      Detector::operator=(A);
      nH=A.nH;
      nV=A.nV;
      nE=A.nE;
      Cent=A.Cent;
      H=A.H;
      V=A.V;
      hSize=A.hSize;
      vSize=A.vSize;
      PlnNorm=A.PlnNorm;
      PlnDist=A.PlnDist;
      EGrid=A.EGrid;
      EData=A.EData;
    }
  return *this;
}

BandDetector::~BandDetector()
  /*!
    Destructor
  */
{}

void
BandDetector::clear()
  /*!
    Clear all the array
  */
{
  nps=0;
  for(size_t i=0;i<nV;i++)
    for(size_t j=0;j<nH;j++)
      for(size_t k=0;k<nE;k++)
	EData.get()[i][j][k]=0.0;
  return;
}
		       
void
BandDetector::setCentre(const Geometry::Vec3D& Cp)
  /*!
    Set the detector Positon
    \param Cp :: Plane centre
   */
{
  Cent=Cp;
  PlnDist=Cent.dotProd(PlnNorm);
  return;
}

void
BandDetector::setEnergy(const double ES,const double EE)
  /*!
    Set the detector Positon
    \param ES :: start Energy [eV] / -ve Wavelength [Angstrom]
    \param EE :: end Energy [eV] / -ve Wavelength [Angstrom]
   */
{
  //  const double E((0.5*RefCon::h2_mneV*1e20)/(W*W)); 

  ELog::RegMethod RegA("BandDetector","setEnergy");
  double engA((ES>=0) ? ES :  
	      (0.5*RefCon::h2_mneV*1e20)/(ES*ES));  
  double engB((EE>=0) ? EE :  
	      (0.5*RefCon::h2_mneV*1e20)/(EE*EE));  

  if (engA>engB)
    std::swap(engA,engB);
  
  if (std::abs(engB-engA)<1e-7)
    {
      ELog::EM<<"Setting energy gap--too small"<<ELog::endWarn;
      return;
    }
  const double step((engB-engA)/static_cast<double>(nE));
  EGrid.clear();
  for(size_t i=0;i<=nE;i++)
    EGrid.push_back(engA+static_cast<double>(i)*step);
  return;
}

void
BandDetector::setDataSize(const size_t Hpts,
			  const size_t Vpts,
			  const size_t Epts)
  /*!
    Set the data Size
    \param Hpts :: Horizontal size
    \param Vpts :: Vertical size 
    \param Epts :: Energy size
   */
{
  if (Hpts>0 && Vpts>0 &&
      (Hpts!=nH || Vpts!=nV || Epts!=nE) )
    {
      nH=Hpts;
      nV=Vpts;
      nE=(Epts>0) ? Epts : 1;
      EData.resize(nV,nH,nE);
      clear();
    }
  return;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
BandDetector::getAxis() const
  /*!
    Access the axises
    \return H,V pair scaled by vSize/hSize
   */
{
  return std::pair<Geometry::Vec3D,
    Geometry::Vec3D>(H*hSize,V*vSize);
}

int
BandDetector::calcCell(const MonteCarlo::particle& N,
		       size_t& NH,size_t& NV) const
  /*!
    Calc a cell
    Tracks from the point to the detector.
    Debug method only.
    \param N :: Particle
    \param NH :: horrizontal position
    \param NV :: vertical position
    \return 1 on hit / 0 on a miss
  */
{
  ELog::RegMethod RegA("BandDetector","calcCell");

  const double OdotN=N.Pos.dotProd(PlnNorm);
  const double DdotN=N.uVec.dotProd(PlnNorm);
  if (std::abs(DdotN)<Geometry::parallelTol)     // Plane and line parallel
    return 0;
  const double u=(PlnDist-OdotN)/DdotN;
  Geometry::Vec3D Pnt=N.Pos+N.uVec*u;
  // Now determine if Pnt is within detector:
  Pnt-=(Cent-H*0.5*hSize-V*0.5*vSize);
  const double hFrac(Pnt.dotProd(H)/hSize);
  const double vFrac(Pnt.dotProd(V)/vSize);
  if (hFrac<0.0 || hFrac>=1.0 || vFrac<0.0 || vFrac>1.0)
    return 0;
  NH=static_cast<size_t>(static_cast<double>(nH)*hFrac);
  NV=static_cast<size_t>(static_cast<double>(nV)*vFrac);

  ELog::EM<<"Point == "<<Pnt<<" ("<<NH<<","<<NV<<")"<<ELog::endDebug;
  ELog::EM<<"Det == "<<Cent<<" ("<<H<<","<<V<<")::"<<PlnNorm<<ELog::endDebug;
  ELog::EM<<"U == "<<u<<" "<<PlnDist<<" "<<hSize<<" "<<vSize<<ELog::endDebug;
  ELog::EM<<ELog::endDebug;
  return 1;
}


void
BandDetector::addEvent(const MonteCarlo::particle& N) 
  /*!
    Add a point to the detector
    Tracks from the point to the detector.
    Added correction for solid angle
    \param N :: Particle
  */
{
  ELog::RegMethod RegA("BandDetector","addEvent");

  const double OdotN=N.Pos.dotProd(PlnNorm);
  const double DdotN=N.uVec.dotProd(PlnNorm);
  if (fabs(DdotN)<Geometry::parallelTol)     // Plane and line parallel
    return;

  const double u=(PlnDist-OdotN)/DdotN;
  Geometry::Vec3D Pnt=N.Pos+N.uVec*u;
  // Now determine if Pnt is within detector:
  Pnt-=(Cent-H*(hSize/2.0)-V*(vSize/2.0));
  const size_t hpt=static_cast<size_t>
    (static_cast<double>(nH)*Pnt.dotProd(H)/hSize);
  const size_t vpt=static_cast<size_t>
    (static_cast<double>(nV)*Pnt.dotProd(V)/vSize);
  if (hpt>=nH || vpt>=nV) return;
  // Scale for (i) distance to scattering point:
  //           (ii) solid angle
  // Distance is u + travel
  const size_t ePoint=calcWavePoint(N.wavelength);
  EData.get()[vpt][hpt][ePoint]+=
    N.weight/((N.travel+u)*(N.travel+u)*std::abs(DdotN));
  nps++;

  return;
}

size_t
BandDetector::calcWavePoint(const double W) const
  /*!
    Given the wavelength calculate the 
    point in the EGrid set
    \param W :: Wavelength [A]
    \return EGrid cell
  */
{
  ELog::RegMethod RegA("BandDetector","calcWavePoint");

  if (EGrid.empty()) return 0;
  const double E((0.5*RefCon::h2_mneV*1e20)/(W*W)); 
  const size_t res=rangePos(EGrid,E);
  return res;
}

size_t
BandDetector::calcEnergyPoint(const double E) const
  /*! 
    Given the wavelength calculate the 
    point in the EGrid set
    \param E :: Energy [eV]
    \return EGrid cell
  */
{
  if (EGrid.empty()) return 0;
  return rangePos(EGrid,E);
}

double
BandDetector::project(const MonteCarlo::particle& Nin,
		  MonteCarlo::particle& Nout) const
  /*!
    Project the neutron into the detector.
    The output neutron is copied from Nin and then
    pointed to a random 
    \param Nin :: Original neutron
    \param Nout :: secondary neutron
    \return distance moved by neutron to detector
  */
{
  ELog::RegMethod RegA("BandDetector","project");

  Nout=Nin;
  const Geometry::Vec3D Pt=getRandPos();
  Nout.uVec=(Pt-Nin.Pos).unit();
  Nout.travel=0.0;
  Nout.addCollision();
  return Nin.Pos.Distance(Pt);
}

Geometry::Vec3D
BandDetector::getRandPos() const
  /*!
    Generate a random position on the detector surface
    \return Vector Position
  */
{
  return Cent+H*hSize*(0.5-Random::rand())+
    V*vSize*(0.5-Random::rand());
}

void
BandDetector::write(std::ostream& OX) const
  /*!
    Write the detector to a stream
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("BandDetector","write(stream,double)");

  const int EBin(1);
  if (EBin<0 || EBin>=static_cast<long int>(EData.shape()[2]))
    {
      if (EGrid.empty())
	ELog::EM<<"EData is empty"<<ELog::endErr;
      else
	ELog::EM<<"Failed to find energy within range :"
	      <<EGrid.front()<<" "<<EGrid.back()<<ELog::endErr;
      return;
    }
  
  ELog::EM<<"EBin == "<<EBin<<" "<<EGrid.size()<<" "
	  <<EGrid.front()<<" "<<EGrid.back()<<ELog::endCrit;
  ELog::EM<<"EDATA == "<<EData.shape()[0]<<" "
	  <<EData.shape()[1]<<" "
	  <<EData.shape()[2]<<ELog::endCrit;

  OX<<"#hvn "<<nH<<" "<<nV<<" from "<<nps<<std::endl;
  OX<<"#cvh "<<Cent<<" : "<<H*hSize<<" : "<<V*vSize<<std::endl;

  for(size_t i=0;i<nV;i++)
    {
      for(size_t j=0;j<nH;j++)
	{
	  const double E=EData.get()[i][j][EBin];
	  OX<<E<<" ";
	}
      OX<<std::endl;

    }
  return;
}

} // NAMESPACE Transport


  
