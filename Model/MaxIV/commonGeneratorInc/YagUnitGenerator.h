/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/YagUnitGenerator.h
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
#ifndef setVariable_YagUnitGenerator_h
#define setVariable_YagUnitGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class YagUnitGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief YagUnitGenerator for variables
*/

class YagUnitGenerator 
{
 private:

  double radius;               ///< void radius   
  double height;               ///< void height [+z]
  double depth;                ///< void depth [-z]
  double wallThick;            ///< pipe thickness

  double flangeRadius;         ///< Joining Flange radius 
  double flangeLength;         ///< Joining Flange length
  double plateThick;           ///< flange plate thick

  // centre port [left]
  double viewZStep;            ///< Viewing Z-lift
  double viewRadius;           ///< Viewing Radius
  double viewThick;            ///< Wall thickness
  double viewLength;           ///< viewing Length [centre to flange end]
  double viewFlangeRadius;     ///< View Flange radius
  double viewFlangeLength;     ///< Joining Flange length
  double viewPlateThick;       ///< Cover plate thickness

  // front/back port 
  double portRadius;         ///< port Radius
  double portThick;          ///< port wall thickness
  double portFlangeRadius;   ///< port flange Radius
  double portFlangeLength;   ///< port flange length
  double frontLength;        ///< front Length [centre to flange end]
  double frontILen;          ///< front Length [centre to start]
  double backLength;         ///< back Length [centre to flange end]
  double backILen;           ///< front Length [centre to start]

  double outerRadius;        ///< Radius to build simple tower void

  std::string voidMat;               ///< void material
  std::string mainMat;               ///< wall material
  
 public:

  YagUnitGenerator();
  YagUnitGenerator(const YagUnitGenerator&);
  YagUnitGenerator& operator=(const YagUnitGenerator&);
  virtual ~YagUnitGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();
  template<typename T> void setPortCF();
  template<typename T> void setViewCF();
  
  void generateYagUnit(FuncDataBase&,
		       const std::string&) const;

};

}

#endif
 
