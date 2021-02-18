/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/particle.cxx
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

#include "Exception.h"
#include "RefCon.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "particleConv.h"
#include "particle.h"

namespace MonteCarlo
{

long int particle::masterID(0);

std::ostream&
operator<<(std::ostream& OX,const particle& A)
  /*!
    Write to a stream
    \param OX :: Output stream
    \param A :: particle to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}

particle::particle(const std::string& pName,
		   const Geometry::Vec3D& Pt,
		   const Geometry::Vec3D& D,
		   const double E) :
  typeID(particleConv::Instance().mcplITYP(pName)),
  ID(++masterID),energy(E),
  wavelength(particleConv::Instance().mcplKEWavelength(typeID,E)),
  Pos(Pt),uVec(D.unit()),weight(1.0),
  travel(0.0),time(0.0),nCollision(0),
  OPtr(0)
  /*!
    Constructor 
    \param W :: Wavelength [Angstrom]
    \param Pt :: Position
    \param D :: Direction vector
  */
{}

particle::particle(const Geometry::Vec3D& Pt,
		   const Geometry::Vec3D& D) :
  typeID(0),ID(++masterID),energy(1.0),
  wavelength(1.0),
  Pos(Pt),uVec(D.unit()),weight(1.0),
  travel(0.0),time(0.0),nCollision(0),
  OPtr(0)
  /*!
    Constructor for empty values
    \param W :: Wavelength [Angstrom]
    \param Pt :: Position
    \param D :: Direction vector
  */
{}

particle::particle(const particle& A) :
	     typeID(A.typeID),ID(A.ID),
	     energy(A.energy),wavelength(A.wavelength),
	     Pos(A.Pos),uVec(A.uVec),
	     weight(A.weight),travel(A.travel),
	     time(A.time),nCollision(A.nCollision),
	     OPtr(A.OPtr)
  /*!
    Copy constructor
    \param A :: particle to copy
  */
{}

particle&
particle::operator=(const particle& A)
  /*!
    Assignment operator
    \param A :: particle to copy
    \return *this
  */
{
  if (this!=&A)
    {
      typeID=A.typeID;
      ID=A.ID;
      energy=A.energy;
      wavelength=A.wavelength;
      Pos=A.Pos;
      uVec=A.uVec;
      weight=A.weight;
      travel=A.travel;
      time=A.time;
      nCollision=A.nCollision;
    }
  return *this;
}

int
particle::equalityFlag(const particle& A) const
  /*!
    Complex function that returns check of the equality 
    of two particles
    \retval 0 :: No matched
    \retval 1 :: wavelength
    \retval 2 :: Pos    
    \retval 4 :: uVec
    \retval 8 :: weight
    \retval 16 :: travel
    \retval 32 :: time
    \retval 64 :: collisions
  */
{
  if (this==&A) return 255;
  int outFlag(0);
  outFlag |= (std::abs(energy-A.energy)<1e-5) ? 1 : 0;
  outFlag |= (Pos==A.Pos) ? 2 : 0;
  outFlag |= (uVec==A.uVec) ? 4 : 0;
  outFlag |= (std::abs(weight-A.weight))<1e-5 ? 8 : 0;
  outFlag |= (std::abs(travel-A.travel))<1e-5 ? 16 : 0;
  outFlag |= (std::abs(time-A.time))<1e-5 ? 32 : 0;
  outFlag |= (nCollision==A.nCollision) ? 64 : 0;
  outFlag |= (typeID==A.typeID) ? 128 : 0;
  return outFlag;
}

void
particle::moveForward(const double Dist)
  /*!
    Move forward by the required distance
    -- This does not change the intercept points
    \param Dist :: Distance to move
  */
{
  Pos+=uVec*Dist;
  travel+=Dist;
  time+=Dist/velocity();
  return;
}

double
particle::velocity() const
  /*!
    Convert the energy [MeV] to m/s in the rest frame
    \return velocity [m/s]
  */
{
  // special case for photon
  if (typeID==22)
    return RefCon::c;

  // MeV / c^2
  const double mass=particleConv::Instance().mcplMass(typeID);
  if (mass<1e-5) return RefCon::c;
  
  const double gamma=mass/(energy+mass);
  return RefCon::c*sqrt(1.0-gamma*gamma);
}

void
particle::setEnergy(const double E)
  /*!
    Sets the energy
    \param E :: Energy [eV] 
  */
{
  wavelength=particleConv::Instance().mcplKEWavelength(typeID,E);
  return;
}

double
particle::Q(const particle& A) const
  /*!
    Calculate the the momentum difference between
    two particles 
    Equation is \f$ \vec{Q}=\vec{k_f}-\vec{k_i} \f$
    but \f$ |Q^2|=k_i^2+k_f^2-2k_f*k_k \cos(\theta) \f$
    \param A :: Particle after scattering
    \return hQ
   */
{
  const particleConv& PC=particleConv::Instance();

  const double ki=PC.mcplMomentumFromKE(typeID,energy);
  const double kf=PC.mcplMomentumFromKE(A.typeID,A.energy);
  const double cosT=uVec.dotProd(A.uVec);
  const double Q2=ki*ki+kf*kf-2.0*cosT*kf*ki;
  return 2.0*M_PI*sqrt(Q2);
}

double
particle::eLoss(const particle& NX) const
  /*!
    Calculate the the energy lost going from higher
    energy e_i to lower energy e_f. 
    Equation is \f$ \hbar\omega_f - \hbar\omega_i \f$
    \param NX :: Particle after scattering
    \return energy lost to final state [eV] 
  */
{
  return energy-NX.energy;
}

int
particle::refract(const double Ni,const double No,
		  const Geometry::Vec3D& n)
  /*!
    Refraction off a surface.
    - Modifies uVec but not Pos.
    - Updates the ID to a new ID
    n is the normal and x is the vector along the surface
    \param Ni :: Incident refractive index
    \param No :: Exit refractive index
    \param n :: Normal vector [n in diagram]
    \retval 0 :: standard refraction
    \retval 1 :: reflection    
   */
{
  ELog::RegMethod RegA("particle","refract");
  const double cosA= n.dotProd(-uVec);
  //  const double alpha=(180/M_PI)*acos(cosA);
  ID= ++masterID;
  
  const double R=Ni/No;
  const double Rfact=1.0-(R*R)*(1.0-cosA*cosA);
  // Process reflection

  if (Rfact<0)
    {
      uVec += n*(2.0*cosA);          
      return 1;
    }
  // Refraction
  if (cosA<0)
    uVec=(uVec*R)+n*(R*cosA+sqrt(Rfact));
  else
    uVec=(uVec*R)+n*(R*cosA-sqrt(Rfact));
  uVec.makeUnit();
  return 0;
}


void
particle::addCollision()
  /*!
    Updates the collision and the ID so
    that the particle appears to be new.
  */
{
  ID= ++masterID;
  nCollision++;
  return;
}

void
particle::write(std::ostream& OX) const
  /*!
    Write a given particle to a stream
    \param OX :: Output stream
   */
{
  OX<<particleConv::Instance().mcplToFLUKA(typeID)<<":"
    <<Pos<<" u: "<<uVec<<" ("<<wavelength<<":"<<1000*energy
    <<") W="<<weight<<" T="<<travel<<"("<<time<<")"<<" nCol="
    <<nCollision<<" (ID="<<ID<<")";
  return;
}

}  // NAMESPACE MonteCarlo
