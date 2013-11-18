/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/CH4Section.h
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
#ifndef ts1System_CH4Section_h
#define ts1System_CH4Section_h

class Simulation;


namespace ts1System
{

/*!
  \class CH4Section
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief TS1 CH4Section upgrade
*/

class CH4Section : public ModBase
{
 private:
    
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle [degrees]
  double height;
  double width;
  double depth;                 ///< depth [y direction]

  double frontWall;             ///< Front wall thickness
  double backWall;              ///< Back wall thickness
  double mainWall;              ///< MainWall thickness;

  double vacGap;               ///< Inner vac gap
  double heGap;                ///< Tertiary layer
  double outGap;               ///< Clearance thickness
  double midAlThick;           ///< Mid layer thickness
  double outAlThick;           ///< Mid layer thickness

  double poisonYStep1;                 ///< Offset on Y
  double poisonYStep2;                 ///< Offset on Y  
  double poisonThick;             ///< Poison (Gadolinium) thickness
  double pCladThick;             ///< Poison (Aluminium) thickness

  int alMat;                    ///< Al
  int ch4Mat;                   ///< LCH4
  int poisonMat;                ///< Poison (Gadolinium) 
  int pCladMat;                 ///< Poison Cladding (Al)  

  double ch4Temp;               ///< LCH4 temperature [K]

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  CH4Section(const std::string&);
  CH4Section(const CH4Section&);
  CH4Section& operator=(const CH4Section&);
  virtual CH4Section* clone() const;
  virtual ~CH4Section();
//
  virtual void addToInsertChain(attachSystem::ContainedComp&) const;
//  void createAll(Simulation&,const attachSystem::FixedComp&);

  virtual Geometry::Vec3D 
    getSurfacePoint(const size_t,const size_t) const;
  virtual int 
    getLayerSurf(const size_t,const size_t) const;
  virtual std::string 
    getLayerString(const size_t,const size_t) const;

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
