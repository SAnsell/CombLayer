/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/makeIMat.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef imatSystem_makeIMat_h
#define imatSystem_makeIMat_h

namespace imatSystem
{

  class IMatGuide;
  class IMatChopper;

  /*!
    \class makeIMat 
    \version 1.0
    \author S. Ansell
    \date June 2011
    \brief Construct IMat from Insert to beamstop
  */

class makeIMat
{
 private:
  
  std::shared_ptr<IMatGuide> G1Obj;    ///< First Guide section
  //  std::shared_ptr<IMatChopper> TZero;  ///< Chopper pit
  //  std::shared_ptr<IMatGuide> G2Ibj;    ///< Second Guide section

 public:
  
  makeIMat();
  makeIMat(const makeIMat&);
  makeIMat& operator=(const makeIMat&);
  ~makeIMat();
  
  void build(Simulation* SimPtr,
	     const mainSystem::inputParam& IParam,
	     const shutterSystem::BulkShield&);


};

}

#endif
