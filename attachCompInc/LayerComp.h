/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/LayerComp.h
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
  
 public:

  LayerComp(size_t NL) : nLayers(NL) {}
  /// Basic copy constructor
  LayerComp(const LayerComp& A) : nLayers(A.nLayers) {}
  /// Basic assignment operator
  LayerComp& operator=(const LayerComp& A)  
    { nLayers=A.nLayers; return *this; }
  virtual ~LayerComp() { }  ///< Simple destructor


  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const =0;
  virtual int getLayerSurf(const size_t,const size_t) const =0;
  virtual std::string getLayerString(const size_t,const size_t) const =0;
  /// Access to common divider surface
  virtual int getCommonSurf(const size_t) const { return 0; }

  /// Access to number of layers:
  size_t getNLayers() const { return nLayers; }

};

}

#endif
 
