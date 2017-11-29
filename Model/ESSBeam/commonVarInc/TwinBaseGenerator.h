/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/TwinBaseGenerator.h
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
#ifndef essSystem_TwinBaseGenerator_h
#define essSystem_TwinBaseGenerator_h

class Simulation;

namespace setVariable
{

/*!
  \class TwinBaseGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief TwinBaseGenerator for variables
*/

class TwinBaseGenerator
{
 protected:

  double stepHeight;       ///< Height to move main cylinder apart
  double mainRadius;       ///< outer radius of disks
  double innerRadius;      ///< radius of disk voids
  double innerTopStep;     ///< Step above centre for top disk
  double innerLowStep;     ///< Step below centre for lower disk

  double motorRadius;   ///< Flange Inner radius
  double motorFlangeInner;   ///< Flange Inner radius
  double motorFlangeOuter;   ///< Flange Outer radius
  double motorOuter;         ///< Motor outer
  double motorLength;        ///< Motor body length
  size_t motorNBolt;         ///< Number of bolts
  double motorBoltRadius;    ///< Radius of bolts
  double motorSealThick;     ///< Seal thickness
  bool motorRevFlagA;         ///< Motor rotated A
  bool motorRevFlagB;         ///< Motor rotated B
  std::string motorSealMat;  ///< Seal material

  size_t ringNBolt;         ///< Bolts in outer ring
  size_t lineNBolt;         ///< Bolts in outer line
  double outerStep;         ///< Bolts step from outer edge
  double outerBoltRadius;   ///< Radius of outer bolts
  std::string outerBoltMat;     ///< Main wall material
  
  std::string wallMat;     ///< Main wall material
  
 public:

  TwinBaseGenerator();
  TwinBaseGenerator(const TwinBaseGenerator&);
  TwinBaseGenerator& operator=(const TwinBaseGenerator&);
  ~TwinBaseGenerator();


  void setWallMaterial(const std::string&);
  void setFrame(const double,const double);  
  void setMainRadius(const double);
  void setMotorLength(const double);
  void setReverseMotors(const bool,const bool); 


  void generateChopper(FuncDataBase&,
		       const std::string&,
		       const double,const double,
		       const double);
};

}

#endif
 
