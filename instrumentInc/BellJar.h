/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   instrumentInc/BellJar.h
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
#ifndef instrumentSystem_BellJar_h
#define instrumentSystem_BellJar_h

class Simulation;

namespace d4cSystem
{

/*!
  \class BellJar
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder object
*/

class BellJar : 
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap
{
 private:
  
  double radius;                ///< Radius of bell jar
  double wallThick;             ///< Outer wall thickness
  double height;                ///< Height/depth
  int wallMat;                  ///< Wall material

  double colRadius;             ///< Centre line of collimator block
  double colWidth;              ///< Width of the diamond block
  double colFront;              ///< Front distance
  double colBack;               ///< Back distance

  double subColWidth;              ///< Width of the diamond block
  double subColFront;              ///< Front distance
  double subColBack;               ///< Back distance

  int colMat;                   ///< Material

  std::vector<double> colAngle;  ///< angles 

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BellJar(const std::string&);
  BellJar(const BellJar&);
  BellJar& operator=(const BellJar&);
  ~BellJar() override;


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
