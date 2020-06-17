/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/SixPortGenerator.h
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
#ifndef setVariable_SixPortGenerator_h
#define setVariable_SixPortGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SixPortGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief SixPortGenerator for variables
*/

class SixPortGenerator 
{
 private:
  
  double radius;               ///< void radius
  double linkRadius;           ///< void radius on 4 cross way
  double wallThick;            ///< pipe thickness

  double frontLength;            ///< full to flange length
  double backLength;             ///< full to flange length
  double sideLength;              ///< full to flange length

  double flangeARadius;         ///< Front Flange radius
  double flangeBRadius;         ///< Back Flange radius
  double flangeSRadius;         ///< Side Flange radius

  double flangeALength;         ///< Front Flange length
  double flangeBLength;         ///< Back Flange length
  double flangeSLength;         ///< Side Flange length
  
  double plateThick;         ///< Joining Flange radius

  std::string voidMat;                 ///< void material
  std::string mainMat;                 ///< main material
  std::string flangeMat;               ///< flange material
  std::string plateMat;                ///< plate material

 public:

  SixPortGenerator();
  SixPortGenerator(const SixPortGenerator&);
  SixPortGenerator& operator=(const SixPortGenerator&);
  virtual ~SixPortGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();

  void generateSixPort(FuncDataBase&,
		       const std::string&) const;

};

}

#endif
 
