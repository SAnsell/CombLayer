/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t2BuildInc/OrthoInsert.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef moderatorSystem_OrthoInsert_h
#define moderatorSystem_OrthoInsert_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class OrthoInsert
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Insert o
*/

class OrthoInsert :
    public attachSystem::ContainedGroup,
    public attachSystem::FixedUnit,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:
  
  Geometry::Vec3D GCent;     ///< Groove centre
  double grooveThick;       ///< Total Width
  double grooveWidth;       ///< Width of the groove
  double grooveHeight;      ///< Height of the groove
  
  Geometry::Vec3D HCent;     ///< Hydrogen centre (cylinder)
  double HRadius;            ///< Hydrogen radius 
  double hydroThick;         ///< Total Width
  

  double orthoTemp;         ///< Temperature [-ve ==> default]
  int orthoMat;             ///< Material

  void populate(const FuncDataBase&);
  void setModeratorCentres(const Hydrogen&,
			const Groove&);

  void createSurfaces(const Hydrogen&);
  void createObjects(Simulation&);

 public:

  OrthoInsert(const std::string&);
  OrthoInsert(const OrthoInsert&);
  OrthoInsert& operator=(const OrthoInsert&);
  ~OrthoInsert() override;

  int viewSurf() const;

  void build(Simulation&,const Hydrogen&,const Groove&);

};

}

#endif
 
