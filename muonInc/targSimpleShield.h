/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muonInc/targSimpleShield.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_targSimpleShield_h
#define muSystem_targSimpleShield_h

class Simulation;

namespace muSystem
{

/*!
  \class targSimpleShield
  \author G. Skoro
  \version 1.0
  \date October 2013
  \brief mu target shielding - Simplified
*/

class targSimpleShield : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int targShieldIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step
  double xyAngle;               ///< Angle (rotation)

  double height;                ///< Height
  double depth;                 ///< Depth
  double width;                 ///< Width

  double baseThick;                 ///< shield thickness
  double forwThick;                 ///< shield thickness
  double backThick;                 ///< shield thickness
  double muonThick;                 ///< shield thickness
  double japThick;                 ///< shield thickness
  
  int mat;                   ///Material: shield


  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  targSimpleShield(const std::string&);
  targSimpleShield(const targSimpleShield&);
  targSimpleShield& operator=(const targSimpleShield&);
  virtual ~targSimpleShield();
  
  virtual void addToInsertChain(attachSystem::ContainedComp&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&);  
};

}

#endif
 
