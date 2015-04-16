/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   constructInc/GroupOrigin.h
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
#ifndef constructSystem_GroupOrigin_h
#define constructSystem_GroupOrigin_h

namespace constructSystem
{

/*!
  \class GroupOrigin
  \version 1.0
  \author S. Ansell
  \date February 2013
  \brief Group origin for two or more objects
*/

class GroupOrigin : public attachSystem::FixedComp
{
 private:
  
  const int grpIndex;           ///< Index of surface offset
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< Angle [degrees]
  double zAngle;                ///< Angle [degrees]

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

 public:

  GroupOrigin(const std::string&);
  GroupOrigin(const GroupOrigin&);
  GroupOrigin& operator=(const GroupOrigin&);
  virtual ~GroupOrigin();
//

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
