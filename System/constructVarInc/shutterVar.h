/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/shutterVar.h
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
#ifndef ts1System_shutterVar_h
#define ts1System_shutterVar_h

class FuncDataBase;

namespace ts1System
{

/*!
  \class shutterVar
  \version 1.0
  \author S. Ansell
  \date February 2013
  \brief Sets the variables for the shutter components
*/

class shutterVar
{
 private:

  std::string keyName;       ///< Shutter key name

  size_t NSections;      ///< Number of sections required
  size_t steelNumber;    ///< Number of steel blocks per section
  double colletHGap;     ///< Collet gap horrizontal
  double colletVGap;     ///< Collet gap vertial
  double colletFGap;     ///< Collet gap forward
  std::string colletMat;         ///< Collet material

  double b4cThick;       ///< Total b4c thick
  double b4cSpace;       ///< Total b4c spacer

  /// Steel Size [width:height]
  std::vector< std::pair<double,double> > steelSize;

  
  double blockVertGap;   ///< Steel block size
  double blockHorGap;    ///< Steel horriontal size

  /// set steel number [cant be zero]
  void setSteelNumber(const size_t I) { steelNumber=(I) ? I : 4; } 
  double B4Cspacer(FuncDataBase&,const size_t,
		   const double,const double,
		   const double,const double) const;

  double B4Cspacer(FuncDataBase&,const size_t,
		   const double,const double,
		   const double) const;

  
  double SteelBlock(FuncDataBase&,const size_t,const double,
		    const double,const double,const double,
		    const double,const double,const std::string&,
		    const size_t) const;

  void header(FuncDataBase&,const double,const double,
	      const double) const;
    
 public:

  shutterVar(const std::string&);
  shutterVar(const shutterVar&);
  shutterVar& operator=(const shutterVar&);
  ~shutterVar() {}   ///< Destructor

  void setSteelOffsets(const size_t,const size_t,
		       const double,const double);

  void buildVar(FuncDataBase&,
		const double,const double,
		const double,const double,
		const double,const double,
		const double) const;

  void buildCylVar(FuncDataBase&,
		   const double,const double,
		   const double,const double,
		   const double) const;

};

}  

#endif
