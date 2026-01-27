/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/XRayHutchBase.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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

#ifndef xraySystem_XRayHutchBase_h
#define xraySystem_XRayHutchBase_h

namespace xraySystem
{
  class PortChicane;
  /*!
    \class XRayHutchBase
    \version 1.0
    \author K. Batkov
    \date December 2025
    \brief Base abstract class for x-ray hutches
  */
  class XRayHutchBase :
    public attachSystem::FixedRotate,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
    {
    protected:
      double height;                ///< void height
      double length;                ///< void out side width
      double outWidth;              ///< Width from beamline centre to outside
      // walls
      double innerThick;            ///< Inner wall/roof skin
      double pbWallThick;           ///< Thickness of lead in walls
      double pbBackThick;           ///< Thickness of lead in back wall
      double pbRoofThick;           ///< Thickness of lead in Roof
      double outerThick;            ///< Outer wall/roof skin

      double innerOutVoid;          ///< Extension for inner left void space
      double outerOutVoid;          ///< Extension for outer left void space
      double outerBackVoid;         ///< Extension for outer back void space

      bool frontPlateActive;        ///< Front plate active flag
      double frontPlateThick;       ///< Front plate thickness
      double frontPlateWidth;       ///< Front plate width
      double frontPlateHeight;      ///< Front plate full height

      bool backPlateActive;         ///< Back plate active flag
      double backPlateThick;        ///< Back plate thick
      double backPlateWidth;        ///< Back plate width
      double backPlateHeight;       ///< back plate Height

      double floorShineThick;       ///< Floor shine thickness
      double floorShineLength;      ///< Floor shine full length (starting from the wall outer surface) - general length that is overriden by the particular lengths below
      double floorShineFrontLength; ///< Floors hine length along the front wall
      double floorShineBackLength;  ///< Floors hine length along the back wall

      std::vector<Geometry::Vec3D> holeOffset;  ///< hole offsets [y ignored]
      std::vector<double> holeRadius;           ///< hole radii

      int voidMat;                ///< Void material
      int skinMat;                ///< Fe layer material for walls
      int pbMat;                  ///< pb layer material for walls
      int floorShineMat;          ///< Floor shine material

      std::vector<std::shared_ptr<PortChicane>> PChicane; /// Chicanes
      forkHoles forks;              ///< Forklift holes if made

    public:
      XRayHutchBase(const std::string&);
      virtual ~XRayHutchBase() = default;

      /// accessor to void mat
      int getInnerMat() const { return voidMat; }

      void populate(const FuncDataBase&);
      virtual void createAll(Simulation&,const attachSystem::FixedComp&,const long int) = 0;
    };
}

#endif
