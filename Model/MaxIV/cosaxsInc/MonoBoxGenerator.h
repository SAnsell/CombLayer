/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/MonoBoxGenerator.h
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
#ifndef setVariable_MonoBoxGenerator_h
#define setVariable_MonoBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MonoBoxGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief MonoBoxGenerator for variables
*/

class MonoBoxGenerator : public VacBoxGenerator
{
 private:

  double overHang;      ///< Roof/base overhang
  double baseThick;     ///< Base thickness [half]
  double roofThick;     ///< Roof thickness [half]
   
 public:

  MonoBoxGenerator();
  MonoBoxGenerator(const MonoBoxGenerator&);
  MonoBoxGenerator& operator=(const MonoBoxGenerator&);
  virtual ~MonoBoxGenerator();

  void setLids(const double,const double,const double);
  
  virtual void generateBox(FuncDataBase&,const std::string&,
			   const double,const double,const double,
			   const double,const double) const;

};

}

#endif
 
