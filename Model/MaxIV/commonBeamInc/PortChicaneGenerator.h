/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/MirrorGenerator1.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef setVariable_PortChicaneGenerator_h
#define setVariable_PortChicaneGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PortChicaneGenerator
  \version 1.0
  \author S. Ansell
  \date June 2018
  \brief PortChicaneGenerator for variables
*/

class PortChicaneGenerator
{
 private:
   
  double width;            ///< Full width
  double height;           ///< Full height
  double clearGap;         ///< Clearance gap each side [+midwall]

  double skinThick;        ///< Skin thickness
  double plateThick;       ///< Plate thickness
  
  double wallThick;        ///< Wall thicness

  std::string plateMat;         ///< main material
  std::string wallMat;          ///< wall material

 public:

  PortChicaneGenerator();
  PortChicaneGenerator(const PortChicaneGenerator&);
  PortChicaneGenerator& operator=(const PortChicaneGenerator&);
  ~PortChicaneGenerator();

  void setSize(const double,const double,const double);
  void setPlate(const double,const double,const std::string&);
  void setWall(const double,const std::string&);


  void generatePortChicane(FuncDataBase&,const std::string&,
			   const double,const double) const;

};

}

#endif
 
