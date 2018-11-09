/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/SteppedPlateUnit.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell / Konstantin Batkov
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
#ifndef beamlineSystem_SteppedPlateUnit_h
#define beamlineSystem_SteppedPlateUnit_h

namespace beamlineSystem
{

/*!
  \class SteppedPlateUnit
  \version 1.0
  \author K. Batkov
  \date Nov 2018
  \brief Plate unit with possible steps
*/

class SteppedPlateUnit : public PlateUnit
{
 private:

  size_t nSegments;        ///< Number of step segments

 public:

  SteppedPlateUnit(const int,const int);
  SteppedPlateUnit(const SteppedPlateUnit&);
  SteppedPlateUnit& operator=(const SteppedPlateUnit&);
  virtual SteppedPlateUnit* clone() const;
  virtual ~SteppedPlateUnit() {;}

  void clear();

  virtual std::string getString(const ModelSupport::surfRegister&,
				const size_t) const;

  //   with steps
  virtual void createSurfaces(ModelSupport::surfRegister&,
			      const std::vector<double>&,
			      const double&,const std::vector<double>&);
};


}

#endif
 
