/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/YagScreenGenerator.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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

class YagScreenGenerator :
    public ScreenGenerator
{
 private:

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


  std::string holderMat;             ///< mirror holder material
  std::string screenMat;             ///< mirror holder material
  std::string screenHolderMat;       ///< mirror holder material
  std::string mirrorMat;             ///< mirror  material

 public:

  YagScreenGenerator();
  YagScreenGenerator(const YagScreenGenerator&);
  YagScreenGenerator& operator=(const YagScreenGenerator&);
  virtual ~YagScreenGenerator();


  virtual void generateScreen(FuncDataBase&,const std::string&,
			      const bool) const;
  
};

}

#endif
