/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SimPhoton.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#ifndef SimPhoton_h
#define SimPhoton_h

namespace MonteCarlo
{
  class particle;
  class neutron;
  class photon;
}

namespace Transport
{
  class countUnit;
}

/*!
  \class SimPhoton
  \brief Modifides Simulation to to MC transport
  \author S. Ansell
  \version 1.0
  \date October 2012
 */

class SimPhoton : public Simulation
{
 private:

  size_t TCount;                             ///< Total counts 

  Transport::Beam* B;                       ///< Main Beam (init partiles)
  std::set<Transport::ChannelDet*> DSet;    ///< Detector Units

  void writeSurfaces(std::ostream&) const;
  void writeCells(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  
 public:
  
  SimPhoton();
  SimPhoton(const SimPhoton&);
  SimPhoton& operator=(const SimPhoton&);
  virtual ~SimPhoton();

  void clearAll();

  void setDefMaterial(const double);
  
  // MAIN RUN:
  void runMonte(const size_t);
  void setBeam(const Transport::Beam&);
  void addDetector(const Transport::ChannelDet&);
  void writeDetectors(const std::string&,const double) const;

  virtual void write(const std::string&) const;
};

#endif
