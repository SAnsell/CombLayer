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

  /*!
  \class PreModWing
  \author S. Ansell / K. Batkov
  \version 2.0
  \date August 2016
  \brief Premoderator wing :fills space below pre-mod and H2 start.
*/

class PreModWing : public attachSystem::ContainedComp,
    public attachSystem::FixedComp,
    public attachSystem::CellMap
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  int engActive;                  ///< engineering active flag
  int tiltSign;                   ///< Z-sign of tilt
  
  double thick;                   ///< (water) thickness
  double wallThick;               ///< wall thickness

  double innerRadius;            ///< Start from inner shape
  double innerYCut;              ///< Start from inner shape

  double tiltAngle;               ///< tilt angle
  double tiltRadius;              ///< radius where tilting starts

  int mat;                        ///< (water) material
  int wallMat;                    ///< wall material

  HeadRule baseSurf;              ///< Top cut surface
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,const bool);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PreModWing(const std::string&);
  PreModWing(const PreModWing&);
  PreModWing& operator=(const PreModWing&);
  virtual PreModWing* clone() const;
  virtual ~PreModWing();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const bool,const int);

};

}

#endif
 
