/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/InnerLayer.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef ts1System_InnerLayer_h
#define ts1System_InnerLayer_h

class Simulation;


namespace ts1System
{

/*!
  \class InnerLayer
  \version 1.0
  \author S. Ansell
  \date Septeber 2014
  \brief TS1 InnerLayer upgrade
*/

class InnerLayer : public ts1System::CH4Layer
{
 private:

  const std::string IKeyName;     ///< Inner key name
  const int innerIndex;           ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t nPoison;                  ///< Number of poison
  std::vector<double> poisonYStep; ///< Y centre of poison layer
  double poisonThick;              ///< Poison thickness
  int poisonMat;                   ///< Poison (Gadolinium) 
  double pCladThick;               ///< Poison thickness
  int pCladMat;                    ///< Poison Cladding (Al)  

  std::vector<int> innerCells;     ///< Cells that need inner reference

  
  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  InnerLayer(const std::string&,const std::string&);
  InnerLayer(const InnerLayer&);
  InnerLayer& operator=(const InnerLayer&);
  virtual InnerLayer* clone() const;
  virtual ~InnerLayer();

  virtual Geometry::Vec3D 
    getSurfacePoint(const size_t,const size_t) const;
  virtual int 
    getLayerSurf(const size_t,const size_t) const;
  virtual std::string 
    getLayerString(const size_t,const size_t) const;

  /// Const accessor
  const std::vector<int>& getInnerCells() const
    { return innerCells; }

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp*,const long int);

};

}

#endif
 
