/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/ExternalCut.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef attachSystem_ExternalCut_h
#define attachSystem_ExternalCut_h

class Simulation;

namespace attachSystem
{

struct cutUnit
{
  HeadRule main;           ///<  cut
  HeadRule divider;        ///<  divider
};
  
  
/*!
  \class ExternalCut
  \version 1.0
  \author S. Ansell
  \date September 2016
  \brief ExternalCut unit  
  
  System to add this to components that need to be cut
*/

class ExternalCut 
{
 private:

  /// typedef for map holder of rules
  typedef std::map<std::string,cutUnit> cutTYPE;
  cutTYPE cutItems;        ///< Set of external rules


  static void makeShiftedSurf(ModelSupport::surfRegister&,
			      const HeadRule&,const int,
			      const int,const Geometry::Vec3D&,const double);
  static void makeExpandedSurf(ModelSupport::surfRegister&,
			       const HeadRule&,const int,
			       const Geometry::Vec3D&,
			       const double);
  
 public:

  ExternalCut();
  ExternalCut(const ExternalCut&);
  ExternalCut& operator=(const ExternalCut&);
  virtual ~ExternalCut();

  cutUnit& getUnit(const std::string&);
  const cutUnit* findUnit(const std::string&) const;
  
  void copyCutSurf(const std::string&,const ExternalCut&,const std::string&);
  
  void setCutSurf(const std::string&,const int);
  void setCutSurf(const std::string&,const std::string&);
  void setCutSurf(const std::string&,const HeadRule&);
  void setCutSurf(const std::string&,const attachSystem::FixedComp&,
		  const long int);
  void setCutSurf(const std::string&,const attachSystem::FixedComp&,
		  const std::string&);
  
  void setCutDivider(const std::string&,const std::string&);
  void setCutDivider(const std::string&,const HeadRule&);

  void createLink(const std::string&,attachSystem::FixedComp&,
		   const size_t,const Geometry::Vec3D&,
		   const Geometry::Vec3D&) const;

  /// Flag accessor
  bool isActive(const std::string&) const;
  std::string getRuleStr(const std::string&) const;

  std::string getComplementStr(const std::string&) const;

  std::string getBridgeStr(const std::string&) const;
  
  const HeadRule& getRule(const std::string&) const;
  const HeadRule& getDivider(const std::string&) const;

  
  Geometry::Vec3D interPoint(const std::string&,
			     const Geometry::Vec3D&,
			     const Geometry::Vec3D&) const;

  
  void makeShiftedSurf(ModelSupport::surfRegister&,
		       const std::string&,const int,
		       const int,const Geometry::Vec3D&,const double) const;
  
  void makeExpandedSurf(ModelSupport::surfRegister&,
			const std::string&,const int,
			const Geometry::Vec3D&,
			const double) const;

};

}

#endif
 
