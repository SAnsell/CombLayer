/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transportInc/ParticleInObj.h
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
#ifndef Transport_ObjCompnent_h
#define Transport_ObjCompnent_h

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
    \class ParticleInObj 
    \author S. Ansell
    \date October 2012  
    \version 3.0
    \brief Method of tracking a particle with an object

    Object Component class, this class brings together the physical attributes 
    of the component to the positioning and geometry tree.
  */

template<typename PTYPE>
class ParticleInObj 
{  
 private: 
  
  const MonteCarlo::Object* ObjPtr;         ///< The phyical geometry 

 public:

  explicit ParticleInObj(const  MonteCarlo::Object* ObjPtr);
  ParticleInObj(const ParticleInObj&);
  ParticleInObj& operator=(const ParticleInObj&);
  ~ParticleInObj();

  double totalXSection(const MonteCarlo::particle&) const;

  int trackIntoCell(const MonteCarlo::particle&,
		    double&,const Geometry::Surface*&) const;

  int trackOutCell(const MonteCarlo::particle&,double&,
  		   const Geometry::Surface*&) const;
  int trackCell(const MonteCarlo::particle&,double&,double&) const;
  
  int trackWeight(MonteCarlo::neutron&,double&,
		  const Geometry::Surface*&) const;
  int trackAttn(MonteCarlo::neutron&,const Geometry::Surface*&) const;

  void attenuate(const double,MonteCarlo::particle&) const;
  double getRefractive(const MonteCarlo::particle&) const;
  double getRefractive(const double) const;
  double scatTotalRatio(const MonteCarlo::particle&,
			const MonteCarlo::particle&) const;

  
  int isValid(const Geometry::Vec3D&) const;       
  int hasIntercept(const MonteCarlo::particle&) const;
  void scatterParticle(MonteCarlo::particle&) const;
  
  virtual void write(std::ostream&) const;
  
};

template<typename PTYPE>
std::ostream& operator<<(std::ostream&,const ParticleInObj<PTYPE>&);

} // Namespace MonteCarlo

#endif 
