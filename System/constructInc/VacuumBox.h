/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VacuumBox.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef constructSystem_VacuumBox_h
#define constructSystem_VacuumBox_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class VacuumBox
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief VacuumBox unit  
*/

class VacuumBox :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  const bool centreOrigin;      ///< Construct on the centre line
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]

  double feHeight;            ///< fe height [top only]
  double feDepth;             ///< fe depth [low only]
  double feWidth;             ///< fe width [total]
  double feFront;             ///< fe front 
  double feBack;              ///< fe back

  double portAXStep;          ///< XStep of port
  double portAZStep;          ///< ZStep of port
  double portAWallThick;      ///< Flange wall thickness
  double portATubeLength;     ///< Port tube
  double portATubeRadius;     ///< Port tube length


  double portBXStep;          ///< XStep of port
  double portBZStep;          ///< ZStep of port
  double portBWallThick;      ///< Flange wall thickness
  double portBTubeLength;     ///< Port tube
  double portBTubeRadius;     ///< Port tube length

  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length
  
  int voidMat;                ///< void material
  int feMat;                  ///< Fe material layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  VacuumBox(const std::string&,const bool =0);
  VacuumBox(const VacuumBox&);
  VacuumBox& operator=(const VacuumBox&);
  virtual ~VacuumBox();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
