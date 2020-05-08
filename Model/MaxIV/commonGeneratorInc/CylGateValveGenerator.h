/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CylGateValveGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef setVariable_CylGateValveGenerator_h
#define setVariable_CylGateValveGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CylGateValveGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief CylGateValveGenerator for variables
*/

class CylGateValveGenerator
{
 private:

    double radius;                ///< Radius
  double depth;                 ///< Void down depth
  double height;                ///< void lift height

  double wallThick;             ///< Wall thickness

  double portRadius;            ///< Port inner radius (opening)
  double portFlangeRadius;      ///< Port inner radius (opening)
  double portInner;             ///< Port inner step
  double portThick;             ///< Port outer step

  double topRadius;             ///< Top outer radius
  double topHoleRadius;         ///< Top hole radius
  double topThick;              ///< Top hole radius

  double liftHeight;            ///< Lift tube height
  double liftRadius;            ///< lift tube inner radius
  double liftThick;             ///< lift tube thickness
  double liftPlate;             ///< lift tube plate cover [if no motor]

  double driveRadius;           ///< drive radius
  
  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius
  
  std::string voidMat;                  ///< Void material
  std::string bladeMat;                 ///< blade material
  std::string driveMat;                 ///< blade material
  std::string wallMat;                  ///< Pipe material

 public:

  CylGateValveGenerator();
  CylGateValveGenerator(const CylGateValveGenerator&);
  CylGateValveGenerator& operator=(const CylGateValveGenerator&);
  ~CylGateValveGenerator();

  void generateFlat(FuncDataBase&,const std::string&,
		    const bool,const bool) const;
  void generateGate(FuncDataBase&,const std::string&,
		    const bool) const;

};

}

#endif
 
