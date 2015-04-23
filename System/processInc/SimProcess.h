/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/SimProcess.h
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
#ifndef SimProcess_SimProcess_h
#define SimProcess_SimProcess_h

class Simulation;
class FuncDataBase;

/*!
  \namespace SimProcess
  \version 1.0
  \author S. Ansell
  \date September 2008
  \brief Process functions to act on a Simuation instance
 */

namespace SimProcess
{

  void writeMany(Simulation&,const std::string&,const int);
  void writeIndexSim(Simulation&,const std::string&,const int);
  void writeIndexSimPHITS(Simulation&,const std::string&,const int);

  template<typename T>
  T getDefVar(const FuncDataBase&,const std::string&,const T&);

  template<typename T,typename U>
  T getDefIndexVar(const FuncDataBase&,const std::string&,
		   const std::string&,const U&,const T&);
  template<typename T,typename U>
  T getIndexVar(const FuncDataBase&,const std::string&,
		const std::string&,const U&);

  template<typename T>
  int getIndexVec(const FuncDataBase&,const std::string&,
		  const int,const std::string&,const std::vector<T>&);

  template<typename T>
  std::vector<T> getVarVec(const FuncDataBase&,const std::string&);
  
  void registerOuter(Simulation&,const int,const int);

}  // namespace SimProcess

#endif
