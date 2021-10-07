/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/BoxD2Moderator.h
 *
 * Copyright (c) 2004-2018 by Konstantin Batkov / Stuart Ansell
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
#ifndef essSystem_BoxD2Moderator_h
#define essSystem_BoxD2Moderator_h

class Simulation;

namespace essSystem
{
  class Box;
  //  class EdgeWater;

/*!
  \class BoxD2Moderator
  \author Konstantin Batkov
  \version 2.0
  \date June 2017
  \brief Box moderator
*/

class BoxD2Moderator :
  public EssModBase
{
 private:

  std::shared_ptr<Box> MidD2;        ///< LD2 box in the middle
  std::shared_ptr<Box> MidBeNNBAR;   ///< Be insertion NNBAR
  std::shared_ptr<Box> MidBeOther;   ///< Be insertion opposite side
  // std::shared_ptr<EdgeWater> LeftWater;    ///< Left reflector part
  //  std::shared_ptr<EdgeWater> RightWater;    ///< Right reflector part

  double totalHeight;                  ///< Total height
  double outerRadius;                  ///< Main outer radius
  int outerMat;                        ///< Material of the outer reflector
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,
			const attachSystem::FixedComp&,
			const long int);

  void createExternal();
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BoxD2Moderator(const std::string&);
  BoxD2Moderator(const BoxD2Moderator&);
  BoxD2Moderator& operator=(const BoxD2Moderator&);
  virtual BoxD2Moderator* clone() const;
  virtual ~BoxD2Moderator();

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;
  virtual int getCommonSurf(const long int) const;

  /// Accessor to radius
  void setRadiusX(const double R) { outerRadius=R; }

  virtual const attachSystem::FixedComp&
    getComponent(const std::string&) const;

  std::string getLeftExclude() const;
  std::string getRightExclude() const;
  std::string getLeftFarExclude() const;
  std::string getRightFarExclude() const;

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif

