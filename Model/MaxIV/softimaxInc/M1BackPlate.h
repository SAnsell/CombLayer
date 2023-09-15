/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1BackPlate.h
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
#ifndef xraySystem_M1BackPlate_h
#define xraySystem_M1BackPlate_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1BackPlate
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1BackPlate :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double length;              ///< Length of support 
  double clearGap;            ///< Clearance gap to crystal
  double backThick;           ///< Thickness of back 
  double mainThick;           ///< Thickness of C-cups
  double extentThick;         ///< Thickness of extras
  double cupHeight;           ///< Height of C-cups 
  double topExtent;           ///< Length of top step
  double baseExtent;          ///< Length of top step
  double voidExtra;           ///< Extra space in voids for supports
  double voidBaseExtra;       ///< Extra space at base for supports
  double voidXExtra;          ///< Extra space in voids for supports (X)

  // spring supports 
  double supVOffset;           ///< offset from Crystal surface 
  double supLength;            ///< length
  double supXOut;              ///< xout step distance
  double supThick;             ///< thickness
  double supEdge;              ///< edge thickness
  double supHoleRadius;        ///< central hole radius

  double springConnectLen;     ///< spring to electronconnect block length
  
  // electron shield
  double elecXOut;            ///< xout step distance
  double elecLength;          ///< length
  double elecThick;           ///< thickness
  double elecHeight;          ///< height
  double elecEdge;            ///< edge thickness
  double elecHoleRadius;      ///< central hole radius

  double outerVaneThick;      ///< Thickness of outer vanes
  double innerVaneThick;      ///< Thickness of inner vanes

  double frontPlateGap;       ///< Gap to crsytal surface
  double frontPlateWidth;     ///< Width from innerBase surface
  double frontPlateHeight;    ///< Height (centred on crystal)

  double frontSupportThick;   ///< Thickness of angle support
  double frontSupportCut;     ///< Distance to start of cut from mirror face
  double frontSupportZCut;    ///< Distance up to cut

  double ringYPos;            ///< Y Position down support
  double ringThick;           ///< Metal thickness
  double ringGap;             ///< Vertical gap between top/base
  double ringClampThick;      ///< Outer ring clamp extra radius
  Geometry::Vec3D ringBackPt; ///< ring offset from back/flat corners
  Geometry::Vec3D ringTopPt;  ///< ring offset from inner high top
  
  int baseMat;                ///< Base material
  int springMat;              ///< Spring join material
  int supportMat;             ///< Base material
  int electronMat;            ///< Electron shield material
  int frontMat;               ///< Front heat material
  int voidMat;                ///< void material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createSupportSurfaces();
  void createSupportObjects(Simulation&);
  void createLinks();

 public:

  M1BackPlate(const std::string&);
  M1BackPlate(const M1BackPlate&);
  M1BackPlate& operator=(const M1BackPlate&);
  ~M1BackPlate() override;
  void adjustExtraVoids(Simulation&,const int,const int,const int);
  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
