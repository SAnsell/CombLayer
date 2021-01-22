/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Scrapper.h
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
#ifndef tdcSystem_Scrapper_h
#define tdcSystem_Scrapper_h

class Simulation;


namespace tdcSystem
{
/*!
  \class Scrapper
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Scrapper for Max-IV
*/

class Scrapper :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]
  double wallThick;             ///< pipe thickness

  double flangeRadius;          ///< Joining Flange radius
  double flangeLength;          ///< Joining Flange length

  double tubeAYStep;            ///< Tube Y Step (A)
  double tubeBYStep;            ///< Tube Y Step (B)
  
  double tubeRadius;            ///< Radius of tube
  double tubeLength;            ///< Length/height of tube
  double tubeThick;             ///< Tube thickness
  double tubeFlangeRadius;      ///< Top flange of tube
  double tubeFlangeLength;      ///< length of tube
  
  double scrapperRadius;         ///< Radius of tube
  double scrapperHeight;         ///< Radius of tube
  double scrapperZLift;          ///< Radius of tube

  double driveRadius;           ///< Radius of drive
  double driveFlangeRadius;     ///< Radius of drive flange
  double driveFlangeLength;     ///< Length of drive flange
  double supportRadius;         ///< Radius of drive support
  double supportThick;          ///< Thickness of support wall
  double supportHeight;         ///< Height of support

  double topBoxWidth;           ///< top box width
  double topBoxHeight;          ///< top box height
  
  int voidMat;                  ///< void material
  int tubeMat;                  ///< main tube material
  int flangeMat;                ///< flange material
  int scrapperMat;               ///< scrapper material
  int driveMat;                 ///< drive pipe material
  int topMat;                   ///< top control material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Scrapper(const std::string&);
  Scrapper(const std::string&,const std::string&);
  Scrapper(const Scrapper&);
  Scrapper& operator=(const Scrapper&);
  virtual ~Scrapper();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
