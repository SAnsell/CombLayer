/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/particle.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef MonteCarlo_particle_h
#define MonteCarlo_particle_h

namespace MonteCarlo
{

  class Object;
  
/*!  
  \class particle
  \brief Defines single point particle
  \version 1.0
  \author S. Ansell
  \date March 2019
*/

class particle
{
 private:

  static long int masterID;      ///< ID iteration to next new particle

 public:

  int typeID;               ///< particle type [phits name ID]
  long int ID;              ///< ID number

  double energy;            ///< Kinetic Energy [MeV] 
  double wavelength;        ///< Wavelength [Angstrom]
  Geometry::Vec3D Pos;      ///< Position
  Geometry::Vec3D uVec;     ///< Direction 
  double weight;            ///< Weight
  double travel;            ///< Distance travelled
  double time;              ///< Time Travelled
  size_t nCollision;        ///< Number of collisions

  const Object* OPtr;       ///< Object for collision [if set]

  particle(const Geometry::Vec3D&,const Geometry::Vec3D&);
  particle(const std::string&,const Geometry::Vec3D&,
	   const Geometry::Vec3D&,const double);
  particle(const particle&);
  particle& operator=(const particle&);
  virtual ~particle() {}  ///< Destructor
  
  int equalityFlag(const particle&) const;

  double velocity() const;
  void moveForward(const double);
  int refract(const double,const double,const Geometry::Vec3D&);

  void setEnergy(const double);
  double Q(const particle&) const;
  double eLoss(const particle&) const;
  void addCollision();
  void setObject(const Object* OP) { OPtr=OP; }

  // Output stuff
  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const particle&);


}  // NAMESPACE MonteCarlo

#endif
