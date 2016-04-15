/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/PreModWing.h
 *
 * Copyright (c) 2015 by Konstantin Batkov
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
#ifndef essSystem_PreModWing_h
#define essSystem_PreModWing_h

class Simulation;

namespace essSystem
{
  class CylFlowGuide;
/*!
  \class PreModWing
  \author Konstantin Batkov
  \version 1.0
  \date Aug 2015
  \brief Premoderator wing - used for flight line tilting when the flight line height is smaller than the Butterfly moderator height.
         Inserted into the flight line.
*/

class PreModWing : public attachSystem::ContainedComp,
    public attachSystem::FixedComp,
    public attachSystem::CellMap
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  int engActive;                  ///< engineering active flag

  double thick;                   ///< (water) thickness
  int mat;                        ///< (water) material
  double wallThick;               ///< wall thickness
  int wallMat;                    ///< wall material

  bool   tiltSide;                ///< true ? top : bottom  side to be tilted
  double tiltAngle;               ///< tilt angle
  double tiltRadius;              ///< radius where tilting starts
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&, const long int, const bool);

  void createSurfaces();
  void createObjects(Simulation&, const attachSystem::FixedComp&, const long int,
		     const attachSystem::FixedComp&);
  void createLinks();

 public:

  PreModWing(const std::string&);
  PreModWing(const PreModWing&);
  PreModWing& operator=(const PreModWing&);
  virtual PreModWing* clone() const;
  virtual ~PreModWing();

  void createAll(Simulation&,const attachSystem::FixedComp&, const long int, const bool,
		 const bool,
		 const attachSystem::FixedComp&);
};

}

#endif
 
