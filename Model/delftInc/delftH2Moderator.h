/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/delftH2Moderator.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef delftSystem_H2Moderator_h
#define delftSystem_H2Moderator_h

class Simulation;

namespace delftSystem
{

/*!
  \class delftH2Moderator
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief delftH2Moderator [insert object]
*/

class delftH2Moderator : public virtualMod
{
 private:
  
  const int hydIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double depth;             ///< Total depth [Max]
  double sideRadius;        ///< Side (height+width) radius
  double innerXShift;       ///< Shift of inner space

  int frontDir;             ///< Front rotation +ve outward/-ve inward
  double frontRadius;       ///< Rear radius [-ve to be concave]
  int backDir;              ///< Back rotation +ve outward/-ve inward
  double backRadius;        ///< Rear radius [-ve to be concave]

  double alBack;            ///< Divider wall thickness
  double alFront;           ///< Front surface
  double alTop;             ///< Top [away from target]
  double alBase;            ///< Base [near target]
  double alSide;            ///< Side walls

  double modTemp;           ///< Moderator temperature
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  Geometry::Vec3D FCentre;  ///< Centre of Front Radius / Plane
  Geometry::Vec3D BCentre;  ///< Centre of Back Radius / Plane
  int HCell;                ///< Main H2 cell

  static int calcDir(const double);
  Geometry::Vec3D calcCentre(const int,const int,const double) const;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  delftH2Moderator(const std::string&);
  delftH2Moderator(const delftH2Moderator&);
  delftH2Moderator& operator=(const delftH2Moderator&);
  virtual delftH2Moderator* clone() const;
  virtual ~delftH2Moderator();

  int getDividePlane() const;
  int viewSurf() const;
  /// Access to hydrogen region
  virtual int getMainBody() const { return HCell; }

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  virtual void postCreateWork(Simulation&);
};

}

#endif
 
