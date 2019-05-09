/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/FilterHolder.h
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#ifndef xraySystem_FilterHolder_h
#define xraySystem_FilterHolder_h

class Simulation;

namespace xraySystem
{

/*!
  \class FilterHolder
  \version 1.0
  \author Konstantin Batkov
  \date 8 May 2019
  \brief Filter holder
*/

class FilterHolder : public attachSystem::ContainedComp,
		    public attachSystem::FixedOffset,
		    public attachSystem::CellMap
{
 private:

  double thick;                ///< Thickness
  double width;                 ///< Width
  double height;                ///< Height
  double depth; ///< Depth

  double legWidth; ///< Leg width
  double legHeight; ///< Leg height
  double baseWidth; ///< Base width
  double baseHeight; ///< Base height

  double foilThick; ///< Foil thickness
  int foilMat; ///< Foil material

  int nWindows; ///< Number of windows
  double wWidth; ///< Window width
  double wHeight; ///< Window height
  double wDepth; ///< Window depth

  int mat;                   ///< Main material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FilterHolder(const std::string&);
  FilterHolder(const FilterHolder&);
  FilterHolder& operator=(const FilterHolder&);
  virtual FilterHolder* clone() const;
  virtual ~FilterHolder();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


