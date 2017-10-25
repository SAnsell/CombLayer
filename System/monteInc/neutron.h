/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/neutron.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef MonteCarlo_neutron_h
#define MonteCarlo_neutron_h


/*!
  \namespace moderatorSystem
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief CompLayer internal transport
*/

namespace MonteCarlo
{

  class Object;
/*!  
  \class neutron
  \brief Defines single point neutron
  \version 1.0
  \author S. Ansell
  \date December 2009
*/

class neutron
{
 private:

  static int masterID;      ///< ID iteration to next new neutron

 public:

  int ID;                   ///< ID number

  double wavelength;        ///< Wavelength [A]
  Geometry::Vec3D Pos;      ///< Position
  Geometry::Vec3D uVec;     ///< Direction 
  double weight;            ///< Weight
  double travel;            ///< Distance travelled
  double time;              ///< Time Travelled
  double nCollision;        ///< Number of collisions
  const Object* OPtr;             ///< Object for collision [if set]

  neutron(const double,const Geometry::Vec3D&,const Geometry::Vec3D&);
  neutron(const neutron&);
  neutron& operator=(const neutron&);
  ~neutron() {}  ///< Destructor
  
  int equalityFlag(const neutron&) const;

  double velocity() const;
  void moveForward(const double);
  int refract(const double,const double,const Geometry::Vec3D&);
  double energy() const;
  void setEnergy(const double);
  double Q(const neutron&) const;
  double eLoss(const neutron&) const;
  void addCollision();
  void setObject(const Object* OP) { OPtr=OP; }

  // Output stuff
  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const neutron&);


}  // NAMESPACE MonteCarlo

#endif
