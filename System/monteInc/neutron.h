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
  \brief Defines single point neutron [special case]
  \version 2.0
  \author S. Ansell
  \date March 2019
*/

class neutron : public particle
{
 private:


 public:

  neutron(const double,const Geometry::Vec3D&,const Geometry::Vec3D&);
  neutron(const neutron&);
  neutron& operator=(const neutron&);
  ~neutron() override {}  ///< Destructor
  
};


}  // NAMESPACE MonteCarlo

#endif
