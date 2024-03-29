/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/Groove.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef moderatorSystem_Groove_h
#define moderatorSystem_Groove_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class Groove
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief Groove [insert object]
*/

class Groove :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:
  
  double width;             ///< Total Width
  double height;            ///< Total height
  double depth;             ///< Total depth
  
  Geometry::Vec3D GCentre;  ///< Groove centre
  double innerRadius;       ///< Inner radius
  double innerXShift;       ///< Inner XShift
  double innerZShift;       ///< Z-shift of groove
  double innerWidth;        ///< Width of the groove 
  double innerHeight;       ///< Height of the groove
  double innerCut;          ///< Cut depth of the groove
  double innerSideAngleE1;  ///< E1 Side angle [void -ve to X]
  double innerSideAngleE5;  ///< E5 Side angle 

  double alInnerCurve;      ///< Inner Curve Al thickness
  double alInnerSides;      ///< Side of groove view
  double alInnerUpDown;     ///< Top/Bottom of view
  double alFront;           ///< Front surface
  double alTop;             ///< Top [away from target]
  double alBase;            ///< Base [near target]
  double alSide;            ///< Side walls

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Groove(const std::string&);
  Groove(const Groove&);
  Groove& operator=(const Groove&);
  ~Groove() override;

  int getDividePlane() const;
  int viewSurf() const;
  Geometry::Vec3D getViewPoint() const;
  Geometry::Vec3D getBackGroove() const;

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
