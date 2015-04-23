/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/HWrapper.h
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
#ifndef moderatorSystem_HWrapper_h
#define moderatorSystem_HWrapper_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class HWrapper
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief HWrapper [insert object]
*/

class HWrapper : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int preIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var
  
  int divideSurf;               ///< Divider surface for cylinders

  double sideExt;              ///< Extra at side [total]
  double heightExt;            ///< Extra at      [total]
  double backExt;              ///< Total depth   [Total]
  double forwardExt;           ///< Total depth   [Total]
  double wingLen;              ///< Forward length of wing

  double vacInner;      ///< Inner Vac spacing 
  double alInner;       ///< Inner Al layer
  double alOuter;       ///< Outer Al layer
  double vacOuter;      ///< Outer spacing


  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  
  void populate(const Simulation&);

  void createSurfMesh(const int,const int,const int*,
		      const double*,const int*);

  void createUnitVector(const attachSystem::FixedComp&);


  void createSurfaces(const attachSystem::FixedComp&,
		      const attachSystem::FixedComp&);
  void createObjects(Simulation&,
		     const attachSystem::ContainedComp&);

 public:

  HWrapper(const std::string&);
  HWrapper(const HWrapper&);
  HWrapper& operator=(const HWrapper&);
  ~HWrapper();

  /// Divide Plane setter
  void setDivideSurf(const int SN) { divideSurf=SN; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 
