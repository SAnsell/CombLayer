/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/RFQ.h
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
#ifndef essSystem_RFQ_h
#define essSystem_RFQ_h

class Simulation;

namespace essSystem
{

/*!
  \class RFQ
  \version 1.0
  \author Konstantin Batkov
  \date 12 Sep 2018
  \brief Radio-frequency quadrupole
*/

class RFQ : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  double outerWidth;            ///< Outer Width
  double innerWidth;            ///< Inner Width

  double wallThick;             ///< Thickness of wall
  double vaneThick;             ///< Vane thickness
  double vaneLength; ///< Length of vane thick section

  double coolantOuterRadius; ///< Radius of outer coolant pipes
  double coolantOuterDist; ///< Distance from origin of outer coolant pipes
  double coolantInnerRadius; ///< Radius of inner coolant pipes
  double coolantInnerDist; ///< Distance from origin of inner coolant pipes

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material
  int coolantMat; ///< Coolant material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  RFQ(const std::string&);
  RFQ(const RFQ&);
  RFQ& operator=(const RFQ&);
  virtual RFQ* clone() const;
  virtual ~RFQ();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


