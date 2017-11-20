/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/HPreMod.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ts1System_HPreMod_h
#define ts1System_HPreMod_h

class Simulation;

namespace ts1System
{

/*!
  \class HPreMod
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief Pre-mod for H2 (wrap around)
*/

class HPreMod : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:
  
  const int preIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  int centOrgFlag;              ///< Origin centred / at a point

  double lSideThick;            ///< Left side of water thickness
  double rSideThick;            ///< Right side of water thickness
  double topThick;              ///< Total height
  double baseThick;             ///< Total height
  double backThick;             ///< Total depth

  double alThick;        ///< Al wall thickness
  double vacThick;       ///< Vac wall thickness

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material

  /// Points to connect to
  std::vector<Geometry::Vec3D> sidePts;
  /// Axis of connection
  std::vector<Geometry::Vec3D> sideAxis;
  
  void getConnectPoints(const attachSystem::FixedComp&,
			const long int);

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);
  void createLinks();

 public:

  HPreMod(const std::string&);
  HPreMod(const HPreMod&);
  HPreMod& operator=(const HPreMod&);
  HPreMod* clone() const;
  virtual ~HPreMod();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
