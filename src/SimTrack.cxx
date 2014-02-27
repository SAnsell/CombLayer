/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/SimTrack.cxx
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
#include <set>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "SimTrack.h"


namespace ModelSupport
{

SimTrack::SimTrack() 
  /*!
    Constructor
  */
{}

SimTrack&
SimTrack::Instance()
  /*!
    Singleton this
    \return SimTrack object
   */
{
  static SimTrack ST;
  return ST;
}

void
SimTrack::clearSim(const Simulation* SimPtr)
  /*!
    Removes a particular simulation
    \param SimPtr :: Simulation pointer
  */
{
  fcTYPE::iterator mc=findCell.
    find(reinterpret_cast<fcTYPE::key_type>(SimPtr));
  if (mc!=findCell.end())
    findCell.erase(mc);
  return;
}

void
SimTrack::addSim(const Simulation* SimPtr)
  /*!
    Adds a new Simulation
    \param SimPtr :: Simulation pointer
  */
{
  const fcTYPE::key_type sInt=
    reinterpret_cast<const fcTYPE::key_type>(SimPtr);
  fcTYPE::iterator mc=findCell.find(sInt);
  if (mc==findCell.end())
    findCell.insert(fcTYPE::value_type(sInt,0));
  return;
}

void
SimTrack::setCell(const Simulation* SimPtr,MonteCarlo::Object* OPtr)
  /*!
    Set the current cell pointer
    \param SimPtr :: Significant figures
    \param OPtr :: Object Pointer
  */
{
  ELog::RegMethod RegA("SimTrack","setCell");

  fcTYPE::key_type sInt=reinterpret_cast<fcTYPE::key_type>(SimPtr);
  fcTYPE::iterator mc=findCell.find(sInt);
  if (mc==findCell.end())
    {
      throw ColErr::InContainerError<fcTYPE::key_type>
	(sInt,"sInt not fould in findCell");
    }
  mc->second=OPtr;
  return;
}

MonteCarlo::Object*
SimTrack::curCell(const Simulation* SimPtr) const
  /*!
    Get the current cell
    \param SimPtr :: Significant figures
    \return :: Object Pointer
  */
{
  ELog::RegMethod RegA("SimTrack","curCell");

  fcTYPE::key_type sInt=reinterpret_cast<fcTYPE::key_type>(SimPtr);
  fcTYPE::const_iterator mc=findCell.find(sInt);
  if (mc==findCell.end())
    throw ColErr::InContainerError<fcTYPE::key_type>
      (sInt,"simluation<long Int>");
  return mc->second;
}

void
SimTrack::checkDelete(const Simulation* SimPtr,MonteCarlo::Object* OPtr)
  /*!
    Determine is a delete is ok
    \param SimPtr :: SimPtr to determine valid cells
    \param OPTr :: Object pointer being deleted
  */
{
  ELog::RegMethod RegA("SimTrack","checkDelete");
  if (curCell(SimPtr)==OPtr)
    setCell(SimPtr,0);

  return;
}

} // NAMESPACE ModelSupport
