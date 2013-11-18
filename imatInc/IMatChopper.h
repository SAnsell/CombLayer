/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/IMatChopper.h
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
#ifndef imatSystem_IMatChopper_h
#define imatSystem_IMatChopper_h

class Simulation;

namespace imatSystem
{

/*!
  \class IMatChopper
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief IMatChopper (ideally for both)
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class IMatChopper : public attachSystem::ContainedComp,
    public attachSystem::TwinComp
{
 private:
  
  const int chopIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on X to Target
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< xyRotation angle
  double zAngle;                ///< zRotation angle

  double length;                ///< Total length of void 
  double height;                ///< Inner void upper
  double depth;                 ///< Inner void base
  double left;                  ///< Inner void left
  double right;                 ///< Inner Right
  double width;                 ///< Inner void

  double feWidth;                 ///< Surround wdith
  double feFront;                 ///< Surround steel (away from Mod)
  double feBack;                  ///< Surround steel (near mod)
  double feBase;                  ///< Surround steel
  double feTop;                   ///< Surround top steel

  double wallWidth;                 ///< Surround wdith
  double wallFront;                 ///< Surround wall (away from Mod)
  double wallBack;                  ///< Surround wall (near mod)
  double wallBase;                  ///< Surround wall
  double wallTop;                   ///< Surround top wall

  int feMat;                    ///< Fe layer Material 
  int wallMat;                  ///< Wall Material 

  int innerVoid;                ///< Inner void cell

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);
			
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  IMatChopper(const std::string&);
  IMatChopper(const IMatChopper&);
  IMatChopper& operator=(const IMatChopper&);
  virtual ~IMatChopper();

  // Accessor to inner void cell
  int getInnerVoid() const { return innerVoid; }

  void createAll(Simulation&,const attachSystem::TwinComp&);
		 

};

}

#endif
 
