/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/ChopperPit.h
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
#ifndef imatSystem_ChopperPit_h
#define imatSystem_ChopperPit_h

class Simulation;

namespace imatSystem
{

/*!
  \class ChopperPit
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief ChopperPit for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class ChopperPit : public attachSystem::ContainedComp,
    public attachSystem::TwinComp
{
 protected:
  
  const int pitIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on X to Target
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< xyRotation angle
  double zAngle;                ///< zRotation angle

  double length;                ///< Total length
  double height;                ///< Inner void
  double width;                 ///< Inner void

  double feSide;                ///< Surround void
  double feBase;                ///< Surround void
  double feTop;                 ///< Surround void
  double feBack;                 ///< Surround void
  double feFront;                 ///< Surround void


  double wallSide;                ///< Surround void
  double wallBase;                ///< Surround void
  double wallTop;                 ///< Surround void
  double wallBack;                 ///< Surround void
  double wallFront;                 ///< Surround void

  int feMat;                  ///< Wall Material 
  int wallMat;                  ///< Wall Material 

  /// These are the insert rules
  std::map<std::string,std::string> insertMaps;

  int innerVoid;                ///< Inner void cell

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);
			
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ChopperPit(const std::string&);
  ChopperPit(const ChopperPit&);
  ChopperPit& operator=(const ChopperPit&);
  virtual ~ChopperPit();

  // Accessor to inner void cell
  int getInnerVoid() const { return innerVoid; }
  
  void registerInsert(const std::string&,const std::string&,
		      const attachSystem::ContainedGroup&);

  void createAll(Simulation&,const attachSystem::TwinComp&);
		 

};

}

#endif
 
