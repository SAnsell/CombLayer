/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/XRayHutchBaseGenerator.h
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
#ifndef setVariable_XRayHutchBaseGenerator_h
#define setVariable_XRayHutchBaseGenerator_h

class FuncDataBase;

namespace setVariable
{
/*!
  \class XRayHutchBaseGenerator
  \version 1.0
  \author K. Batkov
  \date December 2025
  \brief XRayHutchBase variable generator
*/

class XRayHutchBaseGenerator
{
 protected:
      double height;                ///< void height
      double length;                ///< void out side width
      double outWidth;              ///< Width from beamline centre to outside

      // walls
      double innerThick;            ///< Inner wall/roof skin
      double pbWallThick;           ///< Thickness of lead in walls
      double pbBackThick;           ///< Thickness of lead in back plate
      double pbRoofThick;           ///< Thickness of lead in Roof
      double outerThick;            ///< Outer wall/roof skin

      double innerOutVoid;          ///< Extension for inner left void space
      double outerOutVoid;          ///< Extension for outer left void space
      double outerBackVoid;         ///< Extension for outer back void space

      double floorShineThick;       ///< Floor shine thickness
      double floorShineLength;      ///< Floor shine full length (starting from the wall outer surface) - general length that is overriden by the particular lengths below

      std::vector<Geometry::Vec3D> holeOffset;  ///< hole offsets [y ignored]
      std::vector<double> holeRadius;           ///< hole radii

      std::string voidMat;                ///< Void material
      std::string skinMat;                ///< Fe layer material for walls
      std::string pbMat;                  ///< pb layer material for walls
      std::string floorShineMat;          ///< Floor shine material

      // std::vector<std::shared_ptr<PortChicane>> PChicane; /// Chicanes
      // forkHoles forks;              ///< Forklift holes if made

public:

  XRayHutchBaseGenerator();
  XRayHutchBaseGenerator(const XRayHutchBaseGenerator&) =default;
  XRayHutchBaseGenerator& operator=(const XRayHutchBaseGenerator&) =default;
  ~XRayHutchBaseGenerator() =default;

  // set skinthickness
  virtual void setSkin(const double T) final { outerThick=T; innerThick=T;}

  virtual void setBackLead(const double T) final { pbBackThick=T; }
  virtual void setRoofLead(const double T) final { pbRoofThick=T; }
  virtual void setWallLead(const double T) final { pbWallThick=T; }
  virtual void setOuterBackExt(const double T) final { outerBackVoid=T; }

  virtual void setFloorShine(const double T, const double L) final;

  virtual void addHole(const Geometry::Vec3D&,const double) final;

  void generateHut(FuncDataBase&,const std::string&, const double) const;

};

}

#endif
