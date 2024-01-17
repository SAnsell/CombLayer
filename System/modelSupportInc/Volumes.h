/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupportInc/Volumes.h
*
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef ModelSupport_Volumes_h
#define ModelSupport_Volumes_h

namespace ModelSupport
{
  class VolSum;
  
  void calcVolumes(Simulation*,const mainSystem::inputParam&);
  void generatePlot(const std::string&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&,
		    const size_t,
		    const size_t,
		    const std::vector<Geometry::Vec3D>&,
		    const std::vector<Geometry::Vec3D>&);

  void materialCheck(const Simulation&,const mainSystem::inputParam&);
  void populateCells(const Simulation&,const mainSystem::inputParam&,
		     VolSum&);

}


#endif
