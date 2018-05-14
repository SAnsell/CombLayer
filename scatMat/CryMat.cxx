/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMat/CryMat.cxx
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
#include "neutMaterial.h"
#include "SymUnit.h"
#include "AtomPos.h"
#include "loopItem.h"
#include "CifLoop.h"
#include "CifStore.h"
#include "CryMat.h"

extern MTRand RNG;

namespace scatterSystem
{

 /*!
  \struct etaFunc 
  \version 1.0
  \author S. Ansell
  \date June 2013
  \brief Functional operator to calculate attenuation function
*/
  
struct etaFunc
{
  /*!
    Eta function [Eqn 9: Nucl Inst Method. 213 (1983) 495]
    Changed to exp(+eta) from J. Appl Cryst. (1993). 26, 438-447
  */
  double operator()(const double& eta)
    {
      return (std::abs(eta)>1e-7) ? eta/(exp(eta)-1.0) : 1.0;
    }
};

CryMat::CryMat() : neutMaterial()
 /*!
    Constructor
  */
{}

CryMat::CryMat(const std::string& N,const double M,const double D,
	       const double B,const double S,const double I,
	       const double A) : 
  neutMaterial(N,M,D,B,S,I,A),debyeTemp(300.0),
  C1(0),C2(4.27*exp(M/61.0)),Rsum(0),B0plusBT(Bvalue(debyeTemp/realTemp))
  /*!
    Constructor for values
    \param N :: CryMat name
    \param M :: Mass [Mean atomic]
    \param D :: density [atom/angtrom^3]
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

  CryMat::CryMat(const double M,const double D,const double B,
		 const double S,const double I,const double A) : 
  neutMaterial(M,D,B,S,I,A),debyeTemp(300.0),
  C1(0),C2(4.27*exp(M/61.0)),Rsum(0),B0plusBT(Bvalue(debyeTemp/realTemp))
  /*!
    Constructor for values
    \param M :: Mass [Mean atomic]
    \param D :: density [atom/angtrom^3]
    \param B :: b-coherrent [fm]
    \param S :: scattering cross section [barns]
    \param I :: incoherrent scattering cross section [barns]
    \param A :: absorption cross section [barns]
  */
{}

CryMat::CryMat(const CryMat& A) : 
  neutMaterial(A),
  debyeTemp(A.debyeTemp),C1(A.C1),C2(A.C2),
  Rsum(A.Rsum),B0plusBT(A.B0plusBT),XStruct(A.XStruct)
  /*!
    Copy constructor
    \param A :: Crystal to copy
  */
{}

CryMat&
CryMat::operator=(const CryMat& A)
  /*!
    Assignment operator
    \param A :: CryMat to copy
    \return *this
  */
{
  if (this!=&A)
    {
      neutMaterial::operator=(A);
      debyeTemp=A.debyeTemp;
      C1=A.C1;
      C2=A.C2;
      Rsum=A.Rsum;
      B0plusBT=A.B0plusBT;
      XStruct=A.XStruct;
    }
  return *this;
}

CryMat*
CryMat::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new CryMat(*this);
}

CryMat::~CryMat() 
  /*!
    Destructor
   */
{}
  
void
CryMat::setTemperatures(const double dTemp,const double Temp)
  /*!
    Set the Debye temperature 
    \param dTemp :: Debye Temperature [Kelvin]
    \param Temp :: Temperature [Kelvin]
  */
{
  ELog::RegMethod("CryMat","setTemperatures");

  debyeTemp=dTemp;
  realTemp=Temp;
  Rsum=Rvalue(debyeTemp/realTemp);
  B0plusBT=Bvalue(debyeTemp/realTemp);
  
  const double x(debyeTemp/realTemp);
  ELog::EM<<"Rvalue == "<<Rsum<<" "<<x
	  <<" R(6) == "<<3.3*pow(x,-3.5)
	  <<ELog::endDebug;

  return;
}

void
CryMat::setCif(const std::string& FName) 
  /*!
    Set teh crystal file
    \param FName :: Cif file
  */
{
  ELog::RegMethod RegA("CryMat","setCif");
  
  if (XStruct.readFile(FName))
    ELog::EM<<"Failed to read cif file:"<<FName<<ELog::endErr;
  return;
}

