/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/QuadUnit.h
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
#ifndef xraySystem_QuadUnit_h
#define xraySystem_QuadUnit_h

class Simulation;


namespace xraySystem
{
  class Quadrupole;
  
/*!
  \class QuadUnit
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief QuadUnit for Max-IV 

  This is built relative to the proton channel
*/

class QuadUnit : public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double length;                ///< frame length
  double inWidth;               ///< Inner width [flats]
  double ringWidth;             ///< Inner width [to divide not point]
  double outPointWidth;         ///< Outer [wall side] half-width
  double height;                ///< Inner height [straight]

  double endGap;                ///< Inner width
  double endLength;            ///< Inner height [straight]

  double wallThick;             ///< Wall thickness
    
  double flangeARadius;          ///< Joining Flange radius
  double flangeBRadius;          ///< Joining Flange radius 
  double flangeALength;          ///< Joining Flange length
  double flangeBLength;          ///< Joining Flange length

  /// X Direction quad
  std::shared_ptr<xraySystem::Quadrupole> quadX;
  /// Z Direction quad
  std::shared_ptr<xraySystem::Quadrupole> quadZ;
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  int flangeMat;                ///< flange material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  QuadUnit(const std::string&);
  QuadUnit(const QuadUnit&);
  QuadUnit& operator=(const QuadUnit&);
  virtual ~QuadUnit();

  void createQuads(Simulation&,const int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
