/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/HeShield.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef photonSystem_HeShield_h
#define photonSystem_HeShield_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class RingFlange;
}

namespace photonSystem
{
  
/*!
  \class HeShield
  \author S. Ansell
  \version 1.0
  \date October 2016
  \brief Specialized for shield with vertical He3 tubes
*/

class HeShield : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int heIndex;         ///< Index of surface offset
  int cellIndex;             ///< Cell index

  double length;            ///< length of poly 
  double width;             ///< width of poly 
  double height;            ///< height of poly shield

  double frontPolyThick;    ///< Back CD thick

  double collLen;           ///< Cd length of viewport 
  double collWidth;         ///< Cd width [view port]
  double collHeight;        ///< Cd height [view port]
  double collThick;         ///< Cd thickness 

  double cdThick;           ///< Back CD thick

  int polyMat;              ///< outer wall material
  int cdMat;                ///< main decoupler material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:

  HeShield(const std::string&);
  HeShield(const HeShield&);
  HeShield& operator=(const HeShield&);
  virtual ~HeShield();
  virtual HeShield* clone() const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
