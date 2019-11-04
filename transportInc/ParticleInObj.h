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
  class partcile;
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

template<typename PTYPE,typename MatTYPE>
class ParticleInObj 
{  
 private: 
  
  const MonteCarlo::Object* ObjPtr;         ///< The phyical geometry 

  const MatTYPE* MatPtr;                    ///< Material Pointer 

  static const scatterSystem::neutMaterial* neutMat(const int);

 public:

  explicit ParticleInObj(const  MonteCarlo::Object* ObjPtr);
  ParticleInObj(const ParticleInObj&);
  ParticleInObj& operator=(const ParticleInObj&);
  ~ParticleInObj();

  double ScatTotalRatio(const MonteCarlo::neutron&,
			const MonteCarlo::neutron&) const;
  double TotalCross(const MonteCarlo::neutron&) const;

  int trackIntoCell(const MonteCarlo::neutron&,
		    double&,const Geometry::Surface*&) const;

  int trackOutCell(const MonteCarlo::partcile&,double&,
		   const Geometry::Surface*&) const;
  int trackCell(const MonteCarlo::particle&,double&,double&) const;
  
  int trackWeight(MonteCarlo::neutron&,double&,
		  const Geometry::Surface*&) const;
  int trackAttn(MonteCarlo::neutron&,const Geometry::Surface*&) const;

  void attenuate(const double,MonteCarlo::particle&) const;
  double getRefractive(const MonteCarlo::particle&) const;
  double getRefractive(const double) const;

  int isValid(const Geometry::Vec3D&) const;       
  int hasIntercept(const MonteCarlo::particle&) const;
  void scatterNeutron(MonteCarlo::neutron&) const;

  virtual void selectEnergy(const MonteCarlo::neutron&,
			    MonteCarlo::neutron&) const;
  
  virtual void write(std::ostream&) const;
  
};

std::ostream& operator<<(std::ostream&,const ParticleInObj&);

} // Namespace MonteCarlo

#endif 
