/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/R3ChokeChamberGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

#ifndef setVariable_R3ChokeChamberGenerator_h
#define setVariable_R3ChokeChamberGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class R3ChokeChamberGenerator
  \version 1.0
  \author S. Ansell
  \date April 2019
  \brief R3ChokeChamberGenerator for variables
*/

class R3ChokeChamberGenerator
{
 private:

    

 public:

  R3ChokeChamberGenerator();
  R3ChokeChamberGenerator(const R3ChokeChamberGenerator&);
  R3ChokeChamberGenerator& operator=(const R3ChokeChamberGenerator&);
  ~R3ChokeChamberGenerator();


  void setRadius(const double);
  // L/T/W
  void setPlate(const double,const double,const double);

  void setMaterial(const std::string&,const std::string&);
  void setSupport(const double,const double,
		  const double,const double);
  
  void generateChamber(FuncDataBase&,const std::string&) const

};

}

#endif
 
