/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/M1DetailGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef setVariable_M1DetailGenerator_h
#define setVariable_M1DetailGenerator_h

namespace setVariable
{

/*!
  \class M1DetailGenerator
  \author S. Ansell
  \version 1.0
  \date October 2023
  \brief Double Mirror Mono arrangement
*/

class M1DetailGenerator 
{
 private:

      
  double mWidth;               ///< Width of mirror
  double mHeight;              ///< Depth into mirror
  double mLength;              ///< Length along mirror
  double mSlotXStep;           ///< Length along mirror
  double mSlotWidth;           ///< Length along mirror
  double mSlotDepth;           ///< Length along mirror
  double mPipeXStep;           ///< step from mirror surface to base join
  double mPipeYStep;           ///< step from mirror end
  double mPipeZStep;           ///< step from lower base up.

  double mPipeSideRadius;      ///< radius at side
  double mPipeWallThick;       ///< thickness of wall

  double mPipeBaseLen;      ///< radius at base
  double mPipeBaseRadius;      ///< radius at base

  double mPipeOuterLen;       ///< Length of outer pipe vertical
  double mPipeOuterRadius;     ///< radius of outer pipe vertical
  
  std::string mirrorMat;             ///< XStal material
  std::string waterMat;              ///< Plate material
  std::string pipeMat;               ///< Base material
  std::string outerMat;               ///< Base material
  std::string voidMat;               ///< Void material

  
  void makeCrystal(FuncDataBase&,const std::string&,
		   const double,const double) const;

 public:

  M1DetailGenerator();
  M1DetailGenerator(const M1DetailGenerator&);
  M1DetailGenerator& operator=(const M1DetailGenerator&);
  virtual ~M1DetailGenerator();

  void generateMirror(FuncDataBase&,const std::string&,
		      const double,const double) const;

};

}

#endif
 
