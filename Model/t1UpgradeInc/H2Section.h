/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/H2Section.h
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
#ifndef ts1System_H2Section_h
#define ts1System_H2Section_h

class Simulation;


namespace ts1System
{

/*!
  \class H2Section
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief TS1 H2Section upgrade
*/

class H2Section : public constructSystem::ModBase
{
 private:

  double height;                ///< Height of moderator
  double width;                 ///< width of the moderator
  double depth;                 ///< depth [y direction]

  double frontWall;             ///< Front wall thickness
  double backWall;              ///< Back wall thickness
  double mainWall;              ///< MainWall thickness;

  double vacGap;               ///< Inner vac gap
  double heGap;                ///< Tertiary layer
  double outGap;               ///< Clearance thickness
  double midAlThick;           ///< Mid layer thickness
  double outAlThick;           ///< Mid layer thickness

  int nSi;                     ///< Number of Si
  double siThick;              ///< Thickness of Si

  int alMat;                    ///< Al
  int siMat;                    ///< Al
  int lh2Mat;                   ///< LH2
  double lh2Temp;               ///< LH2 temperature [K]

  void populate(const FuncDataBase&);

  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  H2Section(const std::string&);
  H2Section(const H2Section&);
  H2Section& operator=(const H2Section&);
  virtual ~H2Section();
  virtual H2Section* clone() const;

  virtual void addToInsertChain(attachSystem::ContainedComp&) const;

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp*,const long int);

};

}

#endif
 
