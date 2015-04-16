/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muonInc/muonQ1.h
*
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_muonQ1_h
#define muSystem_muonQ1_h

class Simulation;

namespace muSystem
{

/*!
  \class muonQ1
  \author G. Skoro
  \version 1.0
  \date October 2013
  \brief Q11 type quadrupole object
*/

class muonQ1 : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int muQ1Index;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step
  double xAngle;               ///< Angle (rotation)
  double yAngle;               ///< Angle (rotation)  
  double zAngle;               ///< Angle (rotation)  
  double xSize;                 ///< Left/Right size
  double ySize;                 ///< length down target
  double zSize;                 ///< Vertical size
  double cutLenOut;                ///< Distance cut back (on axis)
  double cutLenMid;                ///< Distance cut back (on axis)

  double steelThick;                 ///< Steel thickness
  double copperThick;                 ///< Copper thickness 
  double copperYSize;                 ///< Copper length   
     
  double insertSize;                 ///< Insert size   
  double insertThick;                 ///< Insert thickness
    
  int steelMat;                   ///Material: Steel
  int copperMat;                   ///Material: Copper 
  int insertMat;                   ///Material: Insert 

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  muonQ1(const std::string&);
  muonQ1(const muonQ1&);
  muonQ1& operator=(const muonQ1&);
  virtual ~muonQ1();
  
  void createAll(Simulation&,const attachSystem::FixedComp&);  
};

}

#endif
 
