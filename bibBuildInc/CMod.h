/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuildInc/CMod.h
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
#ifndef bibSystem_bibCMod_h
#define bibSystem_bibCMod_h

class Simulation;

namespace bibSystem
{

/*!
  \class CMod
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Specialized for wheel
*/

class CMod : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int watIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double zStep;                   ///< Z step

  double width;                   ///< width of moderator
  double height;                 
 ///< height of moderator
  double depth;                   ///< depth of moderator
  double wallThick;               ///< Wall thickness

  double sideGap;                 ///< Gaps
  double frontGap;                ///< Gaps
  double backGap;                 ///< Gaps
  double vertGap;                 ///< Gaps

  double waterTemp;               ///< water temperature

  int waterMat;                   ///< Water material
  int wallMat;                    ///< Wall material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void createSurfaces(const attachSystem::FixedComp&,const size_t);
  void createObjects(Simulation&);
  void createLinks();

  public:

  CMod(const std::string&);
  CMod(const CMod&);
  CMod& operator=(const CMod&);
  virtual ~CMod();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);
  
};

}

#endif
 
