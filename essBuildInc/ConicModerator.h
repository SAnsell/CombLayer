/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/ConicModerator.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef essSystem_ConicModerator_h
#define essSystem_ConicModerator_h

class Simulation;

namespace essSystem
{

/*!
  \class ConicModerator
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief ConicModerator [insert object]
*/

class ConicModerator : public essMod
{
 private:
  
  const int conicIndex;      ///< Index of surface offset
  int cellIndex;            ///< Cell index

  double xStep;             ///< X Step
  double yStep;             ///< Y Step
  double zStep;             ///< Z Step
  double xyAngle;           ///< xy rotation angle
  double zAngle;            ///< z rotation angle

  double IWidth;             ///< Horrizontal [small dist]
  double IHeight;            ///< Vertical [small dist]
  double OWidth;             ///< Horrizontal [small dist]
  double OHeight;            ///< Vertical [small dist]
  double length;            ///< inner apex - cut plane 
  double innerAngle;        ///< Inner angle of cone
  double topAngle;          ///< Outer angle of cone
  double baseAngle;         ///< Outer angle of cone
  double thick;             ///< Layer thickness
  double faceThick;         ///< Front layer depth
  
  double alThick;            ///< Thickness of AL view 

  double vacGap;            ///< Vac gap 
  double waterAlThick;      ///< water thickness
  double waterThick;        ///< Thickness of AL view 
  double voidGap;           ///< Thickness of AL view 

  double modTemp;           ///< Moderator temperature
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material
  int waterMat;                ///< Water Material

  int HCell;                ///< Main H2 cell
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ConicModerator(const std::string&);
  ConicModerator(const ConicModerator&);
  ConicModerator& operator=(const ConicModerator&);
  virtual ConicModerator* clone() const;
  virtual ~ConicModerator();

  /// Access to hydrogen region
  virtual int getMainBody() const { return HCell; }

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
