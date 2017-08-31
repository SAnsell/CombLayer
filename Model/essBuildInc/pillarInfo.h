/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/pillarInfo
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
#ifndef essSystem_pillarInfo_h
#define essSystem_pillarInfo_h


namespace essSystem
{
  
/*!
  \struct pillarInfo
  \version 1.0
  \author S. Ansell
  \date June 2017
  \brief Simple struct for holding point of pillar info
*/

struct pillarInfo
{
  const std::string Name;         ///< Name of pillar
  const size_t radN;              ///< Radius number
  const size_t sectN;             ///< Sector number
  const int RI;                   ///< Region index
  const int active;               ///< If active
  Geometry::Vec3D centPoint;      ///< Centre point [non z]
  Geometry::Vec3D YAxis;          ///< Y Axis 

  
  pillarInfo(const std::string&,const size_t,const size_t,
	     const int,const int,
	     const Geometry::Vec3D&,const Geometry::Vec3D&);
  pillarInfo(const pillarInfo&);
  pillarInfo& operator=(const pillarInfo&);
  ~pillarInfo() {}                ///< Destructor

  std::string getNext(const long int,const long int) const;
  
};

}

#endif
 

