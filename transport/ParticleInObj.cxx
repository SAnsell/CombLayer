/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/ParticleInObj.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdexcept> 

#include "MersenneTwister.h"
#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Track.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Rules.h"
#include "particle.h"
#include "neutron.h"
#include "photon.h"
#include "HeadRule.h"
#include "Object.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "neutMaterial.h"
#include "DBNeutMaterial.h"
#include "ParticleInObj.h"

extern MTRand RNG;

namespace Transport
{

template<typename PTYPE>
std::ostream&
operator<<(std::ostream& OX,const ParticleInObj<PTYPE>& A)
/*!
  Standard Output stream
  \param OX :: Output stream
  \param A :: Component to write
  \return OX
 */

{
  A.write(OX);
  return OX;
}

template<typename PTYPE>
ParticleInObj<PTYPE>::ParticleInObj(const MonteCarlo::Object* OP) : 
  ObjPtr(OP)
  /*!
   Constructor
 */
{}

template<typename PTYPE>
ParticleInObj<PTYPE>::~ParticleInObj()
/*!
  Destructor
*/
{}


template<typename PTYPE>
int
ParticleInObj<PTYPE>::hasIntercept(const MonteCarlo::particle& N) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects)

    \param N :: Initial neutron
    \return True(1)  / Fail(0)
  */
{
  return ObjPtr->hasIntercept(N.Pos,N.uVec);
}

template<typename PTYPE>  
void
ParticleInObj<PTYPE>::attenuate(const double D,
				MonteCarlo::particle& N) const
  /*!
    Attenuate the Neutron in the material over the 
    distance					
    \param D :: Distance to travel
    \param N :: Neutron/Photon point
  */
{
  const MonteCarlo::Material* matPtr=ObjPtr->getMatPtr();
  N.weight *= matPtr->calcAtten(N,D);
  return;
}  

template<typename PTYPE>
double 
ParticleInObj<PTYPE>::getRefractive(const MonteCarlo::particle& N) const
  /*!
    Get the real refractive index
    \param N :: Neutron
    \return  n : the refractive index 
  */
{
  if constexpr (std::is_same<PTYPE,MonteCarlo::neutron>::value)
    {
      const scatterSystem::neutMaterial* matPtr=
	dynamic_cast<const scatterSystem::neutMaterial*>(ObjPtr->getMatPtr());
      if (matPtr)
	return matPtr->calcRefIndex(N.wavelength);
    }
  return 1.0;
}


template<typename PTYPE>
void
ParticleInObj<PTYPE>::scatterParticle(MonteCarlo::particle& N) const
  /*!
    Scatter the neutron into a new direction + energy
    Random scatter.
    \param N :: Neutron to scatter
  */
{
  if constexpr (std::is_same<PTYPE,MonteCarlo::neutron>::value)
    {
      const scatterSystem::neutMaterial* matPtr=
	dynamic_cast<const scatterSystem::neutMaterial*>(ObjPtr->getMatPtr());
      if (matPtr)
	matPtr->scatterNeutron(N);
    }
  return;
}

template<typename PTYPE>
double
ParticleInObj<PTYPE>::scatTotalRatio(const MonteCarlo::particle& n0,
				     const MonteCarlo::particle& n1) const
  /*!
    Get the scattering / Total ratio
    \param n0 :: Input particle
    \param n1 :: Ouput particle
    \param :: Output neutron (After scatter)
    \return new neutron weight
  */
{
  if constexpr (std::is_same<PTYPE,MonteCarlo::neutron>::value)
    {
      const scatterSystem::neutMaterial* matPtr=
	dynamic_cast<const scatterSystem::neutMaterial*>(ObjPtr->getMatPtr());
      if (matPtr)
	matPtr->scatTotalRatio(n0);
    }
  else
    {
      ELog::EM<<"Photon part of scatAbsRatio not written"<<ELog::endCrit;
    }
  return 1.0;
}

template<typename PTYPE>
double
ParticleInObj<PTYPE>::totalXSection(const MonteCarlo::particle& n0) const
  /*!
    Get the total scattering cross section
    \param n0 :: particle to obtain wavelength
    \return sigma_total * density
  */
{
  if constexpr (std::is_same<PTYPE,MonteCarlo::neutron>::value)
    {
      const scatterSystem::neutMaterial* matPtr=
	dynamic_cast<const scatterSystem::neutMaterial*>(ObjPtr->getMatPtr());
      if (matPtr)
	return matPtr->totalXSection(n0);
    }
  else
    {
      ELog::EM<<"Photon part of totalXSection not written"<<ELog::endCrit;
    }
  return 0.0;
}

