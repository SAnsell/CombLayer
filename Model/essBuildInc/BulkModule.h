/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/BulkModule.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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

class BulkModule : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int bulkIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                  ///< Engineering active flag
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle of master XY rotation
  double zAngle;                ///< Angle of master Z rotation

  size_t nLayer;                 ///< Number of layers
  std::vector<double> radius;        ///< radius of outer
  std::vector<double> height;        ///< Heights
  std::vector<double> depth;         ///< Depths

  std::vector<Geometry::Vec3D> COffset;   ///< Centre offset
  std::vector<int> Mat;             ///< Default materials

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);

 public:

  BulkModule(const std::string&);
  BulkModule(const BulkModule&);
  BulkModule& operator=(const BulkModule&);
  virtual ~BulkModule();

  std::string getComposite(const std::string&) const;

  void addFlightUnit(Simulation&,const attachSystem::FixedComp&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 

