/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   System/constructInc/TorusGenerator.h
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
#ifndef setVariable_TorusGenerator_h
#define setVariable_TorusGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TorusGenerator
  \version 1.0
  \author Konstantin Batkov
  \date November 2024
  \brief TorusGenerator for variables
*/

class TorusGenerator
{
 private:

  double rMinor;            ///< minor radius

  std::string mat;          ///< Material

 public:

  TorusGenerator();
  TorusGenerator(const TorusGenerator&);
  TorusGenerator& operator=(const TorusGenerator&);
  virtual ~TorusGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
