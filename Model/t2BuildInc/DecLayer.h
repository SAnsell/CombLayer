/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ts2BuildInc/DecLayer.h
*
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef moderatorSystem_DecLayer_h
#define moderatorSystem_DecLayer_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class DecLayer
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief DecLayer [insert object]
*/

class DecLayer : public Decoupled
{
 private:

  std::string lkeyName;               ///< This key
  
  std::vector<double> lThick;         ///< Thickness
  std::vector<int> lMat;              ///< Material
  std::vector<double> lTemp;          ///< temperature
  
  int centMat;                        ///< Central material
  double centTemp;                    ///< Central temperature

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  virtual void createObjects(Simulation&);

 public:

  DecLayer(const std::string&,std::string );
  DecLayer(const DecLayer&);
  DecLayer& operator=(const DecLayer&);
  /// Clone function
  DecLayer* clone() const override { return new DecLayer(*this); }
  ~DecLayer() override;

  /// Doesnt internal pipe
  int needsHePipe() const override { return 0; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

} 



#endif
 
