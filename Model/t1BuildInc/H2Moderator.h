/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/H2Moderator.h
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
#ifndef ts1System_H2Moderator_h
#define ts1System_H2Moderator_h

class Simulation;


namespace ts1System
{

/*!
  \class H2Moderator
  \version 1.0
  \author G Skoro
  \date May 2012
  \brief TS1 H2Moderator [insert object]
*/

class H2Moderator : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,public attachSystem::FixedComp
{
 private:
  
  const int h2Index;            ///< Index of surface offset
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
  double midThick;              ///< Mid layer
  double terThick;              ///< Ter layer 
  double outerThick;            ///< Al thickness (out)
  double clearThick;            ///< final clearacnce

  double vacTop;              ///< Target vac space
  double outerView;            ///< Al thickness (out)
  double clearTop;            ///< final clearacnce

  int alMat;                    ///< Al
  int lh2Mat;                   ///< LH2
  int voidMat;                  ///< void

  double h2Temp;                ///< H2 temperature

  std::map<size_t,double> modLayer;  ///< Surface modification layer

  void applyModification();
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);


  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  H2Moderator(const std::string&);
  H2Moderator(const H2Moderator&);
  H2Moderator& operator=(const H2Moderator&);
  virtual ~H2Moderator();
//
  std::string getComposite(const std::string&) const;
  virtual void addToInsertChain(attachSystem::ContainedComp&) const;
//  void createAll(Simulation&,const attachSystem::FixedComp&);
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
