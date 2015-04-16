/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/ClebschGordan.h
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
#ifndef ClebschGordan_h
#define ClebschGordan_h

namespace mathSupport
{

  /*!
    \class ClebschGordan
    \author S. Ansell
    \date November 2013
    \brief Clebsch-Gordan calculator methods
    \version 1.0
  */
class ClebschGordan
{
 private:

  static double C1(const int,const int,const int,const int,const int);
  static double C2(const int,const int,const int);
  static double C3(const int,const int,const int,
		   const int,const int,const int);
  

 public:

  static double calc(const int,const int,const int,
			   const int,const int);
};

}

#endif
