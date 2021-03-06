/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/AreaDetectorGenerator.h
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
#ifndef setVariable_AreaDetectorGenerator_h
#define setVariable_AreaDetectorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class AreaDetectorGenerator
  \version 1.0
  \author S. Ansell
  \date March 2021
  \brief AreaDetectorGenerator for x-ray plate area detector
*/

class AreaDetectorGenerator 
{
 private:

  double length;                ///< Total length
  double width;                 ///< Width
  double height;                ///< Height
  double screenThick;           ///< Wall thickness
  double screenDepth;           ///< screen depth from front
  double wallThick;             ///< Wall thickness


  std::string voidMat;          ///< void material
  std::string detMat;           ///< main detector material
  std::string screenMat;        ///< screen material
  std::string wallMat;          ///< wall material
 
 public:

  AreaDetectorGenerator();
  AreaDetectorGenerator(const AreaDetectorGenerator&);
  AreaDetectorGenerator& operator=(const AreaDetectorGenerator&);
  virtual ~AreaDetectorGenerator();

  void generateDetector(FuncDataBase&,
			const std::string&,
			const double) const;

};

}

#endif
 
