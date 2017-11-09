/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/ConeModerator.h
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
#ifndef delftSystem_ConeModerator_h
#define delftSystem_ConeModerator_h

class Simulation;

namespace delftSystem
{

/*!
  \class ConeModerator
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief ConeModerator [insert object]
*/

class ConeModerator : public virtualMod
{
 private:
  
  const int coneIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double depth;             ///< apex-apex depth of H2
  double length;            ///< inner apex - cut plane 
  double innerAngle;        ///< Inner angle of cone
  double outerAngle;        ///< Outer angle of cone
  double fCut;              ///< Front cut
  double fDepth;            ///< Front layer depth
  
  double alView;            ///< Thickness of AL view 
  double alBack;            ///< Thickness of AL back
  double faceThick;         ///< Outer face thickness (at length)

  double modTemp;           ///< Moderator temperature
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  int HCell;                ///< Main H2 cell
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ConeModerator(const std::string&);
  ConeModerator(const ConeModerator&);
  ConeModerator& operator=(const ConeModerator&);
  virtual ConeModerator* clone() const;
  virtual ~ConeModerator();

  /// Access to hydrogen region
  virtual int getMainBody() const { return HCell; }

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  virtual void postCreateWork(Simulation&);
};

}

#endif
 
