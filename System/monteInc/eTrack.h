/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/eTrack.h
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
#ifndef MonteCarlo_eTrack_h
#define MonteCarlo_eTrack_h

namespace MonteCarlo
{

  class Object;
  
/*!  
  \class eTrack
  \brief Special non-particle particle type
  \version 1.0
  \author S. Ansell
  \date March 2019
*/

class eTrack : public particle
{

 public:

  eTrack(const Geometry::Vec3D&,const Geometry::Vec3D&);
  eTrack(const eTrack&);
  eTrack& operator=(const eTrack&);
  virtual ~eTrack() {}  ///< Destructor
  
  virtual double velocity() const { return 1; }
  virtual void setEnergy(const double) { return; }
  virtual void moveForward(const double);

  // Output stuff
  virtual void write(std::ostream&) const;

};


}  // NAMESPACE MonteCarlo

#endif
