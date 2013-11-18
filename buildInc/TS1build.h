/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/TS1build.h
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
#ifndef TS1build_h
#define TS1build_h

/*!
  \namespace TS1Build
  \version 1.0
  \author S. Ansell
  \date August 2009
  \brief Stuff to build TS1
*/


namespace TS1Build
{
  void registerPlane(std::set<int>&,const double,
		     const int,const Geometry::Vec3D&,const Geometry::Vec3D&);

  std::string createBox(const std::set<int>&,
			const std::vector<int>&,const std::vector<int>&);

  void addMethaneFlightLines(Simulation&);

}


#endif
