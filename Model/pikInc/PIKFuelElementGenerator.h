/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pikInc/PIKFuelElementGenerator.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef setVariable_PIKFuelElementGenerator_h
#define setVariable_PIKFuelElementGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PIKFuelElementGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2021
  \brief PIKFuelElementGenerator for variables
*/

class PIKFuelElementGenerator
{
 private:

  double length;                ///< Length
  double radius;                ///< Radius
  double height;                ///< Height

  std::string mainMat;                  ///< Main material

 public:

  PIKFuelElementGenerator();
  PIKFuelElementGenerator(const PIKFuelElementGenerator&);
  PIKFuelElementGenerator& operator=(const PIKFuelElementGenerator&);
  virtual ~PIKFuelElementGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
