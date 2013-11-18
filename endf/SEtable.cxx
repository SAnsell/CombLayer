/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endf/SEtable.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "support.h"
#include "mathSupport.h"
#include "RefCon.h"
#include "ENDF.h"
#include "SEtable.h"

namespace ENDF
{
  
SEtable::SEtable() : 
  nE(0)
  /*!
    Constructor
  */
{}			

SEtable::SEtable(const SEtable& A) : 
  nE(A.nE),E(A.E),sTot(A.sTot)
  /*!
    Copy Constructor
    \param A :: SEtable to copy
  */
{}			

SEtable&
SEtable::operator=(const SEtable& A) 
  /*!
    Assignment operator
    \param A :: SEtable to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nE=A.nE;
      E=A.E;
      sTot=A.sTot;
    }
  return *this;
}			
  

void 
SEtable::addEnergy(const double energy,const double sigma)
  /*!
    Adds an energy point. Care is taken to insert 
    them in the correct order [low->high]
    \param energy :: Energy point [eV]
    \param sigma :: cross section [barns]
  */
{
  nE++;
  std::vector<double>::iterator vc
    =lower_bound(E.begin(),E.end(),energy);
  if (vc==E.end())
    {
      E.push_back(energy);
      sTot.push_back(sigma);
    }
  else
    {
      size_t dist=vc-E.begin();
      E.insert(vc,energy);
      sTot.insert(sTot.begin()+dist,sigma);
    }
  return;
}


double
SEtable::STotal(const double energy) const
  /*!
    Calculate sigma_total for a neutron of energy E.
    \param energy :: energy [eV]
    \return \sigma_tot(E)
  */
{
  ELog::RegMethod RegA("SEtable","STotal");

  if (E.empty())
    ELog::EM<<"Energy empty "<<ELog::endErr;
  if (energy<=E.front())
    return sTot.front();
  if (energy>=E.back())
    return sTot.back();
  const long int eInt=mathFunc::binSearch(E.begin(),E.end(),energy);
  // Linear interpolation:
  const double frac=energy-E[eInt]/(E[eInt+1]-E[eInt]);
  return frac*sTot[eInt+1]+(1-frac)*sTot[eInt];
}



} // NAMESPACE ENDF

