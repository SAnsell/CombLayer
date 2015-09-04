/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/SimMonte.h
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

  size_t TCount;                    ///< Total counts 

  int MSActive;                       ///< Multi-scattering [0-all,-1=>single]
  Transport::Beam* B;                 ///< Main Beam (init partiles)
  Transport::DetGroup DUnit;          ///< Detector Units
  
 public:
  
  SimMonte();
  SimMonte(const SimMonte&);
  SimMonte& operator=(const SimMonte&);
  virtual ~SimMonte();

  void clearAll();
  // MAIN RUN:
  void runMonte(const size_t);
  void setBeam(const Transport::Beam&);
  void setDetector(const Transport::Detector&);
  void setMS(const int M) { MSActive=M; }

  void attenPath(const MonteCarlo::Object*,const double,
		 MonteCarlo::neutron&) const;

  Transport::Beam* getBeam() const { return B; }

  /// access detector unit
  Transport::DetGroup& getDU() { return DUnit; }

  void normalizeDetectors();
  void writeDetectors(const std::string&,const double) const;

  virtual void write(const std::string&) const;
};

#endif
