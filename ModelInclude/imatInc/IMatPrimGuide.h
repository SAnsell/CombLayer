/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/IMatPrimGuide.h
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
#ifndef imatSystem_IMatPrimGuide_h
#define imatSystem_IMatPrimGuide_h

class Simulation;

namespace imatSystem
{

/*!
  \class IMatPrimGuide
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief IMatPrimGuide for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class IMatPrimGuide : public IMatGuide
{
 protected:

  double feCut;                  ///< Inner cut (left/right)
  double feCutLen;               ///< Inner cut (left/right)

  double wallCut;                 ///< Inner cut (left/right)
  double wallCutLen;              ///< Inner cut (left/right)

  void populate(const Simulation&);
			
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&);
  void createLinks();

 public:

  IMatPrimGuide(const std::string&);
  IMatPrimGuide(const IMatPrimGuide&);
  IMatPrimGuide& operator=(const IMatPrimGuide&);
  virtual ~IMatPrimGuide();

  virtual void createAll(Simulation&,const attachSystem::TwinComp&);
		 
};

}

#endif
 
