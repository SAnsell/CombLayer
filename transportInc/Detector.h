/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/Detector.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef Transport_Detector_h
#define Transport_Detector_h

namespace Transport
{

/*!  
  \class Detector
  \brief Defines a detector
  \version 1.0
  \author S. Ansell
  \date October 2014
*/

class Detector
{
 protected:

  const size_t index;         ///< Index number
  size_t nps;              ///< Number of points

 public:
  
  explicit Detector(const size_t);
  Detector(const Detector&);
  Detector& operator=(const Detector&);
  virtual Detector* clone() const =0;
  virtual ~Detector();
  
  virtual double project(const MonteCarlo::neutron&,
		       MonteCarlo::neutron&) const =0;
  virtual void addEvent(const MonteCarlo::neutron&) =0;

  virtual void clear() =0;
  virtual void normalize(const size_t) {}
  virtual void writeHeader(std::ostream&) const {}
  virtual void write(std::ostream&) const =0;
};

std::ostream&
operator<<(std::ostream&,const Detector&);

}  // NAMESPACE Transport

#endif
