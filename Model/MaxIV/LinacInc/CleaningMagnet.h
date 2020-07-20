/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/CleaningMagnet.h
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
#ifndef tdcSystem_CleaningMagnet_h
#define tdcSystem_CleaningMagnet_h

class Simulation;

namespace tdcSystem
{

/*!
  \class CleaningMagnet
  \version 1.0
  \author Konstantin Batkov
  \date July 2020
  \brief Cleaning Magnet
*/

class CleaningMagnet :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:

  double length;             ///< Magnet core length
  double width;              ///< Magnet core width
  double height;             ///< Magnet core height
  double gap;                ///< Void gap between cores

  double yokeLength;            ///< Yoke length
  double yokeDepth;             ///< Yoke depth
  double yokeThick;             ///< Yoke thick

  int mat;                   ///< Core aterial
  int yokeMat;               ///< Yoke material
  int voidMat;                  ///< Void material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CleaningMagnet(const std::string&);
  CleaningMagnet(const CleaningMagnet&);
  CleaningMagnet& operator=(const CleaningMagnet&);
  virtual CleaningMagnet* clone() const;
  virtual ~CleaningMagnet();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
