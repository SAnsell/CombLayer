/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/ActivationSource.h
*
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef SDef_ActivationSource_h
#define SDef_ActivationSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class ActivationSource
  \version 1.0
  \author S. Ansell
  \date September 2016
  \brief Creates an active sourcex
*/

class ActivationSource 
{
 private:

  Geometry::Vec3D Origin;         ///< Origin 
  Geometry::Vec3D Axis;           ///< Beam Axis
  double distWeight;              ///< Distance weight
  double angleWeight;             ///< Angular weight
  
  Geometry::Vec3D ABoxPt;         ///< Bounding box corner
  Geometry::Vec3D BBoxPt;         ///< Bounding box corner


 public:

  ActivationSource();
  ActivationSource(const ActivationSource&);
  ActivationSource& operator=(const ActivationSource&);
  ~ActivationSource();

  /// Set cut energy
  void addMaterial(const std::string&,const std::string&);
  void setBiasConst(const Geometry::Vec3D&,const Geometry::Vec3D&,
		    const double,const double);
  void setBox(const Geometry::Vec3D&,const Geometry::Vec3D&);
    
  void createSource(Simulation&);
		 
  
};

}

#endif
 
