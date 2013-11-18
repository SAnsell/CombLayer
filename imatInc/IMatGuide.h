/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/IMatGuide.h
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
#ifndef imatSystem_IMatGuide_h
#define imatSystem_IMatGuide_h

class Simulation;

namespace imatSystem
{

/*!
  \class IMatGuide
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief IMatGuide for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class IMatGuide : public attachSystem::ContainedGroup,
    public attachSystem::TwinComp
{
 protected:
  
  const int guideIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on X to Target
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< xyRotation angle
  double zAngle;                ///< zRotation angle

  double length;                ///< Total length
  double height;                ///< Inner void
  double width;                 ///< Inner void

  double glassThick;            ///< Glass layer
  double boxThick;              ///< Box layer

  double voidSide;               ///< Surround void
  double voidBase;               ///< Surround void
  double voidTop;                ///< Surround void

  double feSide;                 ///< Surround void
  double feBase;                 ///< Surround void
  double feTop;                  ///< Surround void

  double wallSide;                ///< Surround void
  double wallBase;                ///< Surround void
  double wallTop;                 ///< Surround void

  int glassMat;                 ///< guide Material 
  int boxMat;                   ///< boxing Material 
  int feMat;                    ///< Fe layer Material 
  int wallMat;                  ///< Wall Material 

  int innerVoid;                ///< Inner void cell

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);
			
  virtual void createSurfaces();
  virtual void createObjects(Simulation&,const attachSystem::FixedComp&);
  virtual void createLinks();

 public:

  IMatGuide(const std::string&);
  IMatGuide(const IMatGuide&);
  IMatGuide& operator=(const IMatGuide&);
  virtual ~IMatGuide();

  // Accessor to inner void cell
  int getInnerVoid() const { return innerVoid; }

  void createAll(Simulation&,const attachSystem::TwinComp&);
		 

};

}

#endif
 
