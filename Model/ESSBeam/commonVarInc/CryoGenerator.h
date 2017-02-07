/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/CryoGenerator.h
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
#ifndef essSystem_CryoGenerator_h
#define essSystem_CryoGenerator_h

class Simulation;

namespace setVariable
{

/*!
  \class CryoGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief CryoGenerator for variables
*/

class CryoGenerator
{
 private:

    
 public:

  CryoGenerator();
  CryoGenerator(const CryoGenerator&);
  CryoGenerator& operator=(const CryoGenerator&);
  ~CryoGenerator();


  void generateFridge(FuncDataBase&,
		      const std::string&,const double,
		      const double,const double);

};

}

#endif
 
