/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/SourceBase.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef SDef_SourceBase_h
#define SDef_SourceBase_h


class localRotate;
namespace Geometry
{
  class Transform;
}

namespace SDef
{
  class Source;
/*!
  \class SourceBase
  \version 1.0
  \author S. Ansell
  \date November 2017
  \brief Base for all source classes
*/

class SourceBase 
{
 protected:

  /// populate input type
  typedef std::map<std::string,std::vector<std::string>> ITYPE;

  std::string particleType;     ///< Particle Type
  double cutEnergy;             ///< Energy cut point
  
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights  [sum to 1.0]

  double weight;                  ///< Start particle weight
  Geometry::Transform* TransPtr;  ///< Transform [if required]

  Geometry::Vec3D polarVec;          ///< Polariaztion vector
  double polarFrac;                  ///< polar Fraction
  
  void createTransform(const Geometry::Vec3D&,const Geometry::Vec3D&,
		       const Geometry::Vec3D&,const Geometry::Vec3D&);

 public:

  SourceBase();
  SourceBase(const SourceBase&);
  SourceBase& operator=(const SourceBase&);
  virtual SourceBase* clone() const =0;
  virtual ~SourceBase() {}        ///< Destructor

  virtual void populate(const mainSystem::MITYPE&);

  /// Set particle type
  void setParticle(const int);
  /// Set particle type
  void setParticle(const std::string&);
  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }
  void setEnergy(const double);
  void setEnergy(const std::vector<double>&,const std::vector<double>&);
  void createEnergySource(SDef::Source&) const;
  void setPolarization(const Geometry::Vec3D&,const double);

  int setEnergy(std::string,std::string);
  int setEnergyFile(const std::string&,const int,const int,
		    const double,const double);
  
  /// No-op to substitue
  virtual void substituteSurface(const int,const int) {}
  /// No-op to rotate
  virtual void rotate(const localRotate&) { } 
  virtual void createSource(SDef::Source&) const =0;
  virtual void writePHITS(std::ostream&) const =0;
  virtual void writeFLUKA(std::ostream&) const;
  virtual void write(std::ostream&) const =0;
};

}

#endif
 
