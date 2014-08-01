/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuildInc/makeEPB.h
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
#ifndef epbSystem_makeEPB_h
#define epbSystem_makeEPB_h

/*!
  \namespace epbSystem
  \brief General EPB stuff
  \version 1.0
  \date September 2013
  \author S. Ansell
*/

namespace epbSystem
{
  class Building;
  class EPBline;
  class Magnet;
  /*!
    \class makeEPB
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief WheeModerator for EPB 
  */
  
class makeEPB
{
 private:
  
  std::shared_ptr<Building> Hall;                 ///< Main proton line
  std::shared_ptr<EPBline> LineVoid;                 ///< Main proton line
  std::vector<std::shared_ptr<Magnet> > BendMags;    ///< Bending magnets
  std::vector<std::shared_ptr<Magnet> > FocusMags;   ///< Focus hexapoles

  void makeMagnets(Simulation&);

 public:
  
  makeEPB();
  makeEPB(const makeEPB&);
  makeEPB& operator=(const makeEPB&);
  ~makeEPB();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
