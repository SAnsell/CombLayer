/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/DTL.h
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#ifndef essSystem_DTL_h
#define essSystem_DTL_h

class Simulation;

namespace essSystem
{

/*!
  \class DTL
  \version 1.0
  \author Konstantin Batkov
  \date 28 Mar 2017
  \brief Drift Tube Linac section
*/

class DTL : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const std::string baseName; ///< Base name (e.g. Linac)
  const std::string extraName; ///< extra name (e.g. DTL)
  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  size_t nLayers;                   ///< number of layers
  std::vector<double> radius;   ///< Radii of each layer
  double height;                ///< height

  double wallThick;             ///< Thickness of wall

  int wallMat;                   ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,const long int);
  void createLinks(const attachSystem::FixedComp&,const long int);

 public:

  DTL(const std::string&,const std::string&,const size_t);
  DTL(const DTL&);
  DTL& operator=(const DTL&);
  virtual DTL* clone() const;
  virtual ~DTL();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


