/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/CrossWayGenerator.h
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
#ifndef setVariable_CrossWayGenerator_h
#define setVariable_CrossWayGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CrossWayGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief CrossWayGenerator for variables
*/

class CrossWayGenerator 
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
  double sideALength;           ///< full to flange length
  double sideBLength;           ///< full to flange length

  double flangeXRadius;         ///< Joining Flange radius
  double flangeYRadius;         ///< Joining Flange radius
  double flangeZRadius;         ///< Joining Flange radius

  double flangeXLength;         ///< Joining Flange radius
  double flangeYLength;         ///< Joining Flange radius
  double flangeZLength;         ///< Joining Flange radius
  
  double plateThick;            ///< Joining Flange radius

  std::string voidMat;          ///< void material
  std::string wallMat;          ///< main material
  std::string plateMat;         ///< plate material
 
 public:

  CrossWayGenerator();
  CrossWayGenerator(const CrossWayGenerator&);
  CrossWayGenerator& operator=(const CrossWayGenerator&);
  virtual ~CrossWayGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();
  template<typename CF> void setPortCF();
  template<typename CF> void setCrossCF();
  
  void setMainLength(const double,const double);
  void setPortLength(const double,const double);
  void setCrossLength(const double,const double);
  void setPlateThick(const double,const std::string&);
  
  void generateCrossWay(FuncDataBase&,
		       const std::string&) const;

};

}

#endif
 
