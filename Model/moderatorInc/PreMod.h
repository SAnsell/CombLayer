/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/PreMod.h
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
#ifndef moderatorSystem_PreMod_h
#define moderatorSystem_PreMod_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class PreMod
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief GPreMod [insert object]
*/

class PreMod : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int preIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int centOrgFlag;              ///< Origin centred / at a point

  double width;             ///< Total Width
  double height;            ///< Total height
  double depth;             ///< Total depth

  double alThickness;       ///< Al wall thickness

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  int divideSurf;           ///< Division surface
  int targetSurf;           ///< Division surface
  
  void populate(const FuncDataBase&);

  void createUnitVector(const attachSystem::FixedComp&,
			const long int,const long int);


  void createSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  PreMod(const std::string&);
  PreMod(const PreMod&);
  PreMod& operator=(const PreMod&);
  virtual ~PreMod();

  void setEdge() { centOrgFlag=0; }  ///< Set Edge Origin
  void setCent() { centOrgFlag=1; }  ///< Set Centre Origin
  /// Setter 
  void setTargetSurf(const int SN) { targetSurf=SN; }  
  /// Setter 
  void setDivideSurf(const int SN) { divideSurf=SN; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const bool);

};

}

#endif
 
