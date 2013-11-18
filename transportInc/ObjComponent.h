/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/ObjComponent.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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

namespace Transport
{
  //forward declaration
  class neutron;
  class Track;

  /*!
    \class ObjComponent 
    \author S. Ansell
    \date October 2012  
    \version 2.0
    \brief Second light-weight version of ObjCompx

    Object Component class, this class brings together the physical attributes 
    of the component to the pos itioning and geometry tree.
  */

class ObjComponent 
{  
 private: 
  
  const MonteCarlo::Object* ObjPtr;         ///< The phyical geometry 
  /// Material Pointer 
  const scatterSystem::neutMaterial* MatPtr;

  static const scatterSystem::neutMaterial* neutMat(const int);

 public:

  explicit ObjComponent(const  MonteCarlo::Object* ObjPtr);
  ObjComponent(const ObjComponent&);
  ObjComponent& operator=(const ObjComponent&);
  ~ObjComponent();

  double ScatTotalRatio(const MonteCarlo::neutron&,
			const MonteCarlo::neutron&) const;
  double TotalCross(const MonteCarlo::neutron&) const;

  int trackIntoCell(const MonteCarlo::neutron&,
		    double&,const Geometry::Surface*&) const;

  int trackOutCell(const MonteCarlo::neutron&,double&,
			   const Geometry::Surface*&) const;
  int trackCell(const MonteCarlo::neutron&,double&,double&) const;

  int trackWeight(MonteCarlo::neutron&,double&,
		  const Geometry::Surface*&) const;
  int trackAttn(MonteCarlo::neutron&,const Geometry::Surface*&) const;

  void attenuate(const double,MonteCarlo::neutron&) const;
  double getRefractive(const MonteCarlo::neutron&) const;
  double getRefractive(const double) const;

  int isValid(const Geometry::Vec3D&) const;       
  int hasIntercept(const MonteCarlo::neutron&) const;
  void scatterNeutron(MonteCarlo::neutron&) const;

  virtual void selectEnergy(const MonteCarlo::neutron&,
			    MonteCarlo::neutron&) const;

  virtual void write(std::ostream&) const;
  virtual void writeMCNPX(std::ostream&) const;
  
};

std::ostream& operator<<(std::ostream&,const ObjComponent&);

} // Namespace MonteCarlo

#endif 
