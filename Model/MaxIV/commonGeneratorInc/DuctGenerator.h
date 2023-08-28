/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/DuctGenerator.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef setVariable_DuctGenerator_h
#define setVariable_DuctGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DuctGenerator
  \version 1.0
  \author Konstantin Batkov
  \date August 2023
  \brief DuctGenerator for variables
*/

class DuctGenerator
{
 private:

  double length;                ///< Duct length
  double width;                 ///< Width
  double height;                ///< Height
  double radius;                 ///< Radius (if circular)

  int voidMat;                  ///< Void material

 public:

  DuctGenerator();
  DuctGenerator(const DuctGenerator&);
  DuctGenerator& operator=(const DuctGenerator&);
  virtual ~DuctGenerator();

  virtual void generate(FuncDataBase&,const std::string&,
			const double&,const double&,const double&,const double&) const;

};

}

#endif
