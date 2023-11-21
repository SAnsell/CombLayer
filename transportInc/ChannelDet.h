/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transportInc/ChannelDet.h
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
#ifndef Transport_ChannelDet_h
#define Transport_ChannelDet_h

namespace MonteCarlo
{
  class particle;
}

namespace Transport
{

/*!  
  \class ChannelDet
  \brief Defines a detector channel for data
  \version 1.0
  \author S. Ansell
  \date December 2022

  This is constructed to ahve a fixed set of names
  and a fixed set of energy bins
*/

class ChannelDet :
    public attachSystem::FixedRotate
{
 private:

  const size_t index;         ///< Index number 
  std::vector<countUnit> units;

 public:
  
  explicit ChannelDet(const size_t,size_t,size_t,
		      const std::string&);  
  ChannelDet(const ChannelDet&);
  virtual ChannelDet* clone() const;
  ChannelDet& operator=(const ChannelDet&);
  ~ChannelDet() override;

  virtual void clear();
  virtual double project(const size_t,
			 MonteCarlo::particle&) const =0;
  virtual void addEvent(const std::string&,
			const MonteCarlo::particle&) =0;
  virtual void addEvent(const size_t,
			const MonteCarlo::particle&) =0;

  virtual void writeHeader(std::ostream&) const {}
  virtual void write(std::ostream&) const =0;
};

std::ostream&
operator<<(std::ostream&,const ChannelDet&);

}  // NAMESPACE Transport

#endif
