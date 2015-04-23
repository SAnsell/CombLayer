/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/makeImat.h
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
#ifndef imatSystem_makeImat_h
#define imatSystem_makeImat_h

namespace imatSystem
{

  /*!
    \class makeImat 
    \version 1.0
    \author S. Ansell
    \date June 2011
  */

class makeImat
{
 private:


 public:
  
  makeImat();
  makeImat(const makeImat&);
  makeImat& operator=(const makeImat&);
  ~makeImat();
  
  void build(Simulation* SimPtr,
	     const mainSystem::inputParam& IParam,
	     const shutterSystem::BulkShield&);


};

}

#endif
