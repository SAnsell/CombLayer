/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModelInc/siModerator.h
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
#ifndef lensSystem_siModerator_h
#define lensSystem_siModerator_h

class Simulation;
class FuncDataBase;

/*!
  \namespace LensSystem
  \brief Stuff for the lens system
  \author S. Ansell
  \version 1.0
  \date April 2009
*/

namespace lensSystem
{

/*!
  \class siModerator
  \version 1.0
  \author S. Ansell
  \date Octobert 2009
  \brief Inner construct of lens
 */

class siModerator : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int surIndex;           ///< Index of surface offset
  int cellIndex;                ///< Index of surface offset
  int populated;                ///< populated or not

  double modLength;        ///< Moderator length
  double modWidth;         ///< Moderator width
  double modHeight;        ///< Moderator height
  double topThick;         ///< Al top thickness
  double baseThick;        ///< Al base thickness
  double sideThick;       ///< Al side thickness
  double siThick;          ///< Si thickness
  double polyThick;        ///< poly thickness
  double temp;             ///< Temperature

  int siMat;               ///< Material for silicon
  int polyMat;             ///< Material for Poly
  int surroundMat;         ///< Material surround support


  void populate(const FuncDataBase&);
  void createUnitVector();
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  siModerator(const std::string&);
  siModerator(const siModerator&);
  siModerator& operator=(const siModerator&);
  ~siModerator() {}  ///< Destructor

  void createAll(Simulation&);

  /// lowest Point
  double getDepth() const { return -baseThick-modHeight/2.0; }
};

}


#endif
 
