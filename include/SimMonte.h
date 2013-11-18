/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/SimMonte.h
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
#ifndef SimMonte_h
#define SimMonte_h

namespace Transport
{
  class Detector;
  class DetGroup;
  class Beam;
}

/*!
  \class SimMonte
  \brief Modifides Simulation to to MC transport
  \author S. Ansell
  \version 1.0
  \date October 2012
 */

class SimMonte : public Simulation
{
 private:

  long int TCount;                    ///< Total counts 
  Transport::Beam* B;                 ///< Main Beam (init partiles)
  Transport::DetGroup DUnit;          ///< Detector Units
  
 public:
  
  SimMonte();
  SimMonte(const SimMonte&);
  SimMonte& operator=(const SimMonte&);
  ~SimMonte();

  void clearAll();
  // MAIN RUN:
  void runMonte(const size_t);
  void setBeam(const Transport::Beam&);
  void setDetector(const Transport::Detector&);
  
  void writeDetectors(const std::string&,const double) const;
  void write(const std::string&) const;

};

#endif
