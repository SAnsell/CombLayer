/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   scatMat/neutMaterial.cxx
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
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

#include "MersenneTwister.h"
#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "RefCon.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "neutron.h"
#include "neutMaterial.h"

extern MTRand RNG;

namespace scatterSystem
{

neutMaterial::neutMaterial() : 
  Amass(1.0),density(0),realTemp(0.0),scoh(0.0),
  sinc(0.0),sabs(0.0),bTotal(0.0)
  /*!
    Constructor
  */
{}

neutMaterial::neutMaterial(const std::string& N,const double M,
		   const double D,const double B,
		   const double S,const double I,const double A) : 
  Name(N),Amass(M),density(D),realTemp(300.0),bcoh(B),
  scoh(S),sinc(I),sabs(A),bTotal(sqrt(S+I)/(4*M_PI))
  /*!
    Constructor for values
    \param N :: neutMaterial name
    \param M :: Mean atomic mass
    \param D :: density [atom/angtrom^3]
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

neutMaterial::neutMaterial(const double M,const double D,const double B,
		   const double S,const double I,const double A) : 
  Amass(M),density(D),realTemp(300.0),bcoh(B),scoh(S),
  sinc(I),sabs(A),bTotal(sqrt(S+I)/(4*M_PI))
  /*!
    Constructor for values
    \param M :: Mean atomic mass
    \param D :: density [atom/angtrom^3]
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

neutMaterial::neutMaterial(const neutMaterial& A) : 
  Name(A.Name),Amass(A.Amass),density(A.density),
  realTemp(A.realTemp),bcoh(A.bcoh),scoh(A.scoh),
  sinc(A.sinc),sabs(A.sabs),bTotal(A.bTotal)
  /*!
    Copy constructor
    \param A :: neutMaterial to copy
  */
{}

neutMaterial&
neutMaterial::operator=(const neutMaterial& A) 
  /*!
    Assignment operator 
    \param A :: neutMaterial to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Name=A.Name;
      Amass=A.Amass;
      density=A.density;
      realTemp=A.realTemp;
      bcoh=A.bcoh;
      scoh=A.scoh;
      sinc=A.sinc;
      sabs=A.sabs;
      bTotal=A.bTotal;
    }
  return *this;
}

neutMaterial*
neutMaterial::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new neutMaterial(*this);
}

neutMaterial::~neutMaterial() 
  /*!
    Destructor
   */
{}

void 
neutMaterial::setDensity(const double D) 
  /*!
    Sets the density
    \param D :: Density [Atom/Angstrom^3]
  */
{
  density=D;
  return;
}


void
neutMaterial::setScat(const double S,const double I,const double A)
  /*!
    Set the scattering factors
    \param S :: Coherrent-Scattering cross section
    \param I :: Inc-Scattering cross section
    \param A :: Absorption cross section
  */
{
  scoh=S;
  sinc=I;
  sabs=A;
  bTotal=sqrt(S+I)/(4*M_PI);
  return;
}
  
double
neutMaterial::TotalCross(const double Wave) const
  /*!
    Given Wavelength get the attenuation coefficient.
    \param Wave :: Wavelength [Angstrom]
    \return Attenuation (including density)
  */
{
  return density*(scoh+sinc+Wave*sabs/1.798);
}

double
neutMaterial::ScatCross(const double) const
  /*!
    Given wavelength get the scattering cross section
    \param :: Wavelength [Angstrom]
    \return Attenuation (including density)
  */
{
  return density*(scoh+sinc);
}

double
neutMaterial::calcAtten(const double Wave,const double Length) const
  /*!
    Calculate the attenuation factor coefficient.
    \param Wave :: Wavelength [Angstrom]
    \param Length :: Absorption length
    \return Attenuation (including density)
  */
{
  return exp(-Length*density*(scoh+sinc+Wave*sabs/1.798));
}

void
neutMaterial::scatterNeutron(MonteCarlo::neutron& N) const
  /*!
    Calculate the new angle and energy of the neutron
    that is scattered.
    Doesn't change energy:
    \param N :: neutron to scatter
  */
{
  ELog::RegMethod RegA("neutMaterial","scatterNeutron");

  const double theta=2*M_PI*RNG.rand();
  const double phi=M_PI*RNG.rand();
  N.uVec[0]=cos(theta)*sin(phi);
  N.uVec[1]=sin(theta)*sin(phi);
  N.uVec[2]=cos(phi);

  N.addCollision();  // This also updates the ID !!!
  return;
}

double
neutMaterial::ScatTotalRatio(const double Wave) const
  /*!
    Given a scatter return the ratio in scattering cross section
    against total.
    \param Wave :: Wavelength [Angstrom]
    \return sigma_scatter/sigma_total
  */
{
  return (density>0) ? 
    (scoh+sinc)/(scoh+sinc+Wave*sabs/1.798) : 1.0;
}

double
neutMaterial::ElasticTotalRatio(const double) const
  /*!
    Calculate the ratio of elastic/sigma_T
    against total.
    \param :: Wavelength [Angstrom]
    \return sigma_elastic/sigma_scatter
  */
{
  return (density>0) ? scoh/(scoh+sinc) : 1.0;
}


double
neutMaterial::calcRefIndex(const double Wave) const
  /*!
    Calc refractive index 
    \param Wave :: Wavelength
    \return n - the real refractive index
  */
{
//  ELog::RegMethod RegA("neutMaterial","calcRefIndex");
//  ELog::EM<<"Ref ["<<Name<<"]=="
//	  <<1.0-sqrt(1.0-Wave*Wave*(1e-5*density*bcoh/M_PI))<<ELog::endErr;
  return sqrt(1.0-Wave*Wave*(1e-5*density*bcoh/M_PI));
}

double
neutMaterial::dSdOdE(const MonteCarlo::neutron& NIn,
		 const MonteCarlo::neutron& NOut) const
  /*!
    Determines the weight, relative to the total cross section
    of a neutron exiting with the channel NOut.
    Uses Ideal gas equation:
    \param Wave :: Wavelength [incomming]
    \return New wavelength
  */
{
  ELog::RegMethod RegA("neutMaterial","dSdOdE");
  
  // First calculate dSDoDE for the given energy difference.
  // InElastic scatter:
  const double Ei=NIn.energy();
  const double Ef=NOut.energy();

  const double mu=NOut.uVec.dotProd(NIn.uVec);
  
  // Maybe a factor 2 missing?????

  const double alpha=(Ei+Ef-2.0*mu*sqrt(Ei*Ef))/
    (4*Amass*RefCon::k_bev*realTemp); 
  const double beta=(Ei-Ef)/(RefCon::k_bev*realTemp);

  // Non-Elastic scatter [neutron loss]: 
  
  const double SQW=exp(-(beta-alpha)*(beta-alpha)/(4.0*alpha)) / 
    (2*RefCon::k_bev*realTemp*sqrt(M_PI*alpha));
  return SQW*bTotal*bTotal*NIn.wavelength/NOut.wavelength;
}

void
neutMaterial::write(std::ostream& OX) const 
  /*!
    Write out the material information for MCNPX
    \param OX :: Output stream
  */
{
  OX<<"c "<<Name<<std::endl;
  OX<<"m"<<mcnpxNum<<" "<<mcnpxNum*1000<<" "<<density<<std::endl;
  return;
}

} // NAMESPACE MonteCarlo

