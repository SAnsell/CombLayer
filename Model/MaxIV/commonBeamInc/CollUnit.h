/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/CollUnit.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_CollUnit_h
#define xraySystem_CollUnit_h

class Simulation;

namespace xraySystem
{

/*!
  \class CollUnit
  \version 2.0
  \author Stuart Ansell
  \date October 2021
  \brief Cooled Beam Viewer screen
*/

class CollUnit :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:

  double tubeRadius;            ///< radius of tubing
  double tubeWidth;             ///< x-Cross tube-tube gap
  double tubeYStep;             ///< y-forward step
  double tubeWall;              ///< wall thickness of tubing

  double plateThick;            ///< thickness of plate
  double plateLength;           ///< Length of plate
  double plateWidth;            ///< Width of plate

  size_t nHoles;                ///< number of holes
  double holeGap;               ///< Gap between hole
  double holeRadius;            ///< Radius of hols
    
  int voidMat;                  ///< void material
                            
  int screenMat;                ///< screen holder material
  int pipeMat;                  ///< pipe material
  int waterMat;                 ///< water material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CollUnit(const std::string&);
  CollUnit(const CollUnit&);
  CollUnit& operator=(const CollUnit&);
  virtual ~CollUnit();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
