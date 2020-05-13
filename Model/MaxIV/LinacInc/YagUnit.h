/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/YagUnit.h
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
#ifndef tdcSystem_YagUnit_h
#define tdcSystem_YagUnit_h

class Simulation;


namespace tdcSystem
{
/*!
  \class YagUnit
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief YagUnit for Max-IV
*/

class YagUnit :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;               ///< void radius   
  double height;               ///< void height [+z]
  double depth;                ///< void depth [-z]
  double wallThick;            ///< pipe thickness

  double flangeRadius;         ///< Joining Flange radius 
  double flangeLength;         ///< Joining Flange length
  double plateThick;           ///< flange plate thick

  // centre port [left]
  double viewRadius;           ///< Viewing Radius
  double viewThick;            ///< Wall thickness
  double viewLength;           ///< viewing Length [centre to flange end]
  double viewFlangeRadius;     ///< Flange outer Radius
  double viewFlangeLength;     ///< Flange length
  double viewPlateThick;       ///< View Flange Plate thickness

  // front/back port 
  double portRadius;         ///< port Radius
  double portThick;          ///< port wall thickness
  double portFlangeRadius;   ///< port Flange radius
  double portFlangeLength;   ///< port flange length

  double frontLength;        ///< front Length [centre to flange end]
  double backLength;         ///< back Length [centre to flange end]

  double outerRadius;        ///< Radius to build simple tower void

  int voidMat;               ///< void material
  int mainMat;               ///< electrode material
 
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  YagUnit(const std::string&);
  YagUnit(const std::string&,const std::string&);
  YagUnit(const YagUnit&);
  YagUnit& operator=(const YagUnit&);
  virtual ~YagUnit();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
