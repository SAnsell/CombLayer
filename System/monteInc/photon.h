/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/photon.h
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
#ifndef MonteCarlo_photon_h
#define MonteCarlo_photon_h


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
  \class photon
  \brief Defines single point photon [special case]
  \version 2.0
  \author S. Ansell
  \date March 2019
*/

class photon : public particle
{
 private:


 public:

  photon(const double,const Geometry::Vec3D&,const Geometry::Vec3D&);
  photon(const Geometry::Vec3D&,const Geometry::Vec3D&,const double);
  photon(const photon&);
  photon& operator=(const photon&);
  virtual ~photon() {}  ///< Destructor
  
};


}  // NAMESPACE MonteCarlo

#endif
