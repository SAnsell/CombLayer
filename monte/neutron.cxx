/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/neutron.cxx
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
#include "neutron.h"

namespace MonteCarlo
{

int neutron::masterID(0);

std::ostream&
operator<<(std::ostream& OX,const neutron& A)
  /*!
    Write to a stream
    \param OX :: Output stream
    \param A :: neutron to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}

neutron::neutron(const double W,const Geometry::Vec3D& Pt,
		 const Geometry::Vec3D& D) :
  ID(++masterID),wavelength(W),
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

neutron::neutron(const neutron& A) : 
  ID(A.ID),wavelength(A.wavelength),
  Pos(A.Pos),uVec(A.uVec),
  weight(A.weight),travel(A.travel),
  time(A.time),nCollision(A.nCollision),
  OPtr(A.OPtr)
  /*!
    Copy constructor
    \param A :: neutron to copy
  */
{}

neutron&
neutron::operator=(const neutron& A)
  /*!
    Assignment operator
    \param A :: neutron to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ID=A.ID;
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
neutron::equalityFlag(const neutron& A) const
  /*!
    Complex function that returns check of the equality 
    of two neutrons
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
  if (this==&A) return 127;
  int outFlag(0);
  outFlag |= (fabs(wavelength-A.wavelength)<1e-5) ? 1 : 0;
  outFlag |= (Pos==A.Pos) ? 2 : 0;
  outFlag |= (uVec==A.uVec) ? 4 : 0;
  outFlag |= (fabs(weight-A.weight))<1e-5 ? 8 : 0;
  outFlag |= (fabs(travel-A.travel))<1e-5 ? 16 : 0;
  outFlag |= (fabs(time-A.time))<1e-5 ? 32 : 0;
  outFlag |= (nCollision==nCollision) ? 64 : 0;
  return outFlag;
}

void
neutron::moveForward(const double Dist)
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
neutron::velocity() const
  /*!
    Convert the wavelneght [Angstrom] to m/s
    - No reletavisic correction
    \return velocity [m/s]
  */
{
  return (1e10*RefCon::h_mn)/wavelength;
}

void
neutron::setEnergy(const double E)
  /*!
    Sets the energy
    \param E :: Energy [eV] 
  */
{
  wavelength=sqrt(0.5*RefCon::h2_mneV*1e20/fabs(E));
  return;
}

double
neutron::energy() const
  /*!
    Return the energy based on the wavelength
    \return E [eV]
   */
{
  return (0.5*RefCon::h2_mneV*1e20)/(wavelength*wavelength);  
}

double
neutron::Q(const neutron& NX) const
  /*!
    Calculate the the momentum difference between
    two neutrons 
    Equation is \f$ \vec{Q}=\vec{k_f}-\vec{k_i} \f$
    but \f$ |Q^2|=k_i^2+k_f^2-2k_f*k_k \cos(\theta) \f$
    \param NX :: Neutron after scattering
    \return hQ
   */
{
  const double cosT=uVec.dotProd(NX.uVec);
  const double ki=1.0/wavelength;
  const double kf=1.0/NX.wavelength;
  const double Q2=ki*ki+kf*kf-2.0*cosT*kf*ki;
  return 2.0*M_PI*sqrt(Q2);
}

double
neutron::eLoss(const neutron& NX) const
  /*!
    Calculate the the energy lost going from higher
    energy e_i to lower energy e_f. 
    Equation is \f$ \hbar\omega_f - \hbar\omega_i \f$
    \param NX :: Neutron after scattering
    \return energy lost to final state [eV] 
  */
{
  const double WD=1.0/(wavelength*wavelength)-
    1.0/(NX.wavelength*NX.wavelength);

  return RefCon::h2_mneV*1e20*WD;  
}

int
neutron::refract(const double Ni,const double No,
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
  ELog::RegMethod RegA("neutron","refract");
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
neutron::addCollision()
  /*!
    Updates the collision and the ID so
    that the neutron appears to be new.
  */
{
  ID= ++masterID;
  nCollision++;
  return;
}

void
neutron::write(std::ostream& OX) const
  /*!
    Write a given neutron to a stream
    \param OX :: Output stream
   */
{
  OX<<Pos<<" u: "<<uVec<<" ("<<wavelength<<":"<<1000*energy()
    <<") W="<<weight<<" T="<<travel<<"("<<time<<")"<<" nCol="
    <<nCollision<<" (ID="<<ID<<")";
  return;
}

}  // NAMESPACE MonteCarlo
