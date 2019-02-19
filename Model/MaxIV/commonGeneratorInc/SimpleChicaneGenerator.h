/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/SimpleChicaneGenerator.h
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
#ifndef setVariable_SimpleChicaneGenerator_h
#define setVariable_SimpleChicaneGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SimpleChicaneGenerator
  \version 1.0
  \author S. Ansell
  \date June 2018
  \brief SimpleChicaneGenerator for variables
*/

class SimpleChicaneGenerator
{
 private:
   
  double width;            ///< Full width
  double height;           ///< Full height
  double clearGap;         ///< Clearance gap for pipes [+midwall]
  double upStep;           ///< Clearance gap for vertical

  double plateThick;       ///< Plate thickness
  double barThick;         ///< Support bar thickness / width
  
  std::string plateMat;    ///< main material

 public:

  SimpleChicaneGenerator();
  SimpleChicaneGenerator(const SimpleChicaneGenerator&);
  SimpleChicaneGenerator& operator=(const SimpleChicaneGenerator&);
  ~SimpleChicaneGenerator();

  void setSize(const double,const double,const double);
  void setWall(const double,const std::string&);


  void generateSimpleChicane(FuncDataBase&,const std::string&,
			   const double,const double) const;

};

}

#endif
 
