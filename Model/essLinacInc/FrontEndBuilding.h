/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/FrontEndBuilding.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_FrontEndBuilding_h
#define essSystem_FrontEndBuilding_h

class Simulation;

namespace essSystem
{

/*!
  \class FrontEndBuilding
  \version 1.0
  \author Konstantin Batkov
  \date 28 May 2018
  \brief Front end building
*/

class FrontEndBuilding : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double length;                ///< Total length including void
  double widthLeft;             ///< Width left x-
  double widthRight; ///< Width right x+
  double height;                ///< height

  double wallThick;             ///< Thickness of wall

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const long int,
		     const long int,const long int);
  void createLinks();

 public:

  FrontEndBuilding(const std::string&);
  FrontEndBuilding(const FrontEndBuilding&);
  FrontEndBuilding& operator=(const FrontEndBuilding&);
  virtual FrontEndBuilding* clone() const;
  virtual ~FrontEndBuilding();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int,
		 const long int,const long int,
		 const long int,const long int);

};

}

#endif


