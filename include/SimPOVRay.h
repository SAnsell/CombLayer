/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/SimPOVRay.h
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#ifndef SimPOVRay_h
#define SimPOVRay_h

/*!
  \class SimPOVRay
  \brief Modifides Simulation to output a POV-Ray input file
  \author Konstantin Batkov
  \version 1.0
  \date February 2017
 */
class SimPOVRay : public Simulation
{
 private:

  // ALL THE sub-write stuff
  void writeCells(std::ostream&) const;
  void writeSurfaces(std::ostream&) const;
  void writeMaterial(std::ostream&) const;
  
 public:
  
  SimPOVRay();
  SimPOVRay(const SimPOVRay&);
  SimPOVRay& operator=(const SimPOVRay&);
  virtual ~SimPOVRay() {}           ///< Destructor

  virtual void write(const std::string&) const;

};

#endif
