/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/PrismaChamberTube.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_PrismaChamber_h
#define tdcSystem_PrismaChamber_h

class Simulation;


namespace tdcSystem
{

  /*!
  \class PrismaChamber
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief PrismaChamber for Max-IV
*/

class PrismaChamber :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius
  double beamRadius;            ///< radius of Y (Beam) port
  double sideRadius;            ///< radiu of side ports

  double wallThick;             ///< pipe thickness

  double height;                ///< full to flange height above beam
  double depth;                 ///< full to flange depth below beam

  double beamXStep;             ///< beam offset [x directoin
  double frontLength;           ///< full to flange length (-ve Y)
  double backLength;            ///< full to flange length
  double sideLength;            ///< full to flange length

  double flangeRadius;          ///< Top flange radius
  double flangeXRadius;         ///< flange cross direction
  double flangeBeamRadius;      ///< Joining Flange beam direct

  double flangeLength;          ///< Top flange radius
  double flangeXLength;         ///< flange cross direction
  double flangeBeamLength;      ///< Joining Flange beam direct

  double sideThick;             ///< Side thickness
  double plateThick;            ///< Joining Flange radius

  int voidMat;                  ///< void material
  int wallMat;                  ///< main material
  int plateMat;                 ///< plate material
 
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PrismaChamber(const std::string&);
  PrismaChamber(const std::string&,const std::string&);
  PrismaChamber(const PrismaChamber&);
  PrismaChamber& operator=(const PrismaChamber&);
  virtual ~PrismaChamber();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
