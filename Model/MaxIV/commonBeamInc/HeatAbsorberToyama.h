/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/HeatAbsorberToyama.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef xraySystem_HeatAbsorberToyama_h
#define xraySystem_HeatAbsorberToyama_h

class Simulation;

namespace xraySystem
{

/*!
  \class HeatAbsorberToyama
  \version 1.0
  \author Konstantin Batkov
  \date February 2025
  \brief Heat absorber in Toyama front-ends
  https://www.toyama-en.com/synchrotron/hhl_ma_features.html
*/

class HeatAbsorberToyama :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double dumpWidth;            ///< dump width
  double dumpHeight;           ///< dump height
  double dumpLength;            ///< Full length of dump
  double dumpEndRadius;         ///< Dump end curvature radius
  double dumpFrontRadius;       ///< Curvature radius at dump front
  double dumpXOffset;           ///< x-offset of the dump penetration
  // beam penetration
  double gapWidth;            ///< Penetration width
  double gapHeight;           ///< Penetration height

  bool closed;           ///< open/closed flag

  int mainMat;                  ///< Main material
  int voidMat;                  ///< Wall material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  HeatAbsorberToyama(const std::string&);
  HeatAbsorberToyama(const HeatAbsorberToyama&);
  HeatAbsorberToyama& operator=(const HeatAbsorberToyama&);
  virtual HeatAbsorberToyama* clone() const;
  virtual ~HeatAbsorberToyama();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
