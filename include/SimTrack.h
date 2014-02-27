/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/SimTrack.h
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
#ifndef ModelSupport_SimTrack_h
#define ModelSupport_SimTrack_h


class Simulation;

namespace MonteCarlo
{
  class Object;
}

namespace ModelSupport
{

/*!
  \class SimTrack
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Keeps track of the last utilized object pointer

  In a given simulation tracks or isValid operations based on points
  typically start from the last used cell : This keeps a track of the 
  last used cell as an optimization point.
*/


class SimTrack
{
 private:

  /// Storage of the findCell Ptr
  typedef std::map<unsigned long int,MonteCarlo::Object*> fcTYPE;
  fcTYPE findCell;   ///< Find cell Map

  SimTrack();

  ///\cond SINGLETON
  SimTrack(const SimTrack&);
  SimTrack& operator=(const SimTrack&);
  ///\endcond SINGLETON
  
 public:

  static SimTrack& Instance();

  // Cell Ptr:
  MonteCarlo::Object* curCell(const Simulation*) const;
  void setCell(const Simulation*,MonteCarlo::Object*);

  // General
  void checkDelete(const Simulation*,MonteCarlo::Object*);
  void clearSim(const Simulation*);
  void addSim(const Simulation*);

};

}

#endif
