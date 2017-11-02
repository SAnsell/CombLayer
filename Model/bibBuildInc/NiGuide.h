/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/NiGuide.h
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
#ifndef bibSystem_bibNiGuide_h
#define bibSystem_bibNiGuide_h

class Simulation;

namespace bibSystem
{

/*!
  \class NiGuide
  \author R. Vivanco
  \version 1.0
  \date April 2013
  \brief Specialized for filterbox
*/

class NiGuide : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int niGuideIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double width;                   ///< width of Ni guide
  double height;                  ///< height of Ni guide
  double length;                  ///< depth of Ni guide
  double wallThick;                  ///< wall of Ni guide
 
  int wallMat;
   
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces(const attachSystem::FixedComp&,const long int,
		     const attachSystem::FixedComp&,const long int);
  
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createLinks();

  public:

  NiGuide(const std::string&);
  NiGuide(const NiGuide&);
  NiGuide& operator=(const NiGuide&);
  virtual ~NiGuide();

  void createAll(Simulation&, 
		 const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::ContainedComp&);
  
};

}

#endif
 
