/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/ModelSupport.h
*
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef ModelSupport_h
#define ModelSupport_h

/*!
  \namespace ModelSupport
  \author S. Ansell
  \version 1.0
  \date September 2009
  \brief General modelling routines
*/

namespace ModelSupport
{
  std::string getExclude(const int);
  std::string removeOpenPair(const std::string&);

  std::string getComposite(const int,const std::string&);
  std::string getComposite(const surfRegister&,const int,const std::string&);
  std::string getComposite(const surfRegister&,const int,const int,
			   const std::string&);
  std::string getComposite(const surfRegister&,const int,const int,
			   const int,const std::string&);

  std::string getComposite(const surfRegister&,const int,const int);
  std::string getComposite(const surfRegister&,const int,const int,const int);
  std::string getComposite(const surfRegister&,const int,
			   const int,const int,const int);



  std::string getSetComposite(const surfRegister&,const int,
			      const std::string&);
  std::string getSetComposite(const surfRegister&,const int,
			      const int,const std::string&);

}

#endif
