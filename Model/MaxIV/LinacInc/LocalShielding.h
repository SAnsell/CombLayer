/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/LocalShielding.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef tdcSystem_LocalShielding_h
#define tdcSystem_LocalShielding_h

class Simulation;

namespace tdcSystem
{

/*!
  \class LocalShielding
  \version 1.0
  \author Konstantin Batkov
  \date February 2021
  \brief LocalShielding
*/

class LocalShielding :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 protected:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double depth;                 ///< Depth
  double midHoleWidth;          ///< Width of the beam pipe penetration
  double midHoleHeight;         ///< Height of the beam pipe penetration
  double cornerWidth;           ///< Corner cut width
  double cornerHeight;          ///< Corner cut height
  int mainMat;                  ///< Main material
  std::string cType;            ///< Corner type [left,right,default="both"]
  std::string opt;              ///< Option [sideOnly,default=""]

  void populate(const FuncDataBase&) override;

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  LocalShielding(const std::string&);
  LocalShielding(const LocalShielding&);
  LocalShielding& operator=(const LocalShielding&);
  virtual LocalShielding* clone() const;
  ~LocalShielding() override;

  void addUnit(const std::string&);
  void addConnection(const std::string&,const std::string&,const std::string&);
  void addSurf(const std::string&,const std::string&,const std::string&);


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int) override;

};

}

#endif
