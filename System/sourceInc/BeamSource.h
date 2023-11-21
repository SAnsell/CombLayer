/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/BeamSource.h
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
#ifndef SDef_BeamSource_h
#define SDef_BeamSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class BeamSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Circular Beam source
*/

class BeamSource : 
  public attachSystem::FixedRotateUnit,
  public SourceBase
{
 private:

  double radius;                ///< spot size
  double angleSpread;           ///< Angle spread [deg]
  
  void populate(const ITYPE&) override;

 public:

  BeamSource(const std::string&);
  BeamSource(const BeamSource&);
  BeamSource& operator=(const BeamSource&);
  BeamSource* clone() const override;
  ~BeamSource() override;

  /// Set radius
  void setRadius(const double R) { radius=R; }



  using FixedComp::createAll;
  void createAll(const ITYPE&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(const ITYPE&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const Geometry::Vec3D&);
  void createAll(const attachSystem::FixedComp&,
		 const long int);

  void rotate(const localRotate&) override;
  void createSource(SDef::Source&) const override;

  void write(std::ostream&) const override;
  void writePHITS(std::ostream&) const override;
  void writeFLUKA(std::ostream&) const override;

};

}

#endif
 
