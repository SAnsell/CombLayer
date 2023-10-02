/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BulkModule.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef essSystem_BulkModule_h
#define essSystem_BulkModule_h

class Simulation;

namespace essSystem
{

/*!
  \class BulkModule
  \version 1.0
  \author S. Ansell
  \date April 2013
  \brief Bulk around Reflector
*/

class BulkModule :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:
     
  size_t nLayer;                 ///< Number of layers
  std::vector<double> radius;        ///< radius of outer
  std::vector<double> height;        ///< Heights
  std::vector<double> depth;         ///< Depths

  std::vector<Geometry::Vec3D> COffset;   ///< Centre offset
  std::vector<int> Mat;             ///< Default materials

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  
 public:

  BulkModule(const std::string&);
  BulkModule(const BulkModule&);
  BulkModule& operator=(const BulkModule&);
  ~BulkModule() override;



  void addFlightUnit(Simulation&,const attachSystem::FixedComp&);
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 

