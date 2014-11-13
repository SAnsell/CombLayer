/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/Beam.h
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
#ifndef Transport_Beam_h
#define Transport_Beam_h

namespace Transport
{
  
  class OutZone;
/*!  
  \class Beam
  \brief   Top class for a source
  \version 1.0
  \author S. Ansell
  \date December 2009
*/

class Beam
{
 public:
  
  Beam() {}                       ///< Constructonr 
  Beam(const Beam&) {}            ///< Copy Contructor
  /// Assignment operator 
  Beam& operator=(const Beam&) { return *this; } 
  virtual Beam* clone() const =0; ///< Clone object
  virtual ~Beam() {}              ///< Destructor

  /// Effective typeid
  virtual std::string className() const { return "Beam"; }

  /// set the aim zone
  virtual void setAimZone(const OutZone*) { }

  ///\cond VIRTUAL
  virtual void setBias(const double) =0;
  virtual void setWavelength(const double) =0;
  virtual MonteCarlo::neutron generateNeutron() const =0;
  ///\endcond VIRTUAL 

  /// Output stuff
  virtual void write(std::ostream&) const {}

};

}  // NAMESPACE Transport

#endif
