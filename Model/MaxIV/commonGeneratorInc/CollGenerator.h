/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CollGenerator.h
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
  \date May 2018
  \brief CollGenerator for variables
*/

class CollGenerator
{
 protected:

  
  double radius;           ///< Outer Radius

  double minLength;        ///< length along beam
  double AWidth;           ///< width accross beam [start]
  double minWidth;         ///< width accross beam [mid]
  double BWidth;           ///< width accross beam [end]

  double AHeight;          ///< height accross beam [start]
  double minHeight;        ///< height accross beam [mid]
  double BHeight;          ///< height accross beam [end]

  std::string mat;         ///< main material
  std::string voidMat;     ///< void material
    

 public:

  CollGenerator();
  CollGenerator(const CollGenerator&);
  CollGenerator& operator=(const CollGenerator&);
  virtual ~CollGenerator();

  void setMain(const double,const std::string&,const std::string&);
  void setMinSize(const double,const double,const double);
  void setFrontAngleSize(const double,const double,const double);
  void setMinAngleSize(const double,const double,const double,const double);
  void setBackAngleSize(const double,const double,const double);
  void setFrontGap(const double,const double);
  void setBackGap(const double,const double);

  void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
