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

  // zeroX adjusted so that distance to KG wall is 1 m
  const double zeroX(275.0);   // coordiated offset to master
  const double zeroY(481.0);    // drawing README.pdf
  const Geometry::Vec3D zeroOffset(zeroX,zeroY,0.0);

  // forward declaration - implemented in linacVariables
  void setBellow26(FuncDataBase&,const std::string&,
		   const double);
  void setBellow37(FuncDataBase&,const std::string&,
		   const double =16.0);

  void setFlat(FuncDataBase&,const std::string&,
	       const double,const double,const double =0);

  void Segment1Magnet(FuncDataBase&,const std::string&);
  void Segment5Magnet(FuncDataBase&,const std::string&);
  void Segment12Magnet(FuncDataBase&,const std::string&); 
  void Segment29Magnet(FuncDataBase&,const std::string&);
  void Segment32Magnet(FuncDataBase&,const std::string&);

}

}
#endif
