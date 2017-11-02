/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trexInc/beamlineSupport.h
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
#ifndef essSystem_beamlineSupport_h
#define essSystem_beamlineSupport_h

class Simulation;


namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
  class FixedGroup;
}


namespace essSystem
{
  class GuideItem;
}  

/*!
    \namespace essBeamSystem
    \version 1.0
    \author S. Ansell
    \date August 2017
    \brief beamlineSupport for ESS
  */



namespace essBeamSystem
{
  
void
setBeamAxis(attachSystem::FixedOffset&,const FuncDataBase&,
	    const attachSystem::FixedGroup&,const int);

}

#endif

