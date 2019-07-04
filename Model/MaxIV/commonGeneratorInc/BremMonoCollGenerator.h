/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/BremMonoCollGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_BremMonoCollGenerator_h
#define setVariable_BremMonoCollGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BremMonoCollGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BremMonoCollGenerator for variables
*/

class BremMonoCollGenerator
{
 private:

  double radius;              ///< Main radius
  double gap;                 ///< Gap distance to start
  double wallThick;           ///< wall thickness of tube

  double flangeRadius;        ///< Joining Flange radius
  double flangeLength;        ///< Joining Flange length

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double holeWidth;           ///< Front width of hole
  double holeHeight;          ///< Front height of hole

  double inRadius;            ///< inlet radius
  
  std::string voidMat;                ///< void material
  std::string innerMat;               ///< Tungsten material
  std::string wallMat;                ///< Fe material layer

  
 public:

  BremMonoCollGenerator();
  BremMonoCollGenerator(const BremMonoCollGenerator&);
  BremMonoCollGenerator& operator=(const BremMonoCollGenerator&);
  ~BremMonoCollGenerator();


  void setAperature(const double,const double);

  void setMaterial(const std::string&,const std::string&);
  
  void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
