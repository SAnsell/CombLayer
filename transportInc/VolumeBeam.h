/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transportInc/VolumeBeam.h
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
#ifndef Transport_VolumeBeam_h
#define Transport_VolumeBeam_h

namespace Transport
{

/*!  
  \class VolumeBeam
  \brief   Volume source [fixed wavelength]
  \version 1.0
  \author S. Ansell
  \date December 2009
*/

class VolumeBeam : public Beam
{
 private: 
  
  double wavelength;           ///< Wavelength [A]

  Geometry::Vec3D Corner;      ///< Lower corner 
  Geometry::Vec3D X;           ///< Axis 1  [L+X+Y+Z == extent]
  Geometry::Vec3D Y;           ///< Axis 2
  Geometry::Vec3D Z;           ///< Axis 3 
  
  double yBias;                ///< Bias in y for neutron position
  
 public:
  
  VolumeBeam();
  VolumeBeam(const VolumeBeam&);
  VolumeBeam& operator=(const VolumeBeam&);
  /// Clone operation
  Beam* clone() const override { return new VolumeBeam(*this); }  
  ~VolumeBeam() override;

  double wave() const { return wavelength; }
  const Geometry::Vec3D& getCorner() const { return Corner; }

  void setWavelength(const double W) override { wavelength=W; }  
  void setBias(const double YB) override { yBias=YB; }

  ///\endcond GET/SETTER
  
  const Geometry::Vec3D& getAxis(const int) const;
  void setCorners(const Geometry::Vec3D&,const Geometry::Vec3D&);
  
  MonteCarlo::neutron generateNeutron() const override;
  MonteCarlo::photon generatePhoton() const override;
  
  void write(std::ostream&) const override;

};

}  // NAMESPACE Transport

#endif
