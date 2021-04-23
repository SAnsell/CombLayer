/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/SPFCameraShieldGenerator.h
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
#ifndef setVariable_SPFCameraShieldGenerator_h
#define setVariable_SPFCameraShieldGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SPFCameraShieldGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2021
  \brief SPFCameraShieldGenerator for variables
*/

class SPFCameraShieldGenerator
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double wallThick;             ///< Wall [brick] thickness
  double roofLength;            ///< Roof [brick] length
  double roofAngle;             ///< Roof angle around z axis
  double roofXShift;            ///< Roof x-offset
  double roofYShift;            ///< Roof y-offset

  std::string mat;              ///< Shield material

 public:

  SPFCameraShieldGenerator();
  SPFCameraShieldGenerator(const SPFCameraShieldGenerator&);
  SPFCameraShieldGenerator& operator=(const SPFCameraShieldGenerator&);
  virtual ~SPFCameraShieldGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
