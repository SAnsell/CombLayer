/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/DiskPreModFlowGuide.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_DiskPreModFlowGuide_h
#define essSystem_DiskPreModFlowGuide_h

class Simulation;

namespace essSystem
{
/*!
  \class DiskPreModFlowGuide
  \author K. Batkov
  \version 1.0
  \date Aug 2015
  \brief Inner structure of Be reflector (engineering details)
*/

class DiskPreModFlowGuide : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int insIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double wallThick;          ///< Wall thickness
  int    wallMat;            ///< Wall material

  size_t nBaffles;        ///< Number of baffles

  double gapWidth;      ///< Gap width

  double BeRadius;                ///< Inner Be radius
  int    BeMat;                   ///< Inner Be material
  double BeWallThick;             ///< Inner Be wall thickness
  int    BeWallMat;               ///< Inner Be wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&, const attachSystem::FixedComp&);
  void createLinks();

 public:

  DiskPreModFlowGuide(const std::string&);
  DiskPreModFlowGuide(const DiskPreModFlowGuide&);
  DiskPreModFlowGuide& operator=(const DiskPreModFlowGuide&);
  virtual DiskPreModFlowGuide* clone() const;
  virtual ~DiskPreModFlowGuide();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
