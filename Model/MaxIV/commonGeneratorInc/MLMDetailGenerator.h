/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/MLMDetailGenerator.h
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
#ifndef setVariable_MLMDetailGenerator_h
#define setVariable_MLMDetailGenerator_h

namespace setVariable
{

/*!
  \class MLMDetailGenerator
  \author S. Ansell
  \version 1.0
  \date October 2023
  \brief Double Mirror Mono arrangement
*/

class MLMDetailGenerator 
{
 private:

  double gap;                  ///< Gap thickness
      
  double width;               ///< Width of block across beam
  double height;              ///< Depth into beam
  double length;              ///< Length along beam
  
  double topSlotXStep;         ///< Slot on mirror block on top surf
  double topSlotWidth;         ///< Slot width
  double topSlotLength;        ///< Slot length
  double topSlotDepth;         ///< Slot depth

  double baseWidth;            ///< full width
  double baseLength;           ///< full length
  double baseDepth;            ///< Depth from mirror base surface
  double baseFrontHeight;      ///< Front height of lip
  double baseBackHeight;       ///< Back height of lip
  double baseInnerWidth;       ///< Space for crystal
  double baseInnerBeamFaceLen; ///< length of step (beam side) in Y
  double baseInnerOutFaceLen;  ///< length of step (far side) in Y
  double baseBackSlotLen;      ///< length cut out at back
  double baseOutSlotLen;       ///< Slot on out face sie

  std::string mirrorMat;             ///< XStal material
  std::string baseMat;               ///< Base material

  void makeCrystal(FuncDataBase&,const std::string&,
		   const bool,const double,
		   const double,const double) const;
 public:

  MLMDetailGenerator();
  MLMDetailGenerator(const MLMDetailGenerator&);
  MLMDetailGenerator& operator=(const MLMDetailGenerator&);
  virtual ~MLMDetailGenerator();

  /// accessor to gap
  void setGap(const double G) { gap=G; }
  void generateMono(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
