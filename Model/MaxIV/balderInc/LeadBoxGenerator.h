/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/MirrorGenerator1.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_LeadBoxGenerator_h
#define setVariable_LeadBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class LeadBoxGenerator
  \version 1.0
  \author S. Ansell
  \date July 2018
  \brief LeadBoxGenerator for variables
*/

class LeadBoxGenerator
{
 private:

  double height;            ///< void height [top only]
  double depth;             ///< void depth [low only]
  double width;             ///< void width [total]
  double length;            ///< void length [total]

  double wallThick;         ///< wall thickness
  double portGap;           ///< Clearance to port

  double plateWidth;        ///< Pipe cover plate
  double plateHeight;       ///< Pipe Height
  double plateThick;        ///< Pipe thickness

  std::string voidMat;      ///< void material 
  std::string wallMat;      ///< Wall material layer
    

 public:

  LeadBoxGenerator();
  LeadBoxGenerator(const LeadBoxGenerator&);
  LeadBoxGenerator& operator=(const LeadBoxGenerator&);
  ~LeadBoxGenerator();

  void setMain(const double,const double);
  void setPlate(const double,const double,const double);
  
  void generateBox(FuncDataBase&,const std::string&,
		   const double,const double) const;

};

}

#endif
 
