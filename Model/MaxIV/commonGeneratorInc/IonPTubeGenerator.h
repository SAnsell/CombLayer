/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/IonPTubeGenerator.h
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
#ifndef setVariable_IonPTubeGenerator_h
#define setVariable_IonPTubeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class IonPTubeGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief IonPTubeGenerator for variables
*/

class IonPTubeGenerator 
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

  double flangeYRadius;         ///< Joining Flange radius
  double flangeZRadius;         ///< Joining Flange radius

  double flangeYLength;         ///< Joining Flange radius
  double flangeZLength;         ///< Joining Flange radius
  
  double plateThick;            ///< Joining Flange radius

  std::string voidMat;          ///< void material
  std::string wallMat;          ///< main material
  std::string plateMat;         ///< plate material
 
 public:

  IonPTubeGenerator();
  IonPTubeGenerator(const IonPTubeGenerator&);
  IonPTubeGenerator& operator=(const IonPTubeGenerator&);
  virtual ~IonPTubeGenerator();

  template<typename T> void setCF();
  template<typename T> void setMainCF();
  template<typename T> void setPortCF();
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
 
