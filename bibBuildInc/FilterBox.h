/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuildInc/FilterBox.h
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
#ifndef bibSystem_bibFilterBox_h
#define bibSystem_bibFilterBox_h

class Simulation;

namespace bibSystem
{

/*!
  \class FilterBox
  \author R. Vivanco
  \version 1.0
  \date April 2013
  \brief Specialized for filterbox
*/

class FilterBox : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int filterIndex;          ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy angle
  double zAngle;                  ///< zAngle step

  double width;                   ///< width of filterbox
  double height;                  ///< height of filterbox
  double length;                  ///< depth of filterbox
 
  double angleA;                  ///< angle of first wall [-ve]
  double angleB;                  ///< angle of first wall [+ve]
  double wallThick;               ///< Wall thickness				
  double wallGap;	          ///< void gap round waal

  double beTemp;		  ///< Be Temperature  

  int beMat;                      ///< Be filter material
  int wallMat;                    ///< Wall Be material

  
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  public:

  FilterBox(const std::string&);
  FilterBox(const FilterBox&);
  FilterBox& operator=(const FilterBox&);
  virtual ~FilterBox();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const size_t);
  
};

}

#endif
 
