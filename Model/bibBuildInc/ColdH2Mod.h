/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/ColdH2Mod.h
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
#ifndef bibSystem_bibColdH2Mod_h
#define bibSystem_bibColdH2Mod_h

class Simulation;

namespace bibSystem
{

/*!
  \class ColdH2Mod
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Specialized for wheel
*/

class ColdH2Mod : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int coldIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double width;                   ///< width of moderator
  double height;                  ///< height of moderator
  double depth;                   ///< depth of moderator
  double wallThick;               ///< Wall thickness

  double sideGap;                 ///< side Gaps
  double frontGap;                ///< front [target] Gaps
  double backGap;                 ///< back Gaps
  double vertGap;                 ///< vertical Gaps


  int modMat;		          ///< Moderador material
  int waterMat;                   ///< Water material
  int wallMat;                    ///< Wall material
  
  double premThick;               ///< Premoderador thickness
  double wallPremThick;           ///< Wall thickness
  double premGap;                 ///< Gap thickness

  double modTemp;                 ///< moderator temperature
  double waterTemp;               ///< water temperature [premod]
  
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&);
  void createLinks();

  public:

  ColdH2Mod(const std::string&);
  ColdH2Mod(const ColdH2Mod&);
  ColdH2Mod& operator=(const ColdH2Mod&);
  virtual ~ColdH2Mod();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const size_t);
  
};

}

#endif
 
