/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/PreDipole.h
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
#ifndef xraySystem_PreDipole_h
#define xraySystem_PreDipole_h

class Simulation;


namespace xraySystem
{
/*!
  \class PreDipole
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief PreDipole for Max-IV 

  This is built relative to the proton channel
*/

class PreDipole : public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double length;                ///< frame length
  double width;                 ///< Inner width
  double height;                ///< Inner height [straight]
  double angle;                 ///< Angle
  
  double flangeRadius;          ///< Joining Flange radius 
  double flangeLength;          ///< Joining Flange length
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  int flangeMat;                ///< flange material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PreDipole(const std::string&);
  PreDipole(const PreDipole&);
  PreDipole& operator=(const PreDipole&);
  virtual ~PreDipole();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
