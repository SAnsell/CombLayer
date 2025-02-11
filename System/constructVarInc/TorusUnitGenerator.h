/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   System/constructInc/TorusUnitGenerator.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef setVariable_TorusUnitGenerator_h
#define setVariable_TorusUnitGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TorusUnitGenerator
  \version 1.0
  \author Konstantin Batkov
  \date November 2024
  \brief torusUnit generator for variables
*/

class TorusUnitGenerator
{
 private:

  double rMinor;           ///< minor radius
  double rMajor;           ///< major radius
  int nFaces;              ///< number of the inscribed polygon faces

  std::string mat;         ///< Material

 public:

  TorusUnitGenerator();
  TorusUnitGenerator(const TorusUnitGenerator&);
  TorusUnitGenerator& operator=(const TorusUnitGenerator&);
  virtual ~TorusUnitGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
