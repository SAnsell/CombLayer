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
#ifndef setVariable_CollGenerator_h
#define setVariable_CollGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CollGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief CollGenerator for variables
*/

class CollGenerator
{
 private:

  
  double radius;           ///< Radius

  double minLength;        ///< length along beam
  double AWidth;           ///< width accross beam
  double minWidth;         ///< width accross beam
  double BWidth;           ///< width accross beam

  double AHeight;          ///< height accross beam
  double minHeight;        ///< height accross beam
  double BHeight;          ///< height accross beam



  std::string mat;         ///< main material
  std::string voidMat;     ///< void material
    

 public:

  CollGenerator();
  CollGenerator(const CollGenerator&);
  CollGenerator& operator=(const CollGenerator&);
  ~CollGenerator();


  void setMain(const double,const std::string&,const std::string&);
  void setMinSize(const double,const double,const double);
  void setAPort(const double,const double);
  void setBPort(const double,const double);
  
  void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
