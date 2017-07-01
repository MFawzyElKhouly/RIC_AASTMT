/*
 * support.h
 *
 *  Created on: Feb 8, 2017
 *      Author: aastteam
 */

#ifndef CHAIN_ACION_SUPPORT_H_
#define CHAIN_ACION_SUPPORT_H_

#include "../headers/headers.h"
#include "../worldmodel/worldmodel.h"
#include "skillval.h"

#include "../formations/formation.h"
class supportSkill: public skilldesc {
protected:
	WorldModel * wm;
	formationLoader *loader;

public:
	 supportSkill(WorldModel *wm,formationLoader *loader) ;
	double lengthOfSupport();
	double currentSupport();
	double currentPressure();
	double calcCost() ;
	double generateTarget(double theta);
	~supportSkill()  {
		//~base();
	}
};

#endif /* CHAIN_ACION_SUPPORT_H_ */
