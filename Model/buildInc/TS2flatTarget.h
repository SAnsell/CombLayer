/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/TS2flatTarget.h
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
#ifndef TMRSystem_TS2flatTarget_h
#define TMRSystem_TS2flatTarget_h

class Simulation;

namespace TMRSystem
{
/*!
  \class TS2flatTarget
  \version 1.0
  \author S. Ansell
  \date December 2010
  \brief Creates the old flat target
*/

class TS2flatTarget : public attachSystem::FixedComp
{
 private:
  
  const int protonIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index
  int populated;                ///< populated or not

  double yOffset;               ///< Master offset distance 
  
  double mainLength;            ///< Straight length
  double coreRadius;            ///< Inner W radius [cyl]
  double cladRadius;            ///<  Radius of cladding
  double waterRadius;           ///< Thickness of the water
  double pressureRadius;        ///< Thickness of the water

  int wMat;                     ///< Tungsten material
  int taMat;                    ///< Tatalium material
  int waterMat;                 ///< Target coolant material

  double targetTemp;            ///< Target temperature
  double waterTemp;             ///< Water temperature
  double externTemp;            ///< Pressure temperature
  
  void populate(const Simulation&);
  void createUnitVector();
  
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  TS2flatTarget(const std::string&);
  TS2flatTarget(const TS2flatTarget&);
  TS2flatTarget& operator=(const TS2flatTarget&);
  ~TS2flatTarget();

  void createAll(Simulation&);

};

}

#endif
 
