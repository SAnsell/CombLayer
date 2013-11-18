/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuildInc/BeFilter.h
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
#ifndef bibSystem_bibBeFilter_h
#define bibSystem_bibBeFilter_h

class Simulation;

namespace bibSystem
{

/*!
  \class BeFilter
  \author R. Vivanco
  \version 1.0
  \date April 2013
  \brief Specialized for filterbox
*/

class BeFilter : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int befilterindex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy angle
  double zAngle;                  ///< zAngle step

  double width;                   ///< width of Be filter
  double height;                  ///< height of Be filter
  double length;                  ///< depth of Be filter
  double wallThick;                  ///< wall of Be filter
 
  int wallMat;
  int beMat;
  double beTemp;
 
//  int watMat;                   ///< Water premd material
//  int wallMat;                    ///< Wall premd material
  
  
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createLinks();

  public:

  BeFilter(const std::string&);
  BeFilter(const BeFilter&);
  BeFilter& operator=(const BeFilter&);
  virtual ~BeFilter();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const attachSystem::ContainedComp&);
  
};

}

#endif
 
