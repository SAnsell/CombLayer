/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/masterRotate.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "particleConv.h"



particleConv::particleConv() : 
  indexLookup{
  {"n",{"n",1,"neutron",2,2112}},   
    {"h",{"h",9,"proton",1,2212}},
      {"p",{"p",2,"photon",14,22}}
  }
  /*!
    Constructor
  */
{}

const particleConv&
particleConv::Instance()
  /*!
    Singleton this
    \return particleConv object
   */
{
  static particleConv MR;
  return MR;
}


const std::string&
particleConv::phitsType(const std::string& MC) const
  /*
    Accessor to phitsType 
    \param MC :: MCNP Name
    
  */
{
  std::map<std::string,pName>::const_iterator mc;
  mc=indexLookup.find(MC);
  if (mc == indexLookup.end())
    throw ColErr::InContainerError<std::string>
      (MC,"MCNPname not in particleDataBase");

  return mc->second.phitsName;
}

pName::pName(const std::string& MChar,const int mcnpN,
	     const std::string& PhitsN,const int pI,
	     const int mcplN) :
      mcnpName(MChar),mcnpITYP(mcnpN),
      phitsName(PhitsN),phitsITYP(pI),
      mcplNumber(mcplN)
  /*
    Constructor 
    \param MChar :: MCNP Name
    \param mcnpN :: MCNP number
    \param PhitsN :: Phits name
    \param pI :: phits ityp number
    \param mcplN :: kf/mcpl number
  */
{}

pName::pName(const pName& A) :
      mcnpName(A.mcnpName),mcnpITYP(A.mcnpITYP),
      phitsName(A.phitsName),phitsITYP(A.phitsITYP),
      mcplNumber(A.mcplNumber)
  /*
    Copy Constructor 
    \param A :: Object to copy
  */
{}

 
