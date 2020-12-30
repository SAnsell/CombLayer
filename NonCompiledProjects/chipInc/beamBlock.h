/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/beamBlock.h
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
#ifndef hutchSystem_beamBlock_h
#define hutchSystem_beamBlock_h

class FuncDataBase;

namespace hutchSystem
{

/*!
  \struct beamBlock
  \version 1.0
  \author S. Ansell
  \date October 2010
  \brief Blocks in the shutter to improve attenuation
*/

struct beamBlock
{
  static const size_t Size;   ///< Size of a beamstop group

  double thickness;        ///< length of unit 
  double angle;            ///< angle of unit
  int matN;                ///< Material number  

  int setFromControl(const FuncDataBase&,const std::string&,
		     const size_t,const size_t);
  void setVar(const FuncDataBase&,const size_t,const std::string&);

};

}  

#endif
