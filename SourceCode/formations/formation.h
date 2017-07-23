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
	ATTACKING, DEFENDING,ATTDEFEND,DEFATTACK
};
class formationLoader {
protected:
	vector<VecPosition> beamings;
	vector<VecPosition> attacking;
	vector<VecPosition> defending;
	vector<double> attackingLengths;
	vector<double> attackingWidths;
	vector<double> defendingLengths;
	vector<double> defendingWidths;
	TeamState state;
	string strategy;
	string prevState="HALF";
	//string prevState="HALF";
public:
	formationLoader();
	~formationLoader();
	inline VecPosition getBeamingPosition(int unum) {
		return beamings[unum-1];
	}

	inline void SetPrev(string pre){
		this->prevState = pre;
	}

	inline string GetPrev(){
			return prevState;
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
	inline double getDueLength(int unum) {
		return ((state == ATTACKING ||state == ATTDEFEND)) ?
				attackingLengths[unum-1] : defendingLengths[unum-1];
	}

	inline double getDueWidth(int unum) {
		return ((state == ATTACKING ||state == ATTDEFEND)) ?
				attackingWidths[unum-1] : defendingWidths[unum-1];
	}
	void loadFormations();
	void setStrategy(string strategy);
	string getStrategy () {
		return "./formations/"+strategy;
	}
};

#endif /* FORMATIONS_FORMATION_H_ */
