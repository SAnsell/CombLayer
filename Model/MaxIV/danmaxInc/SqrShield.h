/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/danmaxInc/SqrShield.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_SqrShield_h
#define xraySystem_SqrShield_h

class Simulation;

namespace constructSystem
{
    class PipeTube;
}

namespace xraySystem
{

/*!
  \class SqrShield
  \version 1.0
  \author Stuart Ansell
  \date November 2019
  \brief Danmax lead transfer shield
*/

class SqrShield :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double width;           ///< width of void
  double height;          ///< height of void
  double length;          ///< Length [unused if F/B set]
  double thick;           ///< Lead thickness
  double skinThick;       ///< skin thickness

  int mat;                ///< Main wall materails
  int skinMat;            ///< Skin material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SqrShield(const std::string&);
  SqrShield(const SqrShield&);
  SqrShield& operator=(const SqrShield&);
  virtual ~SqrShield();

  HeadRule getInnerVoid() const;
  
  /// set delay
  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif


