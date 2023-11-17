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

  size_t mNWater;             ///< water channels
  double mWaterLength;        ///< water length along silicon
  double mWaterWidth;         ///< water width
  double mWaterHeight;        ///< water height
  double mWaterDrop;          ///< water gap to Si surface
  double mWaterGap;           ///< water gap between

  //-------------------------------------------------------------
  // Front heat shield
  // -------------------------------------------------------------

  double fBaseGap;           ///< Gap from crystal end surf
  double fBaseThick;         ///< Thickness in Y direction
  double fBaseWidth;         ///< Full width
  double fBaseHeight;        ///< Full height

  double fCubePlateThick;    ///< Inner cube plate thickness [across full space)
  double fCubeThick;         ///< Thickness in Y direction
  double fCubeWidth;         ///< Full width [back of xstal]
  double fCubeHeight;        ///< Full height [xstal centre]
  double fCubeSideWall;      ///< Thickness of side wall
  double fCubeBaseWall;      ///< Thickness of base wall
  
  double fInnerThick;        ///< inner block thickness (Y)
  double fInnerHeight;       ///< inner block height
  double fInnerWidth;        ///< inner block width

  double fPipeYStep;         ///< Symmetic y-staep
  double fPipeLength;         ///< pipe length
  double fPipeRadius;        ///< inner radius
  double fPipeWall;          ///< Wall thickness

  //-------------------------------------------------------------
  // Support
  // -------------------------------------------------------------
  
  double sVOffset;             ///< Vertical ofset (6.5)
  double sLength;              ///< Length of support (26.5)
  double sXOut;                ///< Support +ve x value (from back plane)
  double sThick;               ///< Length of support (0.1)
  double sEdge;                ///< drop  of support (1.1)
  double sRadius;              ///< radius of mid-hole  ()

  double scLen;                ///< Length of cross connectors (3)
  
  double bLength;              ///< Length of support (38.9)
  double bClearGap;            ///< Clear Gap (0.2)
  double bBackThick;           ///< Thickness of back (0.5)
  double bMainThick;           ///< Thickness of C-cups (0.3)
  double bExtentThick;         ///< Thickness of C-cups (0.4)
  double bCupHeight;           ///< Height of C-cups (1.8)
  double bTopExtent;           ///< Length of top step (4.2)
  double bBaseExtent;          ///< Length of top step (2.1)
  double bVoidExtra;           ///< Extra thickness for support
  double bVoidBaseExtra;       ///< Extra thickness for support at base
  double bVoidXExtra;          ///< Extra thickness for support at side

  double clipYStep;            ///< Step from end
  double clipLen;              ///< Length of clip
  double clipSiThick;          ///< Clip thickness parallel to Si
  double clipAlThick;          ///< Clip thickness parallel to Al
  double clipExtent;           ///< Clip length away from back

  double standoffRadius;       ///< Standoff radius
  std::vector<Geometry::Vec3D> standPts;

  // supports:
  double bFrontSupportThick;   ///< Thickness of angle support
  double bFrontSupportCut;     ///< Distance to start of cut from mirror face
  double bFrontSupportZCut;    ///< Distance up to cut

  double bRingYStep;             ///< Positive displacment of ring section
  double bRingOuterThick;        ///< Radial thickness of outer (thin) ring
  double bRingOuterLength;       ///< Length along mirror of outer ring
  double bRingInnerYStep;        ///< Step of inner ring relative to outer
  double bRingInnerThick;        ///< Radial thickness of inner (fat) ring
  double bRingInnerLength;       ///< Length along mirror of inner ring
  
  double eXOut;                 ///< Step from the back plane (79.8)
  double eLength;               ///< Length of electorn shield (38.0)
  double eThick;                ///< Length of electorn shield (0.1)
  double eHeight;               ///< Height (internal) of electorn shield (6.8)
  double eEdge;                 ///< Internal step (10.3)
  double eHoleRadius;           ///< Hole in centre (2.36/2)

  double eConnectLength;       ///< External Length
  double eConnectGap;          ///< thickness of gap
  double eConnectThick;        ///< thickness of strips

  double eBlockOffset;         ///< Offset of block from wall
  double eBlockWidth;          ///< Width of block Outward to plate

  double ePlateOffset;         ///< Distance to start of plate
  double ePlateThick;          ///< Thickness of plate
  double ePlateHeight;         ///< Extent up / down of plate

  double ePipeRadius;         ///< Radius of long pipe
  double ePipeThick;            ///< Thickenss of pipe
  

  //
  double fBladeInRad;            ///< Increase in radius
  double fBladeOutRad;            ///< Increase in radius
  double fBladeThick;             ///< Thickness of one fBlade
  double fBladeTopAngle;          ///< Angle to stop the top part of circle
  double fBladeBaseAngle;         ///< Angle to stop the base part of cube
  double fBladeBaseWidth;         ///< Full width of base
  double fBladeBaseHeight;        ///< Height to down cut
  double fBladeFullHeight;        ///< Height from midline to base
  
  std::string mirrorMat;             ///< XStal material
  std::string waterMat;              ///< Plate material
  std::string supportMat;            ///< Plate (support) material
  std::string springMat;             ///< Separator material
  std::string clipMat;               ///< Separator material
  std::string electronMat;           ///< Electron shield material
  std::string ringMat;               ///< Ring material
  std::string pipeMat;               ///< Base material
  std::string outerMat;              ///< Base material
  std::string frontMat;              ///< front heat material
  std::string voidMat;               ///< Void material

  
  void makeCrystal(FuncDataBase&,const std::string&,
		   const double,const double) const;
  void makeElectronShield(FuncDataBase&,const std::string&) const;
  void makeFrame(FuncDataBase&,const std::string&) const;
  void makeSupport(FuncDataBase&,const std::string&) const;
  void makeFrontPlate(FuncDataBase&,const std::string&) const;
  void makeBackPlate(FuncDataBase&,const std::string&) const;
  void makeConnectors(FuncDataBase&,const std::string&) const;
  void makeOuterSupport(FuncDataBase&,const std::string&) const;
  void makeRingSupport(FuncDataBase&,const std::string&) const;
  
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
 
