/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsPhysics/phitsPhysics.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "MapRange.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "particleConv.h"
#include "cellValueSet.h"
#include "pairValueSet.h"
#include "phitsWriteSupport.h"
#include "phitsPhysics.h"

namespace phitsSystem
{
		       
phitsPhysics::phitsPhysics() :
  particleECut
  ({
    {"electron",1.0},
    {"positron",1.0},
    {"photon",0.010}
  }),
  libEMax
  ({
    {"neutron",20.0},    // set for high energy
    {"electron",3000.0},  
    {"positron",1000.0},
    {"photon",3000.0},
  }),

  flags
  ({
    { "nucdata", {1 ,  "JENDL-4.0 default neutrons libs"}},
    { "negs" ,   {1,  "Use egs5 electron [D=0]" }},   
    { "nspred",  {2,  "Lynch(Moliere) scattering"}},
    { "nedisp",  {1,  "Straggling events [D=0]" }},
    { "e-mode",  {2,  "Create secondary events from electrons [D=0]"}},
    { "igmuppd", {1,  "Photon induced muon-pair production [D=0]"}},
    { "ipnint",  {1,  "Photo-nuclear reaction [D=0]"}}
  })
  /*!
    Constructor
  */
{}

phitsPhysics::phitsPhysics(const phitsPhysics& A) :
  particleECut(A.particleECut)
  /*!
    Copy constructor
    \param A :: phitsPhysics to copy
  */
{}

phitsPhysics&
phitsPhysics::operator=(const phitsPhysics& A)
  /*!
    Assignment operator
    \param A :: phitsPhysics to copy
    \return *this
  */
{
  if (this!=&A)
    {
      particleECut=A.particleECut;
    }
  return *this;
}

phitsPhysics::~phitsPhysics()
  /*!
    Destructor
  */
{}

void
phitsPhysics::writePHITS(std::ostream& OX) const
  /*!
    Write out all the non-default and stuff that needs 
    a reminder
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("phitsPhysics","writePHITS");
  
  const particleConv& PC=particleConv::Instance();
  for(const auto& [ particle , value ] :   particleECut)
    {
      const int n=PC.phitsITYP(particle);
      StrFunc::writePHITSIndex(OX,1,"emin",n,value,particle);
    }

  for(const auto& [ flag , IVal ] :   flags)
    StrFunc::writePHITS(OX,1,flag,IVal.first,IVal.second);
  
  for(const auto& [ particle , value ] :   libEMax)
    {
      const int n=PC.phitsITYP(particle);
      StrFunc::writePHITSIndex(OX,1,"dmax",n,value,particle);
    }

      
      
  return;
}
  
  
} // NAMESPACE phitsSystem
      
   
