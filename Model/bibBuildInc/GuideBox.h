/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/GuideBox.h
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
#ifndef bibSystem_bibGuideBox_h
#define bibSystem_bibGuideBox_h

class Simulation;

namespace bibSystem
{

/*!
  \class GuideBox
  \author R. Vivanco
  \version 1.0
  \date April 2013
  \brief Specialized for GuideBox
*/

class GuideBox : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int guideIndex;           ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double width;                   ///< width of GuideBox
  double height;                  ///< height of GuideBox
  double length;                  ///< depth of GuideBox

  double NiRadius;                ///< Start length of the Ni
  double NiThickness;             ///< Ni thickness
  int mat;
    
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);
  void createLinks();

  public:

  GuideBox(const std::string&);
  GuideBox(const GuideBox&);
  GuideBox& operator=(const GuideBox&);
  virtual ~GuideBox();
  
  void createAll(Simulation&, 
		 const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::FixedComp&,
		 const long int);

  
};

}

#endif
 
