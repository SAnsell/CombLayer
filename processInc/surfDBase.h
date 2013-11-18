/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/surfDBase.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ModelSupport_surfDBase_h
#define ModelSupport_surfDBase_h

namespace ModelSupport
{

/*!
  \class surfDBase
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief Generates a surface based on fraction (0:1)
  
  The transition is from one to many. If the surface is inner, 
  then the fraction is allocated at given, if not then the fraction
  is calculated as 1-frac. 

  The number of surfaces created is detemined by the MergeSurf object. 
*/

class surfDBase
{
 protected:

  static int replaceToken(std::vector<Token>&,const int,
			  const std::vector<Token>&);
  static int replaceTokenWithSign(std::vector<Token>&,const int,
				  const std::vector<Token>&,
				  const std::vector<Token>&);
  static void removeToken(std::vector<Token>&,const int);
 
 public:  

  ///\cond ABSTRACT
  surfDBase() {}                     
  surfDBase(const surfDBase&) {}     
  surfDBase& operator=(const surfDBase&) { return *this; }  
  virtual surfDBase* clone() const =0;
  virtual ~surfDBase() {}            

  virtual void populate() =0;
  virtual int createSurf(const double,int&) =0;
  virtual void processInnerOuter(const int,std::vector<Token>&) const =0;
  ///\endcond ABSTRACT
};


}

#endif