template<typename PTYPE>
int
ParticleInObj<PTYPE>::trackOutCell
(const MonteCarlo::particle& N,double& aDist,
 const Geometry::Surface*& surfPtr) const
 /*!
    Tracks a particle to the object boundary and determines the 
    minimum distance that the object has to travel from the object
    \param N :: particle to obtain track from
    \param aDist :: primary distance
    \param surfPtr ;: surface  on exit
    \return true/false if neutron intersects object    
  */
{
  return ObjPtr->trackOutCell(N,aDist,surfPtr);
}

template<typename PTYPE>
int
ParticleInObj<PTYPE>::trackIntoCell
(const MonteCarlo::particle& N,double& aDist,
 const Geometry::Surface*& surfPtr) const
  /*!
    This tracks a neutron to an object and determines the 
    minimum distance that the object has to travel to the object
    \param N :: neutron to obtain track from
    \param aDist :: primary distance
    \param surfPtr ;: surface  on exit
    \return true/false if particle intersects/misses the object    
  */
{
  return ObjPtr->trackIntoCell(N,aDist,surfPtr);
}

template<>
int
ParticleInObj<MonteCarlo::neutron>::trackWeight
(MonteCarlo::neutron& N,double& R,const Geometry::Surface*& surfPtr) const
  /*!
    This tracks a particle through object and determines the 
    the track modification to a scattering point.
    \param N :: neutron to move forward:change weight:new direction
    \param R :: Random number exponent step
    \param surfPtr :: surface that track ended on
    \return surface number that it exited at /  0
    \todo TRACK TO START SURFACE 
  */
{
  ELog::RegMethod RegA("ParticleInObj<neutron>","trackWeight");

  double aDist(0.0);
      
  const int SN=ObjPtr->trackOutCell(N,aDist,surfPtr);
  //  ELog::EM<<"Neutron Track"<<N.weight<<ELog::endDiag;
  const scatterSystem::neutMaterial* matPtr=
    dynamic_cast<const scatterSystem::neutMaterial*>(ObjPtr->getMatPtr());
  if (!matPtr->isVoid())
    {
      // Material to attenuate beam:
      const double sXsec=matPtr->scatXSection(N);
      const double aXsec=matPtr->totalXSection(N)-sXsec;
      
      const double DV= -log(R)/sXsec;
      // Neutron did not reach other size
      if (DV<aDist-Geometry::shiftTol)
	{
	  N.weight*=exp(-DV*aXsec);
	  N.moveForward(DV);
	  surfPtr=0;
	  return 0;
	}
      R=exp(-sXsec*(Geometry::shiftTol+DV-aDist));
      N.weight*=exp(-aDist*aXsec);
    }

  N.moveForward(aDist);
  // Now step over 
  Geometry::Vec3D XX(N.Pos);
  N.Pos+=surfPtr->surfaceNormal(N.Pos)*(sign(SN)*Geometry::shiftTol);
  return SN;
}

template<>
int
ParticleInObj<MonteCarlo::neutron>::trackAttn(MonteCarlo::neutron& N,
					      const Geometry::Surface*& SPtr) const
  /*!
    This tracks a neutron through object and determines the 
    the track modification to an exit
    \param N :: neutron to move forward:change weight:new direction
    \param SPtr :: Surface Pointer
    \return surface number that it exited [Signed].
  */
{
  ELog::RegMethod RegA("ParticleInObj","trackAttn");
  double aDist(0);
      
  // Signed number
  const int SN=ObjPtr->trackOutCell(N,aDist,SPtr);
  const scatterSystem::neutMaterial* matPtr=
    dynamic_cast<const scatterSystem::neutMaterial*>(ObjPtr->getMatPtr());
  
  // Material to attenuate beam:
  const double tXsec=matPtr->totalXSection(N);
  N.weight*=exp(-aDist*tXsec);

  // Micro extra to avoid surface boundary
  N.moveForward(aDist);
  N.Pos-=SPtr->surfaceNormal(N.Pos)*(sign(SN)*Geometry::shiftTol);

  return SN;
}


			      
template<typename PTYPE>
void
ParticleInObj<PTYPE>::write(std::ostream& OX) const
  /*!
    Writes out to a stream
    \param OX :: output stream
  */
{
  ObjPtr->write(OX);
  return;
}

template class ParticleInObj<MonteCarlo::neutron>;

} // Namespace Transport
