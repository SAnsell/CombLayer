/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/LayerComp.h
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
#ifndef attachSystem_LayerComp_h
#define attachSystem_LayerComp_h

namespace attachSystem
{
/*!
  \class LayerComp
  \version 1.0
  \author S. Ansell
  \date February 2013
  \brief Component that is at a fixed position
*/

class LayerComp  
{
 protected:
  
  size_t nLayers;         ///< Number of layers
  size_t nInnerLayers;    ///< Number of inner layers

 public:
    
  LayerComp(const size_t,const size_t =0);
  LayerComp(const LayerComp&);
  LayerComp& operator=(const LayerComp&);
  virtual ~LayerComp() { }  ///< Simple destructor

  virtual double intersectDist(const size_t,const long int,
			       const Geometry::Vec3D&,
			       const Geometry::Vec3D&) const;

  ///\cond ABSTRACT
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const =0;
  virtual HeadRule getLayerHR(const size_t,const long int) const =0;
  ///\endcond ABSTRACT

  /// Access to common divider surface
  virtual HeadRule getCommonSurf(const long int) const { return HeadRule(); }

  /// Access to number of layers:
  virtual size_t getNLayers(const long int =0) const 
     { return nLayers; }
  /// Access to number of layers [on a given side]:
  virtual size_t getNInnerLayers(const long int =1) const 
     { return nInnerLayers; }

};

}

#endif
 
