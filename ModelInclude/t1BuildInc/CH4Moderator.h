/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/CH4Moderator.h
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
#ifndef ts1System_CH4Moderator_h
#define ts1System_CH4Moderator_h

class Simulation;


namespace ts1System
{

/*!
  \class CH4Moderator
  \version 1.0
  \author G Skoro
  \date February 2012
  \brief TS1 CH4Moderator [insert object]
*/

class CH4Moderator : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,public attachSystem::FixedComp
{
 private:
  
  const int ch4Index;            ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< Angle [degrees]

  double width;                 ///< width
  double height;                ///< height
  double depth;                 ///< viewed distance [inner]

  double viewSphere;            ///< View sphere radius

  double innerThick;            ///< Al thickness [inner]
  double vacThick;              ///< Vac layer
  double outerThick;            ///< Al thickness (out)
  double clearThick;            ///< final clearacnce

  double vacTop;              ///< Target vac space
  double outerView;            ///< Al thickness (out)
  double clearTop;            ///< final clearacnce

  double poisonXStep;                 ///< Poison Offset on X to Origin
  double poisonYStep;                 ///< Offset on Y
  double poisonZStep;                 ///< Offset on Z
  double poisonGdThick;             ///< Poison (Gadolinium) thickness
  double poisonAlThick;             ///< Poison (Aluminium) thickness

  int alMat;                    ///< Al
  int ch4Mat;                   ///< ch4
  int voidMat;                  ///< void
  int poisonMat;                  ///< Poison (Gadolinium)
  
  double ch4Temp;                ///< CH4 temperature

  void applyModification(std::map<size_t,double>&) const;
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);


  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  CH4Moderator(const std::string&);
  CH4Moderator(const CH4Moderator&);
  CH4Moderator& operator=(const CH4Moderator&);
  virtual ~CH4Moderator();
//
  std::string getComposite(const std::string&) const;

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
