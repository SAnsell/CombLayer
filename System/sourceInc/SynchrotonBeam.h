/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/SynchrotonBeam.h
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
#ifndef SDef_SynchrotonBeam_h
#define SDef_SynchrotonBeam_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class SynchrotonBeam
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Synchroton beam from Fluka

  The origin is the centre point of the wiggler/undulator
  The beamXYZ is the offset towards the emmision point
  Z is assumed to be the magnetic direction and Y the emmision
  direction [centre]
*/

class SynchrotonBeam : 
  public attachSystem::FixedRotate,
  public SourceBase
{
 private:

  double electronEnergy;        ///< Electron energy
  double magneticField;         ///< Magnetic field
  double lowEnergyLimit;        ///< Low energy limit
  double arcLength;             ///< Length of arc

  Geometry::Vec3D beamXYZ;      ///< Start point of beam [origin for emission]
  
  void populate(const mainSystem::MITYPE&) override;

 public:

  SynchrotonBeam(const std::string&);
  SynchrotonBeam(const SynchrotonBeam&);
  SynchrotonBeam& operator=(const SynchrotonBeam&);
  SynchrotonBeam* clone() const override;
  ~SynchrotonBeam() override;


  void rotate(const localRotate&) override;
  void createSource(SDef::Source&) const override;
  void write(std::ostream&) const override;
  void writePHITS(std::ostream&) const override;
  void writeFLUKA(std::ostream&) const override;

  using FixedComp::createAll;
  void createAll(const mainSystem::MITYPE&,
		 const attachSystem::FixedComp&,
		 const long int);
  void createAll(const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
