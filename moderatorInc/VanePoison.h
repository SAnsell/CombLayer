/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/VanePoison.h
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
#ifndef moderatorSystem_VanePoison_h
#define moderatorSystem_VanePoison_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class VanePoison
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief VanePoison [insert object]
*/

class VanePoison : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
  private:
  
  const int vaneIndex;      ///< Index of surface offset
  int cellIndex;            ///< Cell index

  size_t nBlades;           ///< number of blades
  double bWidth;            ///< Width of a blade
  double absThick;          ///< Thickness of abs [in blade]
  double bGap;              ///< Space between blades
  double yLength;           ///< Length into the moderator
  double zLength;           ///< Height of a blade
  
  double xOffset;           ///< Offset relative to origin 
  double yOffset;           ///< Offset relative to origin 
  double zOffset;           ///< Offset relative to origin 

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int bladeMat;               ///< Moderator material
  int absMat;               ///< Moderator material

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,
			const size_t);

  void createLinks();
  void createSurfaces();
  void createObjects(Simulation&,
		     const attachSystem::FixedComp&,
		     const size_t);

 public:

  VanePoison(const std::string&);
  VanePoison(const VanePoison&);
  VanePoison& operator=(const VanePoison&);
  /// Clone function
  virtual VanePoison* clone() const { return new VanePoison(*this); }
  virtual ~VanePoison();

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const size_t);

};

}

#endif
 
