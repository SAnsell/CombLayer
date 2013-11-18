/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/ChipIRFilter.h
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
#ifndef hutchSystem_ChipIRFilter_h
#define hutchSystem_ChipIRFilter_h

class Simulation;

namespace hutchSystem
{


/*!
  \class ChipIRFilter
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Filter set for ChipIR
  
*/

class ChipIRFilter :  public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int filterIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< populated or not
 
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< x-y Rotation angle
  double zAngle;                ///< z Rotation angle
  
  double outerLen;              ///< Outer length
  double outerWidth;            ///< Outer Width
  double outerHeight;           ///< Outer Height

  int nLayers;              ///< number of layers
  std::vector<double> fracFrac; ///< guide Layer thicknesss (fractions)
  std::vector<int> fracMat; ///< guide Layer materials

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  ChipIRFilter(const std::string&);
  ChipIRFilter(const ChipIRFilter&);
  ChipIRFilter& operator=(const ChipIRFilter&);
  ~ChipIRFilter();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
