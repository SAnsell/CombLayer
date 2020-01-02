/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/Undulator.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_Undulator_h
#define xraySystem_Undulator_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class Undulator
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Undulator magnetic chicane

  Built around the central beam axis
*/

class Undulator :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double vGap;                    ///< Vertical gap
  
  double length;                  ///< Main length
  double magnetWidth;              ///< Block [quad unit] width
  double magnetDepth;              ///< Depth of unit

  double sVOffset;                  ///< suppor offset in z
  double supportThick;              ///< support thick [z axis]
  double supportWidth;              ///< support width [x axis]
  double supportLength;             ///< Support length [y axis]

  double standHeight;                ///< stand depth [z] 
  double standWidth;                ///< support thick [z axis]

  int voidMat;                    ///< Void material
  int magnetMat;                   ///< Block material
  int supportMat;                 ///< support material
  int standMat;                   ///< stand material
  
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Undulator(const std::string&);
  Undulator(const Undulator&);
  Undulator& operator=(const Undulator&);
  virtual ~Undulator();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
