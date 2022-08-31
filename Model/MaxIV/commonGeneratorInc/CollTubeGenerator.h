/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/CollTubeGenerator.h
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
#ifndef setVariable_CollTubeGenerator_h
#define setVariable_CollTubeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CollTubeGenerator
  \version 1.0
  \author S. Ansell
  \date November 2021
  \brief CollTubeGenerator for variables
*/

class CollTubeGenerator 
{
 private:
  
  double mainRadius;            ///< main beam radius (Y)
  double xRadius;               ///< radius of x port 
  double yRadius;               ///< radius of Y (Beam) end ports
  double zRadius;               ///< main veritical radius

  double wallThick;             ///< pipe thickness

  double height;                ///< full to flange height above beam
  double depth;                 ///< full to flange depth below beam

  double frontLength;           ///< full to flange length (-ve Y)
  double backLength;            ///< full to flange length
  
  double flangeXRadius;         ///< Side join flange
  double flangeYRadius;         ///< Beam flange radius
  double flangeZRadius;         ///< Main radius

  double flangeXLength;         ///< Side Flange length
  double flangeYLength;         ///< Beam Flange length
  double flangeZLength;         ///< Main Flange length

  double sideZOffset;           ///< Second Z lift
  double sideLength;           ///< side distance from centre

  double gaugeZOffset;          ///< Gauge Z offset
  double gaugeRadius;           ///< Gauge Z offset
  double gaugeLength;           ///< Gauge Length
  double gaugeHeight;           ///< Gauge Height up
  double gaugeFlangeRadius;     ///< Gauge flange thickness
  double gaugeFlangeLength;     ///< Gauge flange thickness

  double plateThick;            ///< Top/Base plate thick
  
  std::string voidMat;          ///< void material
  std::string wallMat;          ///< main material
  std::string plateMat;         ///< plate material
 
 public:

  CollTubeGenerator();
  CollTubeGenerator(const CollTubeGenerator&);
  CollTubeGenerator& operator=(const CollTubeGenerator&);
  virtual ~CollTubeGenerator();

  template<typename T> void setCF();
  template<typename T> void setPortCF();
  template<typename T> void setSideCF(const double);
  void setMainPort(const double,const double);

  void setMainRadius(const double R) { mainRadius=R; }
  void setWallThick(const double W) { wallThick=W; }
  void setVertical(const double D,const double H)
    { depth=D; height=H; }
  
  void generateTube(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
 
