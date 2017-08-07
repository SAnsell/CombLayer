/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMat/SQWmaterial.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <complex>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/multi_array.hpp>

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
#include "mathSupport.h"
#include "Simpson.h"
#include "RefCon.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "neutron.h"
#include "SQWtable.h"
#include "SEtable.h"
#include "ENDFmaterial.h"
#include "neutMaterial.h"
#include "SQWmaterial.h"

extern MTRand RNG;

namespace scatterSystem
{

SQWmaterial::SQWmaterial() :
  neutMaterial(),Extra(0)
 /*!
    Constructor
  */
{}

SQWmaterial::SQWmaterial(const std::string& N,const double M,
			 const double D,const double B,
			 const double S,const double I,const double A) : 
  neutMaterial(N,M,D,B,S,I,A),eFrac(0),Extra(0)
  /*!
    Constructor for values
    \param N :: SQWmaterial name
    \param M :: Mean atomic mass
    \param D :: density [atom/angtrom^3]
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

SQWmaterial::SQWmaterial(const double M,const double D,const double B,
			 const double S,const double I,const double A) : 
  neutMaterial(M,D,B,S,I,A),eFrac(0),Extra(0)
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

SQWmaterial::SQWmaterial(const SQWmaterial& A) : 
  neutMaterial(A),eFrac(A.eFrac),HMat(A.HMat),
  Extra(A.Extra ? A.Extra->clone() : 0)
  /*!
    Copy constructor
    \param A :: Crystal to copy
  */
{}

SQWmaterial&
SQWmaterial::operator=(const SQWmaterial& A)
  /*!
    Assignment operator
    \param A :: SQWmaterial to copy
    \return *this
  */
{
  if (this!=&A)
    {
      neutMaterial::operator=(A);
      eFrac=A.eFrac;
      HMat=A.HMat;
      delete Extra;
      Extra=(A.Extra) ? A.Extra->clone() : 0;
    }
  return *this;
}

SQWmaterial*
SQWmaterial::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new SQWmaterial(*this);
}

SQWmaterial::~SQWmaterial() 
  /*!
    Destructor
   */
{
  delete Extra;
}

void 
SQWmaterial::setExtra(const std::string& N,const double Frac,
		      const double M,const double B,
		      const double S,const double I,
		      const double A) 
  /*!
    Set the extra material. Note that the density is copied from
    the original material
    \param Name :: sub-Material name
    \param M :: Mean atomic mass
    \param B :: b-coherrent [fm]
    \param S :: Scattering cross section [barns]
    \param I :: Incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{
  delete Extra;
  Extra=new neutMaterial(N,density,M,B,S,I,A);
  eFrac=Frac;
  return;
}

void
SQWmaterial::setENDF7(const std::string& FName) 
  /*!
    Set teh crystal file
    \param FName :: Cif file
  */
{
  ELog::RegMethod RegA("SQWmaterial","setENDF7");
  
  if (HMat.ENDF7file(FName))
    ELog::EM<<"Failed to read endf-7 file:"<<FName<<ELog::endErr;
  return;
}


double
SQWmaterial::ScatCross(const double Wave) const
  /*!
    Given wavelength get the scattering cross section
    \param Wave :: Wavelength [Angstrom]
    \return Scattering Attenuation (including density)
  */
{
  const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
  return density*HMat.sigma(E);
}

double
SQWmaterial::calcAtten(const double Wave,const double Length) const
  /*!
    Calculate the attenuation factor coefficient.
    \param Wave :: Wavelength [Angstrom]
    \param Length :: Absorption length
    \return Attenuation (including density)
  */
{
  const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
  return exp(-Length*density*(HMat.sigma(E)+Wave*sabs/1.78));
}

double
SQWmaterial::ScatTotalRatio(const double Wave) const
  /*!
    Given a scatter return the ratio in scattering cross section
    against total.
    \param Wave :: Wavelength [Angstrom]
    \return sigma_scatter/sigma_total
  */
{
  if (density>0)
    {
      const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
      const double SC=HMat.sigma(E);
      const double AB=Wave*sabs/1.798;
      return SC/(SC+AB);
    }
  return 1.0;
}

void
SQWmaterial::scatterNeutron(MonteCarlo::neutron& N) const
  /*!
    Scatter the neutron N appropiately
    \param N :: neutron to scatter 
  */
{
  ELog::RegMethod RegA("SQWmaterial","scatterNeutron");

  if (density>0)
    {
      const double& Wave(N.wavelength);
      const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
      const double SC=HMat.sigma(E);
      const double AB=Wave*sabs/1.798;
      // Now change energy [Randomly: this is poor]
      //      double ENew=E;
      //      ENew *= RNG.rand()*(4.0-1.0/51)+1.0/51;
      N.weight*= SC/(SC+AB);      // NEED THE energy-BIN!!!
    }
  return;
}

double
SQWmaterial::TotalCross(const double Wave) const 
  /*!
    Given Wavelength get the attenuation coefficient.
    \param Wave :: Wavelength [Angstrom]
    \return Attenuation (including density)
  */
{
  // Energy [eV]
  const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
  return density*(Wave*sabs/1.798+HMat.sigma(E));
}

double
SQWmaterial::dSdOdE(const MonteCarlo::neutron& NIn,
		    const MonteCarlo::neutron& NOut) const
  /*!
    Determines the weight, relative to the total cross section
    of a neutron exiting with the channel NOut.
    Uses Ideal gas equation:
    \param Wave :: Wavelength [incomming]
    \return S(Q,w)
  */
{
  ELog::RegMethod RegA("SQWmaterial","dSdOdE");
  
  // First calculate dSDoDE for the given energy difference.
  // InElastic scatter:
  const double Ei=NIn.energy();
  const double Ef=NOut.energy();
  const double mu=NOut.uVec.dotProd(NIn.uVec);

  const double SOut=HMat.dSdOdE(Ei,Ef,mu);
  if (SOut<0.0)
    {
      ELog::EM<<"Negative at "<<SOut<<ELog::endCrit;
      return neutMaterial::dSdOdE(NIn,NOut);
    }
  if (Extra)
    {
      const double Sextra=Extra->dSdOdE(NIn,NOut);
      ELog::EM<<NIn.wavelength<<" == "<<SOut<<" "<<Sextra<<" :: "
	      <<eFrac<<ELog::endCrit;
      return Sextra*eFrac+(1.0-eFrac)*SOut;
    }
  return SOut;
}


} // NAMESPACE scatterSystem

