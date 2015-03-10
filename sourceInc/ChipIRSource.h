/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   sourceInc/ChipIRSource.h
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
#ifndef ChipIRSource_h
#define ChipIRSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class ChipIRSource
  \version 1.0
  \author S. Ansell
  \date August 2010
  \brief Adds a new source in the guide system
*/

class ChipIRSource 
{
 private:
  
  double cutEnergy;             ///< Energy cut point
  double weight;                ///< neutron weight
  Geometry::Vec3D CentPoint;    ///< Origin Point [Start shutter]
  Geometry::Vec3D Direction;    ///< Direction of centre
  double angleSpread;           ///< Angle spread
  double radialSpread;          ///< Radial spread
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights
  
  void loadEnergy(const std::string&);
  void createSource(SDef::Source&) const;

 public:

  ChipIRSource();
  ChipIRSource(const ChipIRSource&);
  ChipIRSource& operator=(const ChipIRSource&);
  ~ChipIRSource();

  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }

  void createAll(const std::string&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const double,
		 const double,SDef::Source&);

  void createAll(const std::string&,
		 const attachSystem::FixedComp&,
		 const double,const double,SDef::Source&);

  /// Access central point:
  const Geometry::Vec3D& getCentPoint() const { return CentPoint; }
  /// Access central axis:
  const Geometry::Vec3D& getDirection() const { return Direction; }
  
};

}

#endif
 
