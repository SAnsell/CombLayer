/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/BandDetector.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

#include "MersenneTwister.h"
#include "RefCon.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "Vec3D.h"
#include "particle.h"
#include "Detector.h"
#include "BandDetector.h"

extern MTRand RNG;

namespace Transport
{


BandDetector::BandDetector() : 
  Detector(0),nH(0),nV(0),nE(0),Cent(Geometry::Vec3D(0,0,0)),
  H(Geometry::Vec3D(1,0,0)),V(Geometry::Vec3D(0,0,1)),
  hSize(1.0),vSize(1.0),
  PlnNorm(Geometry::Vec3D(0,1,0)),PlnDist(0.0)
  /*!
    Default constructor
    Note that Detector index is unused
  */
{
}

BandDetector::BandDetector(const int Hpts,const int Vpts,const int Epts,
			     const Geometry::Vec3D& CV,
			     const Geometry::Vec3D& Hvec,
			     const Geometry::Vec3D& Vvec,
			     const double ES,const double EE) : 
  Detector(0),nH(Hpts),nV(Vpts),nE((Epts>0) ? Epts : 1),
  Cent(CV),H(Hvec.unit()),V(Vvec.unit()),
  hSize(Hvec.abs()),vSize(Vvec.abs()),
  PlnNorm((V*H).unit()),PlnDist(Cent.dotProd(PlnNorm)),
  EData(boost::extents[nV][nH][nE])
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
  for(int i=0;i<nV;i++)
    for(int j=0;j<nH;j++)
      for(int k=0;k<nE;k++)
	EData[i][j][k]=0.0;
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
      if (EData.shape()!=A.EData.shape())
	{
	  EData.resize(boost::extents[nH][nV][nE]);
	  ELog::EM<<"Data size == "<<A.EData.shape()[0]<<" "
		  <<A.EData.shape()[1]<<" "
		  <<A.EData.shape()[2]<<ELog::endCrit;
	}
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
  for(int i=0;i<nV;i++)
    for(int j=0;j<nH;j++)
      for(int k=0;k<nE;k++)
	EData[i][j][k]=0.0;
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
  
  if (fabs(engB-engA)<1e-7)
    {
      ELog::EM<<"Setting energy gap--too small"<<ELog::endWarn;
      return;
    }
  const double step((engB-engA)/nE);
  EGrid.clear();
  for(int i=0;i<=nE;i++)
    EGrid.push_back(engA+i*step);
  return;
}

void
BandDetector::setDataSize(const int Hpts,const int Vpts,
		      const int Epts)
  /*!
    Set the data Size
    \param Hpts :: Horizontal size
    \param Vpts :: Vertical size 
    \param Epts :: Energy size
   */
{
  if (Hpts>0 && Vpts>0 && (Hpts!=nH || Vpts!=nV) )
    {
      nH=Hpts;
      nV=Vpts;
      nE=(Epts>0) ? Epts : 1;
      EData=boost::multi_array<double,3>(boost::extents[nV][nH][nE]);
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
		   int& NH,int& NV) const
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
  if (fabs(DdotN)<Geometry::parallelTol)     // Plane and line parallel
    return 0;
  const double u=(PlnDist-OdotN)/DdotN;
  Geometry::Vec3D Pnt=N.Pos+N.uVec*u;
  // Now determine if Pnt is within detector:
  Pnt-=(Cent-H*0.5*hSize-V*0.5*vSize);
  NH=static_cast<int>(nH*Pnt.dotProd(H)/hSize);
  NV=static_cast<int>(nV*Pnt.dotProd(V)/vSize);
  ELog::EM<<"Point == "<<Pnt<<" ("<<NH<<","<<NV<<")"<<ELog::endDebug;
  ELog::EM<<"Det == "<<Cent<<" ("<<H<<","<<V<<")::"<<PlnNorm<<ELog::endDebug;
  ELog::EM<<"U == "<<u<<" "<<PlnDist<<" "<<hSize<<" "<<vSize<<ELog::endDebug;
  ELog::EM<<ELog::endDebug;
  return (NH<0 || NV<0 || NH>=nH || NV>=nV) ? 0 : 1;
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
  const int hpt=static_cast<int>(static_cast<double>(nH)*
				 Pnt.dotProd(H)/hSize);
  const int vpt=static_cast<int>(nV*Pnt.dotProd(V)/vSize);
  if (hpt<0 || vpt<0 || hpt>=nH || vpt>=nV) return;
  // Scale for (i) distance to scattering point:
  //           (ii) solid angle
  // Distance is u + travel
  const long int ePoint=calcWavePoint(N.wavelength);
  if (ePoint>=0 || ePoint<static_cast<long int>(EGrid.size())-1)
    {
      EData[vpt][hpt][ePoint]+=
	N.weight/((N.travel+u)*(N.travel+u)*fabs(DdotN));
      nps++;
    }
  return;
}

long int
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
  const long int res=indexPos(EGrid,E);
  if (res<0 || res>=static_cast<long int>(EData.shape()[0]))
    {
      ELog::EM<<"Bins failed on : "<<W<<" "<<
	E<<" == "<<EGrid.front()<<" "<<EGrid.back()<<ELog::endCrit;
    }
  return res;
}

long int
BandDetector::calcEnergyPoint(const double E) const
  /*!
    Given the wavelength calculate the 
    point in the EGrid set
    \param E :: Energy [eV]
    \return EGrid cell
  */
{
  if (EGrid.empty()) return 0;
  return indexPos(EGrid,E);
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
  return Cent+H*hSize*(0.5-RNG.rand())+
    V*vSize*(0.5-RNG.rand());
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
  boost::format FMR("%1$12.8e%|20t|");
  OX<<"#hvn "<<nH<<" "<<nV<<" from "<<nps<<std::endl;
  OX<<"#cvh "<<Cent<<" : "<<H*hSize<<" : "<<V*vSize<<std::endl;

  for(int i=0;i<nV;i++)
    {
      for(int j=0;j<nH;j++)
	OX<<FMR % EData[i][j][EBin];
      OX<<std::endl;

    }
  return;
}

} // NAMESPACE Transport


  
