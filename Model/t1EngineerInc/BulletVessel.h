/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1EngineeringInc/BulletVessel.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Goran Skoro
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
#ifndef ts1System_BulletVessel_h
#define ts1System_BulletVessel_h

class Simulation;

namespace ts1System
{

/*!
  \class BulletVessel
  \version 1.0
  \author S. Ansell/G. Skoro
  \date April 2011
  \brief BulletVessel [insert object]
*/

class BulletVessel : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int pvIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double frontClear;            ///< Clearance on front face
  double frontThick;            ///< Front wall thickness   
  double frontWaterThick;       ///< Front water thickness

  double mainRadius;            ///< Central cylinder
  
  std::vector<double> radii;    ///< Cone/Cylinder radii
  std::vector<double> length;   ///< Cone/Cylinder length

  double waterHeight;           ///< total water gap height
  double taVertThick;           ///< Extra thickness for Ta layer vertical
  double taRadialThick;         ///< Radial thickness of Ta

  double clearVertThick;          ///< vertical thickness of clearance
  double clearRadialThick;         ///< Radial thickness of clearance

  int waterMat;                 ///< Water [D2O]
  int wallMat;                  ///< Material for walls


  std::vector<int> innerCells;  ///< Inner water cells
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  BulletVessel(const std::string&);
  BulletVessel(const BulletVessel&);
  BulletVessel& operator=(const BulletVessel&);
  ~BulletVessel();

  /// accessor to inner water cells
  const std::vector<int>& getInnerCells() const
    { return innerCells; }
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
