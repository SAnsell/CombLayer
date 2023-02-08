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

  // Support wheel
  double wheelRadius;          ///< main wheel radius
  double wheelOuterRadius;     ///< full outer reaius
  double wheelHubRadius;       ///< Hub radius
  double wheelHeight;          ///< Thickness of wheel
  size_t nSpokes;              ///< nummber of slots
  double spokeThick;           ///< spoke thickness
  double spokeCornerRadius;    ///< nice radius in corner
  double spokeCornerGap;       ///< gap at thin points


  double radialLength;               ///< Total length
  double radialSupportLen;           ///< Support length

  double radialTopGap;               ///< Top plate drop down
  double radialTopThick;             ///< Top plate thickness
  double radialTopBeamWidth;         ///< Top plate width
  double radialTopOutWidth;          ///< Top plate width

  double radialPlateThick;           ///< Thickness of plate
  double radialPlateLength;          ///< Thickness of plate
  double radialPlateBeam;            ///< Extra extention under beam
  double radialPlateXStep;           ///< Width of straight block
  
  double radialSideWidth;            ///< Width of straight block
  double radialSideBlock;            ///< Length of outside block
  double radialSideLift;             ///< extra straight vertical
  double radialSideFullWidth;        ///< Length between inner blocks
  double radialSideBaseWidth;        ///< Length between inner blocks
  double radialSideOutWidth;         ///< Length between outer blocks

  double radialSupportHeight;        ///< Height from base to edge block
  double radialSupportOuterHeight;   ///< corner bolt blocks
  double radialSupportInnerLift;     /// inner curve maximum 
  
  double radialBladeDrop;            ///< Distance blades down 
  double radialBladeThick;           ///< blade thickness
  double radialBladeHeight;          ///< blade height from base plate
  double radialBladeTopGap;          ///< blade separation at top
  double radialBladeBaseGap;         ///< blade separation at base 

  double radialBaseThick;            ///< Full base thickness
  
  std::string plateMat;              ///< Plate material
  std::string mirrorMat;             ///< XStal material
  std::string baseMat;               ///< Base material
  std::string voidMat;               ///< Void material

  void makeRadialSupport(FuncDataBase&,const std::string&,
			 const double,const double) const;
  
  void makeSupportWheel(FuncDataBase&,const std::string&,
			const double,const double) const;
  
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
 
