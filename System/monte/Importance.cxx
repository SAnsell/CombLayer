/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Importance.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <set>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "particleConv.h"
#include "Importance.h"

namespace MonteCarlo
{

std::ostream&
operator<<(std::ostream& OX,const Importance& A)
  /*!
    Basic write operator
    \param OX :: Output stream
    \param A :: Importance to write
   */
{
  A.write(OX);
  return OX;
}
 
  
constexpr double zeroImpTol(1e-12);
  
Importance::Importance() :
  zeroImp(0),allSame(1),defValue(1.0)
  /*!
    Constructor 
  */
{}

Importance::Importance(const Importance& A) :
  zeroImp(A.zeroImp),
  allSame(A.allSame),
  defValue(A.defValue),
  particles(A.particles),
  impMap(A.impMap)
  /*!
    Copy constructor
    \param A :: Importance to copy
  */
{}

Importance&
Importance::operator=(const Importance& A)
  /*!
    Assignment operator
    \param A :: Importance to copy
    \return *this
  */
{
  if (this!=&A)
    {
      zeroImp=A.zeroImp;
      allSame=A.allSame;
      defValue=A.defValue;
      particles=A.particles;
      impMap=A.impMap;
    }
  return *this;
}

void
Importance::setImp(const std::string& particle,const double V)
  /*!
    Set particle importance
    \param particle :: Particle name 
    \param V :: Value to set
   */
{
  ELog::RegMethod RegA("Importance","setImp(particle)");

  if (particle=="all" || particle=="All")
    {
      setImp(V);
      return;
    }
  const int index=particleConv::Instance().mcplITYP(particle);
  const double VV=(V<0) ? 0.0 : V;

  particles.emplace(index);
  impMap[index]=VV;
  
  if (allSame && std::abs<double>(VV-defValue)>zeroImpTol)
    allSame=0;

  zeroImp=(allSame && std::abs<double>(VV)<zeroImpTol) ? 1 : 0;
    
  return;
}
  
void
Importance::setImp(const double V)
  /*!
    Set all the importances
    \param V :: Value to set
   */
{
  particles.clear();
  impMap.clear();
  allSame=1;
  if (V<zeroImpTol)
    {
      defValue=0.0;
      zeroImp=1;
    }
  else
    defValue=V;

  return;
}

double
Importance::getAllImp() const
  /*!
    Get the standard importance for all particles
    \throw NumericalAbort :: if no general default value
    \return value
   */
{
  ELog::RegMethod RegA("Importance","getImp(All)");

  if (zeroImp) return 0.0;
  if (allSame) return defValue;
  throw ColErr::NumericalAbort("No default value");
}

std::tuple<bool,double>
Importance::getAllPair() const
  /*!
    Get the standard importance for all particles
    \throw NumericalAbort :: if no general default value
    \return value
   */
{
  if (zeroImp) return std::make_tuple(1,0.0);
  if (allSame) return std::make_tuple(1,defValue);
  return std::make_tuple(0,0.0);
}
  
double
Importance::getImp(const std::string& particle) const
  /*!
    Get the standard importance for all particles
    \throw NumericAbort :: if no general default value
    \return value
   */
{
  ELog::RegMethod RegA("Importance","getImp(particle)");

  if (zeroImp) return 0.0;
  if (allSame) return defValue;
  const int index=particleConv::Instance().mcplITYP(particle);

  std::map<int,double>::const_iterator mc=impMap.find(index);
  if (mc!=impMap.end())
    throw ColErr::InContainerError<std::string>(particle,"Particle in ImpMap");

  return mc->second;
}

double
Importance::getImp(const int particleID) const
  /*!
    Get the standard importance for all particles
    \throw NumericAbort :: if no general default value
    \return value
   */
{
  ELog::RegMethod RegA("Importance","getImp(particleID)");

  if (zeroImp) return 0.0;
  if (allSame) return defValue;

  std::map<int,double>::const_iterator mc=impMap.find(particleID);
  if (mc!=impMap.end())
    throw ColErr::InContainerError<int>(particleID,"Particle in ImpMap");

  return mc->second;
}
  
void
Importance::write(std::ostream& OX) const
  /*!
    Write out importance
    \param OX :: Output stream
   */
{
  const particleConv& pConv=particleConv::Instance();
  
  OX<<"zFlag "<<zeroImp<<" allFlag "<<allSame<<" D="<<defValue<<"\n";
  for(const int PN : particles)
    {
      const std::string pName=pConv.mcplToFLUKA(PN);
      std::map<int,double>::const_iterator mc=
	impMap.find(PN);
      
      OX<<pName<<"["<<PN<<"] : "<<mc->second<<"\n";
    }
  if (particles.empty())
    OX<<"No particles\n";

  return;
}
  
}  // NAMESPACE MonteCarlo
