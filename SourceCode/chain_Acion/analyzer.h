#ifndef ANALYZER_H
#define ANALYZER_H


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>

#include <boost/shared_ptr.hpp>

#include "../skills/skill.h"
#include "skillval.h"
#include <map>
#include <fstream>
using namespace std;
enum PassType {
	POINTS, PLAYERS
};



class WorldModel;
class BodyModel;
class NaoBehavior;
class VecPosition;

class formationLoader;

using namespace std;

class Analyzer {
private:
	WorldModel *wm;
	formationLoader *loader;

	skilldesc *max;

	double maxShot;
protected:
public:
	VecPosition H_Goal = (wm->getMyRightGoalPost() + wm->getMyLeftGoalPost())/2;
	double X_Goal = H_Goal.getX();
	double Y_Goal = H_Goal.getY();
	vector<skilldesc> skillset;
	Analyzer(WorldModel *wm);

	Analyzer(WorldModel *wm,formationLoader *loader);
	void generateCanditates();
	void generateBallHolderSkills();
	void generateAttackingSkills();
	void generateDefensiveSkills();
	VecPosition getDuePos();
	skilldesc getTopSkill();
	double calcCost(skilldesc scil);
	void generatePassPoints(PassType type);
	void resetCandidates();
	void generatePoints();
	//VecPosition generateThrough(int i);
	void generatePlayers();
	void generateKick() ;
	void generateintersect();
	VecPosition getHeatMap();
	void generateDribble(int range) ;
	bool canThreat(double time) ;
};
void loadParameters(string file,map<string,double> &factors);
#endif
