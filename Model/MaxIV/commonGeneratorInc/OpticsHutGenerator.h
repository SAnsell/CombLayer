/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/OpticsHutGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_OpticsHutGenerator_h
#define setVariable_OpticsHutGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class OpticsHutGenerator
  \version 1.0
  \author S. Ansell
  \date July 2021
  \brief Standard (exept length/chicane) optics hutch for Max IV
*/

class OpticsHutGenerator 
{
 protected:

  double height;                ///< void height
  double outWidth;              ///< Width from beamline centre to outside
  
  // walls
  double innerThick;            ///< Inner wall/roof skin
  double pbWallThick;           ///< Thickness of lead in walls
  double pbBackThick;           ///< Thickness of lead in back plate
  double pbRoofThick;           ///< Thickness of lead in Roof
  double outerThick;            ///< Outer wall/roof skin
  
  double backPlateThick;        ///< Back plate thick
  double backPlateWidth;        ///< Back plate width
  double backPlateHeight;       ///< back plate height

  double innerOutVoid;          ///< Extension for inner left void space
  double outerOutVoid;          ///< Extension for outer left void space
  double frontVoid;             ///< Extension for inner front void space
  double backVoid;              ///< Extension for inner back void space
  double outerBackVoid;         ///< Extension for outer back void space

  std::vector<Geometry::Vec3D> holeOffset;  ///< hole offsets [y ignored]
  std::vector<double> holeRadius;           ///< hole radii
  
  std::string skinMat;         ///< Fe layer material for walls
  std::string pbMat;           ///< pb layer material for walls
  std::string voidMat;         ///< Void material

 public:

  OpticsHutGenerator();
  OpticsHutGenerator(const OpticsHutGenerator&) =default;
  OpticsHutGenerator& operator=(const OpticsHutGenerator&) =default;
  ~OpticsHutGenerator() =default;

  // set skinthickness
  void setSkin(const double T) { outerThick=T; innerThick=T;}

  void setBackLead(const double T) { pbBackThick=T; }
  void setRoofLead(const double T) { pbRoofThick=T; }
  void setWallLead(const double T) { pbWallThick=T; }
  void setFrontExt(const double T) { frontVoid=T; }
  void setBackExt(const double T) { backVoid=T; }
  void setOuterBackExt(const double T) { outerBackVoid=T; }

  /// Set back plate [must be smaller than back wall]
  void setBackPlateSize(const double W,const double H,const double T)
  { backPlateWidth=W;backPlateHeight=H;backPlateThick=T; }

  void addHole(const Geometry::Vec3D&,const double);
  
  void generateHut(FuncDataBase&,const std::string&,
		   const double) const;

};

}

#endif
 
