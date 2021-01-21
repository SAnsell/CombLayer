/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/IonGaugeGenerator.h
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
#ifndef setVariable_IonGaugeGenerator_h
#define setVariable_IonGaugeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class IonGaugeGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief IonGaugeGenerator for variables
*/

class IonGaugeGenerator 
{
 private:
  
  double radius;                ///< void radius
  double xRadius;               ///< radius of x port 
  double yRadius;               ///< radiu of Y (Beam) port

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

  IonGaugeGenerator();
  IonGaugeGenerator(const IonGaugeGenerator&);
  IonGaugeGenerator& operator=(const IonGaugeGenerator&);
  virtual ~IonGaugeGenerator();

  template<typename T> void setCF();
  template<typename T> void setPortCF();
  template<typename T> void setSideCF(const double);
  void setMainLength(const double,const double);

  void setRadius(const double R) { radius=R; }
  void setWallThick(const double W) { wallThick=W; }
  void setVertical(const double D,const double H)
    { depth=D; height=H; }
  
  void generateTube(FuncDataBase&,
		    const std::string&) const;

};

}

#endif
 
