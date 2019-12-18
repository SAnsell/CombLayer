/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomChopper.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef zoomSystem_ZoomChopper_h
#define zoomSystem_ZoomChopper_h

class Simulation;

namespace shutterSystem
{
  class GeneralShutter;
  class BulkShield;
}

namespace zoomSystem
{
  class ZoomBend;

/*!
  \class ZoomChopper
  \version 1.0
  \author S. Ansell
  \date September 2011
  \brief ZoomChopper unit  
*/

class ZoomChopper : public attachSystem::FixedGroup,
    public attachSystem::ContainedComp
{
 private:
  

  double xStep;                 ///< Origin step
  double yStep;                 ///< Origin step
  double zStep;                 ///< Origin step

  double length;               ///< Full length
  double depth;                ///< Full depth
  double height;               ///< Full height
  double leftWidth;            ///< long width
  double rightWidth;           ///< Long width
  double leftAngle;            ///< Side angle from bulk vs Centre line
  double rightAngle;           ///< Side angle from bulk vs Centre line
  double leftWaxAngle;         ///< Side angle from bulk vs Centre line
  double rightWaxAngle;        ///< Side angle from bulk vs Centre line

  double voidLeftWidth;        ///< Void Main hole [left]
  double voidRightWidth;       ///< Void Main hole [right]
  double voidHeight;           ///< Void Main hole [height]
  double voidDepth;            ///< Void Main hole [depth]

  double voidCut;              ///< Void Main hole [back cut towards BulkWall]
  double voidChanLeft;         ///< Void Channel [left]
  double voidChanRight;        ///< Void Channel [right]
  double voidChanUp;           ///< Void Channel [up]
  double voidChanDown;         ///< Void Channel [down]

  double voidEnd;              ///< Void Main hole [back cut towards BulkWall]
  double voidEndLeft;          ///< Void Channel [left]
  double voidEndRight;         ///< Void Channel [right]
  double voidEndUp;            ///< Void Channel [up]
  double voidEndDown;          ///< Void Channel [down]
  int voidEndMat;              ///< Material for void end

  double voidEndBeamLeft;          ///< Void Channel [left]
  double voidEndBeamRight;         ///< Void Channel [right]
  double voidEndBeamUp;            ///< Void Channel [up]
  double voidEndBeamDown;          ///< Void Channel [down]

  double waxLeftSkin;          ///< Thickness of wax layer
  double waxRightSkin;         ///< Thickness of wax

  size_t nShield;              ///< number of layers
  int shieldCell;
  std::vector<double> shieldFrac;
  std::vector<int> shieldMat;
  
  int wallMat;              ///< Material for walls
  int waxMat;               ///< Material for walls (outer)

  // ROOF:
  double roofBStep;            ///< Roof back step
  double roofHeight;           ///< Roof Height
  double roofExtra;            ///< Extra roof size
  int roofMat;                 ///< Main roof material
  int roofExtraMat;            ///< Roof extra material

  double outerRadius;          ///< Special outer radius [isolated case]
  double outerOffset;          ///< Special outer radius [isolated case]

  int monoWallSurf;      ///< Montolith Exit surface
  int voidCell;          ///< Inner void
  
  void populate(const FuncDataBase&);
  void createUnitVector(const zoomSystem::ZoomBend&);
  
  void createSurfaces(const shutterSystem::GeneralShutter&);
  void createSurfaces();
  void createSurfacesCommon();

  void createObjects(Simulation&,const attachSystem::ContainedGroup&);
  void createLinks();
  void divideMainShield(Simulation&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) {}
 public:

  ZoomChopper(const std::string&);
  ZoomChopper(const ZoomChopper&);
  ZoomChopper& operator=(const ZoomChopper&);
  virtual ~ZoomChopper();

  /// Set surface
  void setMonoSurface(const int M) { monoWallSurf=M; }
  /// Get void cell
  int getVoidCell() const { return voidCell; }  
  /// Access central axis
  const Geometry::Vec3D& getBeamAxis() const {  return Y; }

  void createAll(Simulation&,const zoomSystem::ZoomBend&,
		 const shutterSystem::GeneralShutter&);
  void createAll(Simulation&,const zoomSystem::ZoomBend&);

};

}

#endif
 
