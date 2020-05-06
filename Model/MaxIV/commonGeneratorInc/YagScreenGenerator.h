/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/YagScreenGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_YagScreenGenerator_h
#define setVariable_YagScreenGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class YagScreenGenerator
  \version 1.0
  \author Konstantin Batkov
  \date May 2020
  \brief YagScreenGenerator for variables
*/

class YagScreenGenerator
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double wallThick;             ///< Wall thickness

  int mainMat;                  ///< Main material
  int wallMat;                  ///< Wall material

 public:

  YagScreenGenerator();
  YagScreenGenerator(const YagScreenGenerator&);
  YagScreenGenerator& operator=(const YagScreenGenerator&);
  virtual ~YagScreenGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
