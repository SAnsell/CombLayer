/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/CH4Moderator.h
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
  public attachSystem::LayerComp,
  public attachSystem::FixedOffset
{
 private:
  
  const int ch4Index;            ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double width;                 ///< width
  double height;                ///< height
  double depth;                 ///< viewed distance [inner]

  double viewSphere;            ///< View sphere radius

  double innerThick;            ///< Al thickness [inner]
  double vacThick;              ///< Vac layer
  double outerThick;            ///< Al thickness (out)
  double clearThick;            ///< final clearance

  double vacTop;              ///< Target vac space
  double outerView;            ///< Al thickness (out)
  double clearTop;            ///< final clearance

  double poisonXStep;               ///< Poison Offset on X to Origin
  double poisonYStep;               ///< Offset on Y
  double poisonZStep;               ///< Offset on Z
  double poisonGdThick;             ///< Poison (Gadolinium) thickness
  double poisonAlThick;             ///< Poison (Aluminium) thickness

  int alMat;                    ///< Al
  int ch4Mat;                   ///< ch4
  int voidMat;                  ///< void
  int poisonMat;                  ///< Poison (Gadolinium)
  
  double ch4Temp;                ///< CH4 temperature

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);


  void createSurfaces();
//  void createObjects(Simulation&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  CH4Moderator(const std::string&);
  CH4Moderator(const CH4Moderator&);
  CH4Moderator& operator=(const CH4Moderator&);
  virtual ~CH4Moderator();

  std::string getComposite(const std::string&) const;

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
