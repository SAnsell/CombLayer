/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsTubeNoseCone.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef xraySystem_cosaxsTubeNoseCone_h
#define xraySystem_cosaxsTubeNoseCone_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsTubeNoseCone
  \version 1.0
  \author Konstantin Batkov
  \date 17 May 2019
  \brief cosaxsTubeNoseCone
*/

class cosaxsTubeNoseCone :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                 ///< Length of the pyramid-shaped segment

  double frontPlateWidth;        ///< Back plate width
  double frontPlateHeight;       ///< Back plate height
  double frontPlateThick;        ///< Back plate thickness
  
  double backPlateWidth;         ///< Front plate width
  double backPlateHeight;        ///< Front plate height
  double backPlateThick;         ///< Front plate thickness
  double backPlateRimThick;      ///< Front plate rim thickness
  
  double flangeRadius; ///< flange radius
  double flangeLength; ///< Flange length

  double pipeLength; ///< Pipe (+flange) length
  double pipeRadius; ///< Pipe inner radius
  double pipeWallThick; ///< Pipe wall thickness

  double wallThick;                ///< Thickness of inclined walls
  double windowRadius;             ///< Extra window radius 
  double windowThick;              ///< Thickness of nosecone window
  
  int wallMat;                  ///< Wall material
  int windowMat;                  ///< Wall material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsTubeNoseCone(const std::string&);
  cosaxsTubeNoseCone(const cosaxsTubeNoseCone&);
  cosaxsTubeNoseCone& operator=(const cosaxsTubeNoseCone&);
  virtual cosaxsTubeNoseCone* clone() const;
  virtual ~cosaxsTubeNoseCone();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


