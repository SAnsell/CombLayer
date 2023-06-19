/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   globalInc/XMLwriteVisitor.h
*
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef XMLwriteVisitor_h
#define XMLwriteVisitor_h

namespace XML
{

class XMLcollect;

/*!
  \class XMLwriteVisitor
  \version 1.0
  \author S. Ansell
  \date April 2008
  \brief XMLwriter (global)

  This is the global output class to XML via the BaseVisit
  system. The .h file is included everywhere so should be
  light, i.e. all classes are forward declared.
*/ 

class XMLwriteVisitor : public Global::BaseVisit
{
 private: 
  
  int managed;                 ///< Is the reference a managed object
  XML::XMLcollect& XOut;       ///< Storage space 
  int spectraData;             ///< Write out spectra data

  XMLwriteVisitor& operator=(const XMLwriteVisitor&);  ///< Inaccessable assignment operator
  
 public:

  XMLwriteVisitor();
  XMLwriteVisitor(XML::XMLcollect&);
  XMLwriteVisitor(const XMLwriteVisitor&);
  ~XMLwriteVisitor() override;

  /// Access Collection
  XML::XMLcollect& getCollect() const { return XOut; }
  void writeStream(std::ostream&) const;
  void writeFile(const std::string&) const;

  // Geometry stuff:
  void Accept(const Geometry::Surface&) override;
  void Accept(const Geometry::Quadratic&) override;
  void Accept(const Geometry::ArbPoly&) override;
  void Accept(const Geometry::CylCan&) override;
  void Accept(const Geometry::Cone&) override;
  void Accept(const Geometry::Cylinder&) override;
  void Accept(const Geometry::General&) override;
  void Accept(const Geometry::MBrect&) override;
  void Accept(const Geometry::NullSurface&) override;
  void Accept(const Geometry::Plane&) override;
  void Accept(const Geometry::Sphere&) override;
  void Accept(const Geometry::Torus&) override;
  void Accept(const Geometry::Line&) override;

  // MonteCarlo stuff:
  void Accept(const MonteCarlo::Material&) override;
  void Accept(const MonteCarlo::Object&) override;

  // ModelSupport stuff:
  void Accept(const ModelSupport::surfIndex&) override;
    
  // Functions:
  void Accept(const FuncDataBase&) override;  
};

}  // NAMESPACE XML

#endif
