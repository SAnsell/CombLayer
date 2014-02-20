/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muonInc/DiscTarget.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef bnctSystem_DiscTarget_h
#define bnctSystem_DiscTarget_h

class Simulation;

namespace bnctSystem
{

/*!
  \class DiscTarget
  \author S. Ansell
  \version 1.0
  \date January
  \brief Thin Be/Layer target
*/

class DiscTarget : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int discIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step
  double xyAngle;               ///< XY angle
  double zAngle;                ///< Z Angle

  size_t NLayers;                  ///< Number of layers
  std::vector<double> depth;      ///< Depth
  std::vector<double> radius;     ///< Radius
  std::vector<int> mat;           ///< Material  

  double maxRad;                  ///< MAx radius
  size_t maxIndex;                ///< Maximum index 

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiscTarget(const std::string&);
  DiscTarget(const DiscTarget&);
  DiscTarget& operator=(const DiscTarget&);
  virtual ~DiscTarget();

  void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
