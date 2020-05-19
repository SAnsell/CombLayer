/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Scraper.h
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
#ifndef tdcSystem_Scraper_h
#define tdcSystem_Scraper_h

class Simulation;


namespace tdcSystem
{
/*!
  \class Scraper
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief Scraper for Max-IV
*/

class Scraper :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]
  double outerThick;            ///< pipe thickness

  double flangeRadius;          ///< Joining Flange radius
  double flangeLength;          ///< Joining Flange length

  double tubeAYStep;            ///< Tube Y Step (A)
  double tubeBYStep;            ///< Tube Y Step (B)
  
  double tubeRadius;            ///< Radius of tube
  double tubeLength;            ///< Length/height of tube
  double tubeThick;             ///< Tube thickness
  double tubeFlangeRadius;      ///< Top flange of tube
  double tubeFlangeLength;      ///< length of tube
  
  double scraperRadius;         ///< Radius of tube
  double scraperHeight;         ///< Radius of tube
  double scraperZLift;          ///< Radius of tube

  double driveRadius;           ///< Radius of drive 

  double topBoxWidth;           ///< top box width
  double topBoxHeight;          ///< top box height
  
  int voidMat;                  ///< void material
  int tubeMat;                  ///< main tube material
  int flangeMat;                ///< flange material
  int scraperMat;               ///< scraper material
  int driveMat;                 ///< drive pipe material
  int topMat;                   ///< top control material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Scraper(const std::string&);
  Scraper(const std::string&,const std::string&);
  Scraper(const Scraper&);
  Scraper& operator=(const Scraper&);
  virtual ~Scraper();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
