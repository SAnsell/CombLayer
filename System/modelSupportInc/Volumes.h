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

template<typename T> class multiData;

namespace ModelSupport
{
  class VolSum;
  
  void calcVolumes(Simulation*,const mainSystem::inputParam&);
  void readPts(const std::string&,
	       const Geometry::Vec3D&,
	       const Geometry::Vec3D&,
	       const Geometry::Vec3D&,
	       const Geometry::Vec3D&,
	       std::vector<int>&,
	       std::vector<Geometry::Vec3D>&,
	       std::vector<Geometry::Vec3D>&);
	       
  void readHeat(const std::string&,
		std::vector<double>&,
		std::vector<double>&,
		std::vector<double>&,
		multiData<double>&);
  
  
  void generatePlot(const std::string&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&,
		    const Geometry::Vec3D&,
		    const size_t,
		    const size_t,
		    const std::vector<Geometry::Vec3D>&,
		    const std::vector<Geometry::Vec3D>&);
  std::tuple<size_t,size_t,size_t,size_t>
  computeIndex(const std::vector<double>&,
	       const std::vector<double>&,
	       const std::vector<double>&,
	       const Geometry::Vec3D&);
  void materialHeat(const Simulation&,const mainSystem::inputParam&);
  void materialCheck(const Simulation&,const mainSystem::inputParam&);
  void populateCells(const Simulation&,const mainSystem::inputParam&,
		     VolSum&);

}


#endif