void
CryMat::setMass(const double A) 
  /*!
    Set the mean atomic mass
    \param A :: Mean mass
  */
{
  Amass=A;
  C2=4.27*exp(Amass/61.0);
  B0plusBT=Bvalue(debyeTemp/realTemp);
  return;
}

double
CryMat::Rvalue(const double x) const
  /*!
    Calculate the R value for a given x.
    Formula \f$ R_n= B_n x^{n-1}/[n!(n+5/2)] \f$
    \param x :: ratio Theta_Debye/T;
    \return R value
  */
{
  ELog::RegMethod RegA("CryMat","Rvalue");
  const double BN[]={1.0,1.0/6.0,-1.0/30.0,
		     1.0/42.0, -1.0/30.0, 5.0/66.0,
                     -691.0/2730.0,7.0/6.0,
                      -3617.0/510,43867.0/798,
		     -174611.0/330.0,854513/138.0};
  const int sizeBN(sizeof(BN)/sizeof(double));

  if (x>6.0)
    return 3.3*pow(x,-3.5);
  
  // special case for b1.
  double sum= -0.5/3.5;
  for(int i=0;i<sizeBN;i++)
    {
      double L=factorialDB(i*2);
      L*=(2.0*i+2.5);
      sum+=(BN[i]*pow(x,1.0-2*i))/L;
    }
  return sum;
}

double
CryMat::Bvalue(const double x) const
  /*!
    Calculate the B value for a given x.
    Formula \f$ 4*B_0 \phi(x)/x \f$
    were \f[ \phi(x)=1/x \int^x_0 \frac{\eta}{e^{-\eta}-1} d\eta \f]
    \param x :: ratio Theta_Debye/T;
    \return B value [Angstrom^2]
  */
{
  ELog::RegMethod RegA("CryMat","Bvalue");
  // B0 is 3h^2/2k_b (Debye * AtomicMass)
  // B0 == 1e20 * 1000 * N_A * 3 * h^2 */ 2k_b
  const double B0= 2872.5688107029/(Amass*debyeTemp);
  const double BT= 4.0*B0*Simpson::integrate<etaFunc>(40,0.0,x)/(x*x);
  return B0+BT;
}

double
CryMat::sigmaSph(const double E) const
  /*!
    Calucate sigma_sph given the energy
    \param E :: Energy [eV]
  */
{
  const double tR(sqrt(RefCon::k_bmev*debyeTemp/(E*1000.0)));
  return 3*Rsum*tR*(scoh+sinc)/Amass;
}

double
CryMat::sigmaMph(const double E) const
  /*!
    Calucate sigma_mph (multiphonon) given the energy
    \param E :: Energy [eV]
    \return sigma_mph [barns]
  */
{
  const double sigma_free=(Amass/(Amass+1))*(Amass/(Amass+1))*
    (scoh+sinc);
  return (1.0-exp(-B0plusBT*C2*E))*sigma_free;
}

double
CryMat::TotalCross(const double Wave) const
  /*!
    Given Wavelength get the attenuation coefficient.
    \param Wave :: Wavelength [Angstrom]
    \return Attenuation (including density)
  */
{
  // Energy [eV]
  const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
  return density*(Wave*sabs/1.798+sigmaSph(E)+
		  sigmaMph(E));
}

double
CryMat::ScatCross(const double Wave) const
  /*!
    Given wavelength get the scattering cross section
    \param Wave :: Wavelength [Angstrom]
    \return Scattering Attenuation (including density)
  */
{
  const double E=(0.5*RefCon::h2_mneV*1e20)/(Wave*Wave);  
  return density*(sigmaSph(E)+sigmaMph(E));
}

double
CryMat::calcAtten(const double Wave,const double Length) const
  /*!
    Calculate the attenuation factor coefficient.
    \param Wave :: Wavelength [Angstrom]
    \param Length :: Absorption length
    \return Attenuation (including density)
  */
{
  return exp(-Length*density*TotalCross(Wave));
}

double
CryMat::ScatTotalRatio(const double Wave) const
  /*!
    Given a scatter return the ratio in scattering cross section
    against total.
    \param Wave :: Wavelength [Angstrom]
    \return sigma_scatter/sigma_total
  */
{
  if (density>0)
    {
      const double SC=ScatCross(Wave);
      const double AB=Wave*sabs/1.798;
      return SC/(SC+AB);
    }
  return 1.0;
}

} // NAMESPACE scatterSystem

