/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/DiskBlades.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef constructSystem_DiskBlades_h
#define constructSystem_DiskBlades_h

class Simulation;

namespace constructSystem
{

/*!
  \struct DiskBlades
  \version 1.0
  \author S. Ansell
  \date June 2015
  \brief DiskBlade basic info
*/

class DiskBlades
{
private:
  
  size_t nBlades;                          ///< Number of blades
  std::vector<double> phaseAngle;          ///< Phase angle [deg]
  std::vector<double> openAngle;           ///< Opening angle [deg]

public:

  double thick;               ///< Thickness of disk
  double innerThick;          ///< Inner Thickness of disk
  int innerMat;               ///< Inner material [non-viewed]
  int outerMat;               ///< Outer material [viewed]

  DiskBlades();
  DiskBlades(const DiskBlades&);
  DiskBlades& operator=(const DiskBlades&);
  ~DiskBlades() {};           ///< Destructor

  void addOpen(const double,const double);

  /// Number of  openings in blade
  size_t getNPhase() const { return nBlades; }
  /// getter for thickness
  double getThick() const { return thick; }
  /// getter for inner mat
  int getInnerMat() const { return innerMat; }
  /// getter for outer mat
  int getOuterMat() const { return outerMat; }

  double getOpenAngle(const size_t) const;
  double getCloseAngle(const size_t) const;
};

}

#endif
 
