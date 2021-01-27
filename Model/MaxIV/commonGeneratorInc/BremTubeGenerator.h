/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/BremTubeGenerator.h
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
#ifndef setVariable_BremTubeGenerator_h
#define setVariable_BremTubeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BremTubeGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief BremTubeGenerator for variables
*/

class BremTubeGenerator 
{
 private:
  
  
  double frontRadius;           ///< Front radius
  double frontLength;           ///< Front length
  double frontFlangeRadius;     ///< Front length
  double frontFlangeLength;     ///< Front length
 
  double midRadius;             ///< mid radius
  double midLength;             ///< mid length

  double tubeRadius;            ///< main radius
  double tubeHeight;            ///< tube  height
  double tubeDepth;             ///< main tube  depth
  double tubeFlangeRadius;      ///< tube flange radius
  double tubeFlangeLength;      ///< tube Flange Length

  double backRadius;            ///< Back radius
  double backLength;            ///< back length
  double backFlangeRadius;      ///< back flange radius
  double backFlangeLength;      ///< back flange length

  double wallThick;             ///< pipe thickness  
  double plateThick;            ///< Joining Flange radius

  std::string voidMat;          ///< Void material
  std::string wallMat;          ///< Wall Material
  std::string plateMat;         ///< Plate Material
  
  
 public:

  BremTubeGenerator();
  BremTubeGenerator(const BremTubeGenerator&);
  BremTubeGenerator& operator=(const BremTubeGenerator&);
  virtual ~BremTubeGenerator();

  template<typename T> void setCF();

  void setVertical(const double D,const double H)
    { tubeDepth=D; tubeHeight=H; }
  
  void generateTube(FuncDataBase&,const std::string&) const;

};

}

#endif
 
