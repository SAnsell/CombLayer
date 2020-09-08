/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/magnetVar.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef linacVar_magnetVar_h
#define linacVar_magnetVar_h

namespace setVariable
{

namespace linacVar
{
  // forward declaration - implemented in linacVariables
  void setBellow26(FuncDataBase&,const std::string&,
		   const double);
  void setFlat(FuncDataBase&,const std::string&,
	       const double,const double);

void Segment32Magnet(FuncDataBase&,const std::string&);
void Segment34Magnet(FuncDataBase&,const std::string&);


}

}
#endif
