/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transportInc/photonInObj.h
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
#ifndef Transport_photonInObj_h
#define Transport_photonInObj_h

namespace MonteCarlo
{
  class particle;
  class neutron;
  class photon;
  class Track;
}

namespace Transport
{  
  /*!
    \class photonInObj 
    \author S. Ansell
    \date Dembember 2022  
    \version 1.0
    \brief Method of tracking a particle with an object

    Object Component class, this class brings together the physical attributes 
    of the component to the positioning and geometry tree.
  */

template<typename PTYPE>
class photonInObj 
{  
 private: 
  
  const MonteCarlo::Object* ObjPtr;         ///< The phyical geometry 

 public:

  explicit photonInObj(const  MonteCarlo::Object* ObjPtr);
  photonInObj(const photonInObj&);
  photonInObj& operator=(const photonInObj&);
  ~photonInObj();

  double totalXSection(const MonteCarlo::particle&) const;

  int trackCell(const MonteCarlo::particle&,double&,double&) const;
  
  int trackWeight(MonteCarlo::neutron&,double&,
		  const Geometry::Surface*&) const;
  int trackAttn(MonteCarlo::neutron&,const Geometry::Surface*&) const;

  void attenuate(const double,MonteCarlo::particle&) const;
  double getRefractive(const MonteCarlo::particle&) const;
  double getRefractive(const double) const;
  double scatTotalRatio(const MonteCarlo::particle&,
			const MonteCarlo::particle&) const;

  virtual void write(std::ostream&) const;
  
};

} // Namespace MonteCarlo

#endif 
