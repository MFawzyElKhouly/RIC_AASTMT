/*
 * formation.h
 *
 *  Created on: Feb 6, 2017
 *      Author: aastteam
 */

#ifndef FORMATIONS_FORMATION_H_
#define FORMATIONS_FORMATION_H_
#include <vector>
#include <iostream>
#include <string>
#include "../math/vecposition.h"
using namespace std;
enum TeamState {
	ATTACKING, DEFENDING,ATTDEFEND,DEFETTACK
};
class formationLoader {
protected:
	vector<VecPosition> beamings;
	vector<VecPosition> attacking;
	vector<VecPosition> defending;
	vector<double> attackingRadius;
	vector<double> defendingRadius;
	TeamState state;
	string strategy;
public:
	formationLoader();
	~formationLoader();
	inline VecPosition getBeamingPosition(int unum) {
		return beamings[unum-1];
	}

	inline void setTeamState(TeamState state) {
		this->state = state;
	}

	inline TeamState getTeamState() {
		return state;
	}

	inline VecPosition getDuePosition(int unum) {
		return ((state == ATTACKING ||state == ATTDEFEND) ? attacking[unum-1] : defending[unum-1]);
	}
	inline double getDueRadius(int unum) {
		return ((state == ATTACKING ||state == ATTDEFEND)) ?
				attackingRadius[unum-1] : defendingRadius[unum-1];
	}
	void loadFormations();
	void setStrategy(string strategy);
	string getStrategy () {
		return "./formations/"+strategy;
	}
};

#endif /* FORMATIONS_FORMATION_H_ */
