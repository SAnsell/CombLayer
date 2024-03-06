/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/IonChamberGenerator.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef setVariable_IonChamberGenerator_h
#define setVariable_IonChamberGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class IonChamberGenerator
  \version 1.0
  \author Konstantin Batkov
  \date March 2024
  \brief IonChamberGenerator for variables
*/

class IonChamberGenerator
{
 private:

  double radius;                ///< Moderator radius
  double height;                ///< Height

 public:

  IonChamberGenerator();
  IonChamberGenerator(const IonChamberGenerator&);
  IonChamberGenerator& operator=(const IonChamberGenerator&);
  virtual ~IonChamberGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
