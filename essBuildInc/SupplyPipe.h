/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/SupplyPipe.h
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
#ifndef essSystem_SupplyPipe_h
#define essSystem_SupplyPipe_h

class Simulation;

namespace essSystem
{

/*!
  \class SupplyPipe
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief SupplyPipe [insert object]
*/

class SupplyPipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  ModelSupport::PipeLine Coaxial; ///< Global outer
  
  size_t NSegIn;                      ///< Number of segments  
  std::vector<Geometry::Vec3D> PPts;  ///< Pipe points

  double inRadius;                   ///< H2 material
  double inAlRadius;                 ///< Al material
  double midRadius;                   ///< H2 material
  double midAlRadius;                 ///< Al material

  int inMat;                       ///< inner H2 
  int inAlMat;                       ///< inner Al layer
  int midMat;                       ///< mid vacuum
  int midAlMat;                     ///< mid Al layer
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void insertInlet(Simulation&,const attachSystem::FixedComp&,
		   const size_t);

 public:

  SupplyPipe(const std::string&);
  SupplyPipe(const SupplyPipe&);
  SupplyPipe& operator=(const SupplyPipe&);
  ~SupplyPipe();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const size_t);

};

}

#endif
 
