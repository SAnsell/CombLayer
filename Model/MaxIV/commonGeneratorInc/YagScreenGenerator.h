/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/YagScreenGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_YagScreenGenerator_h
#define setVariable_YagScreenGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class YagScreenGenerator
  \version 1.0
  \author Konstantin Batkov
  \date May 2020
  \brief YagScreenGenerator for variables
*/

class YagScreenGenerator
{
 private:

  double juncBoxLength;         ///< electronics junction box length
  double juncBoxWidth;          ///< electronics junction box width
  double juncBoxHeight;         ///< electronics junction box height
  double juncBoxWallThick;      ///< electronics junction box wall thickness


  double feedLength;            ///< linear pneumatics feedthrough length
  double feedInnerRadius;       ///< linear pneumatics feedthrough inner radius
  double feedWallThick;         ///< linear pneumatics feedthrough wall thickness
  double feedFlangeLen;         ///< linear pneumatics feedthrough flange length
  double feedFlangeRadius;      ///< linear pneumatics feedthrough flange radius

  double threadLift;            ///< screen thread lift
  double threadRadius;          ///< screen thread inner radius

  // volume containing Yag screen + mirror

  double holderWidth;           ///< Flat across holder
  double holderDepth;           ///< In beamaxis
  double holderShortLen;        ///< short length of trapizium
  double holderLongLen;         ///< long length

  double mirrorAngle;           ///< Flat (mirror) angle
  double mirrorRadius;          ///< quartz mirror radius
  double mirrorThick;           ///< quartz mirror thickness

  double screenAngle;           ///< screen inclination angle [deg]
  double screenVOffset;         ///< Start of screen join above mirror cut
  double screenRadius;          ///< Radius of screen
  double screenThick;           ///< Thickness of screen

  double screenHolderRadius;     ///< screen holder thickness
  double screenHolderThick;     ///< screen holder thickness


  std::string voidMat;               ///< void material

  /// electronics junction box cable/inside material
  std::string juncBoxMat;                
  std::string juncBoxWallMat;        ///< electronics junction box wall material
  std::string threadMat;             ///< screen thread material
  std::string mirrorMat;             ///< mirror  material
  std::string screenMat;             ///< screen  material
  
  std::string holderMat;       ///< screen holder material

  std::string feedWallMat;           ///< Feedthrough wall material  


 public:

  YagScreenGenerator();
  YagScreenGenerator(const YagScreenGenerator&);
  YagScreenGenerator& operator=(const YagScreenGenerator&);
  ~YagScreenGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setFlangeCF();

  void generateScreen(FuncDataBase&,const std::string&,
		      const bool) const;

};

}

#endif
