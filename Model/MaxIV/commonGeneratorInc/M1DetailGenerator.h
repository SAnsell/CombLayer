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

  double sVOffset;             ///< Vertical ofset (6.5)
  double sLength;              ///< Length of support (26.5)
  double sXOut;                ///< Support +ve x value (from back plane)
  double sThick;               ///< Length of support (0.1)
  double sEdge;                ///< drop  of support (1.1)
  double sRadius;              ///< radius of mid-hole  ()

  double bLength;              ///< Length of support (38.9)
  double bClearGap;            ///< Clear Gap (0.2)
  double bBackThick;           ///< Thickness of back (0.5)
  double bMainThick;           ///< Thickness of C-cups (0.3)
  double bExtentThick;         ///< Thickness of C-cups (0.4)
  double bCupHeight;           ///< Height of C-cups (1.8)
  double bTopExtent;           ///< Length of top step (4.2)
  double bBaseExtent;          ///< Length of top step (2.1)

  double eXOut;                 ///< Step from the back plane (79.8)
  double eLength;               ///< Length of electorn shield (38.0)
  double eThick;                ///< Length of electorn shield (0.1)
  double eHeight;               ///< Height (internal) of electorn shield (6.8)
  double eEdge;                 ///< Internal step (10.3)
  double eHoleRadius;           ///< Hole in centre (2.36/2)
  
  std::string mirrorMat;             ///< XStal material
  std::string waterMat;              ///< Plate material
  std::string supportMat;            ///< Plate (support) material
  std::string electronMat;           ///< Electron shield material
  std::string pipeMat;               ///< Base material
  std::string outerMat;              ///< Base material
  std::string voidMat;               ///< Void material

  
  void makeCrystal(FuncDataBase&,const std::string&,
		   const double,const double) const;
  void makeSupport(FuncDataBase&,const std::string&) const;
  void makeBackPlate(FuncDataBase&,const std::string&) const;


 public:

  M1DetailGenerator();
  M1DetailGenerator(const M1DetailGenerator&);
  M1DetailGenerator& operator=(const M1DetailGenerator&);
  virtual ~M1DetailGenerator();

  void generateSupport(FuncDataBase&,const std::string&) const;
  void generateMirror(FuncDataBase&,const std::string&,
		      const double,const double) const;

};

}

#endif
 