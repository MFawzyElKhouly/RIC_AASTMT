/*
 * skillval.h
 *
 *  Created on: Feb 5, 2017
 *      Author: aastteam
 */

#ifndef CHAIN_ACION_SKILLVAL_H_
#define CHAIN_ACION_SKILLVAL_H_
#include "../headers/headers.h"
#include "../math/vecposition.h"

class  skilldesc {
protected:
	SkillType skill;
	VecPosition target;
	double cost;
	double factor;
public :
double angle = 0;
inline skilldesc(SkillType sk){
	this->skill = sk;
	target = *(new VecPosition(0,0,0));
	cost = 1.0;

	factor = 1.0;
}
inline
void setTarget(VecPosition target){
	this->target = target;
}
inline
VecPosition getTarget() {
	return target;
}

double virtual calcCost(){
return cost =1.0;
}
void setCost(double cost) {
	this->cost= cost;
}
inline SkillType getType() {
	return skill;
}
inline
double getCost() {
	return cost;

}

	virtual ~skilldesc(){

	}
};

#endif /* CHAIN_ACION_SKILLVAL_H_ */
