/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/PBW.h
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#ifndef essSystem_PBW_h
#define essSystem_PBW_h

class Simulation;

namespace essSystem
{

/*!
  \class PBW
  \version 1.0
  \author Konstantin Batkov
  \date 26 Jan 2017
  \brief Proton beam window
*/

class PBW : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  int plugNSegments; ///< Number of segments in the vessel

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< height

  double wallThick;             ///< Thickness of wall

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);


 public:

  PBW(const std::string&);
  PBW(const PBW&);
  PBW& operator=(const PBW&);
  virtual PBW* clone() const;
  virtual ~PBW();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int&);

};

}

#endif


