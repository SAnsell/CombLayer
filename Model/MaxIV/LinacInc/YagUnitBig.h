/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/YagUnitBig.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell / Konstantin Batkov
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
#ifndef tdcSystem_YagUnitBig_h
#define tdcSystem_YagUnitBig_h

class Simulation;


namespace tdcSystem
{
/*!
  \class YagUnitBig
  \version 1.0
  \author S. Ansell / K. Batkov
  \date June 2020

  \brief YagUnit big type for Max-IV
*/

class YagUnitBig :
  public attachSystem::FixedRotate,
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
  double viewAZStep;            ///< Step from origin
  double viewARadius;           ///< Viewing Radius
  double viewAThick;            ///< Wall thickness
  double viewALength;           ///< viewing Length [centre to flange end]
  double viewAFlangeRadius;     ///< Flange outer Radius
  double viewAFlangeLength;     ///< Flange length
  double viewAPlateThick;       ///< View Flange Plate thickness

  // side view port [back]
  double viewBYStep;            ///< Step from origin
  double viewBRadius;           ///< Viewing Radius
  double viewBThick;            ///< Wall thickness
  double viewBLength;           ///< viewing Length [centre to flange end]
  double viewBFlangeRadius;     ///< Flange outer Radius
  double viewBFlangeLength;     ///< Flange length
  double viewBPlateThick;       ///< View Flange Plate thickness

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

  YagUnitBig(const std::string&);
  YagUnitBig(const std::string&,const std::string&);
  YagUnitBig(const YagUnitBig&);
  YagUnitBig& operator=(const YagUnitBig&);
  virtual ~YagUnitBig();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
