/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/pinInfo
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_pinInfo_h
#define essSystem_pinInfo_h


namespace essSystem
{
  
/*!
  \struct pinInfo
  \version 1.0
  \author S. Ansell
  \date June 2017
  \brief Simple struct for holding point of pin info
*/

struct pinInfo
{
  const std::string Name;         ///< Name of pin
  const size_t radN;              ///< Radial unit
  const size_t sectN;             ///< Sector nubmer
  const size_t index;             ///< Pin index
  

  double length;                  ///< start of pin 
  double boltRadius;              ///< thread size
  double clearRadius;             ///< clearance gap

  Geometry::Vec3D basePoint;      ///< Base point [non z]
  
  pinInfo(const std::string&,const size_t,const size_t,
	  const size_t,const double,const double,
	  const double,const Geometry::Vec3D&);
  pinInfo(const pinInfo&);
  pinInfo& operator=(const pinInfo&);
  ~pinInfo() {}                ///< Destructor
  
};

}

#endif
 

