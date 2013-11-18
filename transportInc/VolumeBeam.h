/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/VolumeBeam.h
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
  
  double yBias;                ///< Bias in y
  
 public:
  
  VolumeBeam();
  VolumeBeam(const VolumeBeam&);
  VolumeBeam& operator=(const VolumeBeam&);
  /// Clone operation
  Beam* clone() const { return new VolumeBeam(*this); }  
  virtual ~VolumeBeam();

  /// Effective typeid
  virtual std::string className() const { return "VolumeBeam"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  /// Get wavelength
  double wave() const { return wavelength; }      
  /// get corner point
  const Geometry::Vec3D& getCorner() const { return Corner; }
  const Geometry::Vec3D& getAxis(const int) const;

  /// set bias
  void setBias(const double YB) { yBias=YB; }


  void setCorners(const Geometry::Vec3D&,const Geometry::Vec3D&);
  /// Set Wavelength
  virtual void setWavelength(const double W) { wavelength=W; }  

  virtual MonteCarlo::neutron generateNeutron() const;

  // Output stuff
  void write(std::ostream&) const;

};

}  // NAMESPACE Transport

#endif
