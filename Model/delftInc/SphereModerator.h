/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/SphereModerator.h
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
#ifndef delftSystem_SphereModerator_h
#define delftSystem_SphereModerator_h

class Simulation;

namespace delftSystem
{

/*!
  \class SphereModerator
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Spherical moderator for Delft [old FRM-1 mod]
*/

class SphereModerator : public virtualMod
{
 private:
  
  const int hydIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double innerRadius;       ///< Inner Radius
  double innerAl;           ///< Inner Al
  double outerRadius;       ///< Outer Radius
  double outerAl;           ///< Outer Al

  double outYShift;         ///< Y shift of outer             
  double fYShift;           ///< Front Y shift

  double capThick;          ///< Front Y shift

  double pipeLen;           ///< Length of pipe
  double pipeRadius;        ///< Pipe inner radius
  double pipeAlRadius;      ///< Pipe outer radius

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  int HCell;                ///< Main H2 cell

  ModelSupport::PipeLine InnerA;       ///< In-going H2
  ModelSupport::PipeLine InnerB;       ///< In-going H2

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  SphereModerator(const std::string&);
  SphereModerator(const SphereModerator&);
  SphereModerator& operator=(const SphereModerator&);
  virtual SphereModerator* clone() const;
  virtual ~SphereModerator();

  int getDividePlane() const;
  int viewSurf() const;
  /// Main body for H2 item
  virtual int getMainBody() const { return HCell; }

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  virtual void postCreateWork(Simulation&);

};

}

#endif
 
