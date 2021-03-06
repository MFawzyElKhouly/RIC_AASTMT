/*
 * analyzer.cc
 *
 *  Created on: Jan 28, 2017
 *      Author: aastteam
 */

#include "analyzer.h"
#include "../behaviors/naobehavior.h"

#include "../debugger/debugger.h"
#include "../math/vecposition.h"
#include "../math/Geometry.h"
#include "skillpass.h"
#include "support.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include<string>
#include <fstream>

#include <iostream>

#include <cstdio>
#define eps 0.01
#define INF 1<<12
using namespace std;
using namespace SIM;
string strategyFile;
map<string, double> factors;
double threatTime;

void loadParameters(string file, map<string, double> &factors) {
	factors.clear();
	fstream of(file.c_str(), ios_base::in);
	string str;
	double val;

	while (of >> str >> val) {
		factors[str] = val;
	}
	of.close();

}
kickSkill::kickSkill(SkillType type, WorldModel *wm, formationLoader *fm) :
		skilldesc(type) {
	this->wm = wm;
	this->fm = fm;
}
passSkill::passSkill(WorldModel *wm, formationLoader *fm) :
		kickSkill(SKILL_PASS, wm, fm) {

	loadParameters(fm->getStrategy() + "/passing.io", factors);
	myGoalP = factors["myGoal"];
	theirGoalP = factors["theirGoal"];
	surrOppP = factors["surrounding"];
	passlenP = factors["length"], passCutP = factors["cut"], supportP =
			factors["supporting"], supportP = factors["cutConst"], Cnear =
			factors["pressConst"];
	factor = factors["factor"];
	MyGoalC = factors["myGoalConst"], TheirGoalC = factors["theirGoalConst"];

}

throughSkill::throughSkill(WorldModel *wm, formationLoader *fm) :
		passSkill(wm, fm) {
	this->wm = wm;
	this->fm = fm;
}
shootSkill::shootSkill(WorldModel *wm, formationLoader *fm) :
		kickSkill(SKILL_SHOOT, wm, fm) {
	loadParameters(fm->getStrategy() + "/shooting.io", factors);

	//double C_TheirGoal = 0.0 , L_TheirGoal = -0.7 , R_TheirGoal = 0.7;
	Ccut = factors["cutConst"], Cnear = factors["pressConst"];
	factor = factors["factor"];
}

Analyzer::Analyzer(WorldModel *wm) {
	//freopen("f.txt", "w", stdout);
	this->wm = wm;
	this->maxShot = 10; //TODO get this from wm
	loader = NULL;
	max = (new skilldesc(SKILL_STAND));
}
Analyzer::Analyzer(WorldModel *wm, formationLoader*loader) {
	this->wm = wm;
	this->maxShot = 10; //TODO get this from wm
	this->loader = loader;
	strategyFile = loader->getStrategy();

	max = (new skilldesc(SKILL_STAND));
}
void Analyzer::resetCandidates() {

	for (int i = 0; i < (int) skillset.size(); i++)
		skillset[i].~skilldesc();
	skillset.clear();
}

bool off = false, of = true, od = true;
void Analyzer::generateDribble(int range) {
//	VecPosition tar = wm->getBall() - wm->getMyPosition();
//	tar*=1.2;
//	dribble skks = *(new dribble(wm,loader));
//	skks.setTarget(tar);
//	skillset.push_back(skks);
//	return;
//	if (threatTime < 2) {
//		skillset.push_back(*new dribbIntercept(wm,loader));
//		return;
//	}
	int thfas = wm->getOpponentClosestTo(wm->getBall()) + WO_OPPONENT1 - 1;

	double angle = -range;
	stringstream ss;
	int cltime = wm->getPlayerTimeTo(
			wm->getOpponentFastestTo(wm->getBall()) + WO_OPPONENT1 - 1,
			wm->getBall());

	for (int i = 0; angle <= range; i++) {
		double time = fabs(wm->getMyAngDeg() - angle) / 15;
		//if(cltime < time +0.5)
		//continue;
		dribble skill = *(new dribble(wm, loader));
		//skill.setAngle(angle);
		skill.angle = angle;
		VecPosition tar, ball = wm->getBall();
		VecPosition lball = wm->g2l(ball);
		tar = lball + VecPosition(1, angle, 0, POLAR);
		tar = wm->l2g(tar);
		skill.setTarget(tar);
		skill.calcCost();
		skillset.push_back(skill);
		//ss << angle << "_" << skill.getCost() << "|";
		angle += range / 10;

	}
//	skilldesc skk = *(new skilldesc(SKILL_STAND));
//	skk.setCost(100);
//	skillset.push_back(skk);

	/*dribble skill = *(new dribble(wm, loader));
	 skill.setTarget(wm->getBall() + VecPosition(1, Deg2Rad(wm->getMyAngDeg()), 0, POLAR));
	 skill.calcCost();
	 skillset.push_back(skill);
	 */string mes;
	ss >> mes;
	//NaoBehavior::debug->debug(wm->getCycle(), mes);
	//cout<<mes<<endl;
}
VecPosition Analyzer::getDuePos() {
	VecPosition min = loader->defending[wm->getUNum() - 1];

	VecPosition max = loader->attacking[wm->getUNum() - 1];
	double ratio = (wm->getBall().getX() + 15) / 30;
	VecPosition ret = min + (max - min) * ratio;
//if(wm->getUNum() == 2)
//	cout<< ret<<endl;
	return ret;
}
void Analyzer::generateBallHolderSkills() {

	int threat = WO_OPPONENT1 + wm->getOpponentFastestTo(wm->getBall()) - 1; // a2rab opponent lel kora
	double clos = 50; // wm->getBall().getDistanceTo(wm->getOpponent(WO_OPPONENT1-1+wm->getOpponentClosestTo(wm->getBall())));
	for (int i = WO_OPPONENT2; i <= WO_OPPONENT11; i++) {
//		cout<<	wm->getWorldObject(i)->orien
//				- (wm->getBall() - wm->getOpponent(i)).getAngleWithVector(
//						VecPosition(1, 0, 0, CARTESIAN))<<"\n";
//		if (!fabs(
//				wm->getWorldObject(i)->orien
//						- (wm->getBall() - wm->getOpponent(i)).getAngleWithVector(
//								VecPosition(1, 0, 0, CARTESIAN))) < 45) {
//			continue;
//		}
		if (!wm->getWorldObject(i)->validPosition) // || wm->getFallenOpponent(i))
			continue;
		double fact = 1;
		if ((wm->getBall() - wm->getMyPosition()).getAngleWithVector(
				wm->getBall() - wm->getOpponent(i)) < 90) {
			//fact = 2;
		}
		double add = 0;
		if (wm->getFallenOpponent(i))
			add = 2;
		clos = min(clos,
				fact * (wm->getBall() - wm->getOpponent(i)).getMagnitude()
						+ add);
	}
	//threatTime = wm->getPlayerTimeTo(threat, wm->getBall());
	if(wm->getPlayMode() == PM_PLAY_ON)
		threatTime = clos * 1.25;
	//threatTime = clos;
	int range = 20;
	if (wm->distancetoBall(wm->getMyPosition()) < 1.5) {
		od = false;
		if (clos > 0 //2.6
				//threatTime > 5 //TODO change this
				) {
			of = false;
			od = true;
			generatePassPoints(PLAYERS);
			generateKick();
			range = 80;
			//cout << "FAR"<<clos<<"\n";
			//return ; //delete
		}
		if (clos > 0			//0.2
				//threatTime >1
				) {
			generateDribble(range);
		} else {
			skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
			scill.setTarget(wm->getBall());
			scill.setCost(0);
			skillset.push_back(scill);
		}
		return;

	}
	skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
	//scill.skill = ;
	scill.setTarget(wm->getBall());
	scill.setCost(0);
	skillset.push_back(scill);

}
bool inZone(VecPosition ch, VecPosition center, double width, double top,
		double bottom) {
	return (abs(ch.getX()) <= top && abs(ch.getX()) >= bottom)
			&& (abs(ch.getY() - center.getY()) <= width / 2);
}
void Analyzer::generateAttackingSkills() {
	int unum = wm->getUNum();
	double top = getDuePos().getX() + loader->getDueLength(unum) / 2, bottom =
			getDuePos().getX() - loader->getDueLength(unum) / 2;
	for (int i = WO_OPPONENT1; i < WO_OPPONENT11; i++) {
		if (inZone(wm->getOpponent(i), getDuePos(), loader->getDueWidth(unum),
				top, bottom)) {
			top = wm->getOpponent(i).getX();
		}
	}
	if (wm->getBall().getX() > 0) {
		VecPosition supporter = wm->getBall() - VecPosition(1.5, 0, 0);
		if (wm->getTeammateClosestTo(supporter) == wm->getUNum()) {
			skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
			scill.setTarget(supporter);
			scill.setCost(0.0);
			skillset.push_back(scill);
			return;

		}
	}

	if (!inZone(wm->getMyPosition(), getDuePos(), loader->getDueWidth(unum),
			top, bottom)) {
		skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
		scill.setTarget(getDuePos());
		scill.setCost(0.0);
		skillset.push_back(scill);
		//cout<<"GoingHome "<<wm->getUNum()<<endl;;
		return;
	}
	supportSkill scill2 = *(new supportSkill(this->wm, this->loader));
	skillset.push_back(scill2);
	//of = true;
	//cout << "I am supporting" <<wm->getUNum()<<endl;
	//}
}
void Analyzer::generateDefensiveSkills() {
	skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
	scill.setTarget(getDuePos());
	scill.setCost(1000);
	skillset.push_back(scill);

	for (int i = 0 + WO_OPPONENT2; i <= WO_OPPONENT11; i++) {
		if ((wm->getTeammateClosestTo(wm->getOpponent(i)) == wm->getUNum()
				&& wm->getOpponentClosestTo(
						wm->getBall() == i - WO_OPPONENT1 + 1))
				&& (wm->getMyPosition().getX() - 0.5) < wm->getBall().getX()) { //if the nearest is me ..
			continue;
		}
		mark scill = *(new mark(wm, loader));
		//scill.skill = ;

		scill.setTarget(wm->getOpponent(i));
		VecPosition targ;
		scill.calcCost();
		if (wm->getRole(wm->getUNum() - 1) < 2) {
			if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
					< wm->getOpponent(i).getDistanceTo(
							wm->getMyRightGoalPost()))
				targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
			else
				targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
			targ *= 0.25;
			targ = wm->getOpponent(i) - targ;
		}
		if (wm->getRole(wm->getUNum() - 1) < 3) {
			if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
					< wm->getOpponent(i).getDistanceTo(
							wm->getMyRightGoalPost()))
				targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
			else
				targ = wm->getOpponent(i) - wm->getMyRightGoalPost();
			targ *= 0.15;
			targ = wm->getOpponent(i) - targ;
		}
//		 else {
//			targ = wm->getBall() - wm->getOpponent(i);
//			targ *= 0.25;
//			targ += wm->getOpponent(i);
//		}

		scill.setTarget(targ);

		skillset.push_back(scill);

	}
	generateintersect();
}

void Analyzer::generateintersect() {
	int n = 0;
	int Mark = 0;
	VecPosition me = wm->getMyPosition();
	VecPosition Half_Goal = (wm->getMyLeftGoalPost() + wm->getMyRightGoalPost())
			/ 2;
	//int NrstLeftTeammate,NrstRightTeammate;
	//	NrstLeftTeammate = wm->getTeammateClosestTo((wm->getMyLeftGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
	//	NrstRightTeammate = wm->getTeammateClosestTo((wm->getMyRightGoalPost()+Half_Goal)/2)+WO_TEAMMATE1-1;
	VecPosition ball = wm->getBall();
	//int cnt = 0;
	double d = wm->distancetoBall(wm->getMyPosition());

	if (wm->getMyPosition().getX() > wm->getBall().getX()
			|| wm->getFallenTeammate(wm->getUNum()) == true)
		//	&& d > 1.5)
		d += 50;

	for (int i = WO_TEAMMATE2; i <= WO_TEAMMATE11; i++) {// kam teammate a2rb ll kora

		if ((i - WO_TEAMMATE1 + 1) == (wm->getUNum() - WO_TEAMMATE1 + 1))
			continue;

//		if(me.getX()>ball.getX() && Mark &&
//						wm->getTeammateClosestTo(ball)==wm->getUNum()+WO_TEAMMATE1-1){
//		//			double DisToOpp = wm->getTeammate(i).getDistanceTo(ball);
//		//			int AngleWithBall = wm->getTeammate(i).getAngleWithVector(ball);
//		//			int AngleWithGoal = wm->getTeammate(i).getAngleWithVector((wm->getOppLeftGoalPost()+wm->getOppRightGoalPost())/2);
//					if(wm->getTeammate(i).getX()+1.0 < wm->getOpponent((wm->getOpponentClosestTo(ball)+WO_OPPONENT1-1)).getX()){
//						Mark =0;
//						cout << "Teammate Succ " << i << endl;
//					}
//				}
		double di = wm->distancetoBall(wm->getTeammate(i));
		if (wm->getTeammate(i).getX() > wm->getBall().getX()
				|| wm->getFallenTeammate(i - WO_TEAMMATE1 + 1)) {// && wm->distancetoBall(wm->getTeammate(i)) > 1.5)
			di += 50;
			Mark++;
		}

		if ((di < d) /*(di == d && i - (WO_TEAMMATE1 < wm->getUNum() - 1))*/
		&& wm->getWorldObject(i)->validPosition
				&& wm->getFallenTeammate(i) == false
				//&& !wm->getFallenTeammate(i)
						) {
			n++;
		}
	}
	int MyNum = wm->getUNum() + WO_TEAMMATE1 - 1;
	int ClosestTeammte = wm->getTeammateClosestTo(ball) + WO_TEAMMATE1 - 1;
	int i = WO_OPPONENT1 - 1 + wm->getOpponentClosestTo(wm->getBall()); //a2rb opp ll kora
	VecPosition v = wm->getMyRightGoalPost() + wm->getMyLeftGoalPost();
	v /= 2;
	double ang = wm->getWorldObject(i)->orien;
	VecPosition end = v; //wm->getOpponent(i) +(wm->getOpponent(i) - );// + VecPosition(20, ang, 0, POLAR);
	VecPosition tar = perpPoint(wm->getMyPosition(), wm->getBall(), end);

	int Opp = wm->getOpponentClosestTo(ball) + WO_OPPONENT1 - 1;
	VecPosition target, tempTarget;
	tempTarget = wm->predictBall(0.3);
	target = tempTarget + ((wm->predictBall(0.7) - ball) * 7);
//		if(ball.getX()>-5)
//			target = tempTarget+((tempTarget-ball)*3);
//		else
//			target = tempTarget+((tempTarget-ball)*5);
	//target = tempTarget + (tempTarget-ball);
//		if(fabs(tempTarget.getX()-ball.getX()) > 5){
//			cout << "Fault Pred" << e	&& wm->getRole(wm->getUNum()) > 2ndl;
//			VecPosition lball  = wm->g2l(ball);
//			double OppAngle = wm->getOpponent(Opp).getAngleWithVector((wm->getMyLeftGoalPost()+wm->getMyRightGoalPost())/2);
//			target = lball+*(new VecPosition(-1,/*wm->getWorldObject(Opp)->orien*/(180-OppAngle),0,POLAR));
//			target =wm->l2g(target);
//		}
//		else
//		{
//			cout << "GOOD" << endl;
	if (Mark == 9 && wm->getRole(wm->getUNum() - 1) == 1) {
		//cout << "Marking ALLLL" << endl;
		if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
				< wm->getOpponent(i).getDistanceTo(wm->getMyRightGoalPost()))
			target = wm->getOpponent(i) - wm->getMyLeftGoalPost();
		else
			target = wm->getOpponent(i) - wm->getMyRightGoalPost();
		//	cout << "Ball " << ball.getX() << " " << ball.getY() << endl;
		target *= 0.2;
		target = ball - target;
		//		cout << "New " << target.getX() << " " << target.getY() << endl;

		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));
		scill.setTarget(target);
		scill.setCost(0);
		skillset.push_back(scill);
		return;
	}
	//cout << "Ball " << ball.getX() << " " << ball.getY() << endl;
	//cout << "New " << target.getX() << " " << target.getY() << endl;
	else if (n == 0 //|| ((wm->distanceToMyGoal(ball) < 10) && (wm->getUNum() < 6))
			) {

		//cout << "Angle : "<< OppAngle << endl;
	//cout << "Ball " << ball.getX() << " " << ball.getY() << endl;
	//	cout << "New " << target.getX() << " " << target.getY() << endl;
		//VecPosition IncDis = *(new VecPosition (0.3,0,0,POLAR));
		//cout << "Ball Pos = " << ball.getX() << " " << ball.getY() << endl;
		//cout << "New POSSSSSS = " << target.getX() << " " << target.getY() << endl;
		//cout << "Nearest Players is : " << wm->getUNum() << endl;
		skilldesc scill = *(new skilldesc(SKILL_INTERCEPT));

		if (wm->getFallenOpponent(wm->getOpponentClosestTo(ball))) {

			scill.setTarget(ball);
			//cout << "GOING" << endl;
		} else
			scill.setTarget(target);		//(ball+tar)/2);
		//	cout << "DEF" << endl;

		scill.setCost(0);
		skillset.push_back(scill);
		return;
	} else if (n == 1) // || ((wm->distanceToMyGoal(wm->getBall()) < 10) && (wm->getUNum() < 6))
			{
		//	cout << "Second Nst = " << wm->getUNum() << endl;
		//cout << "Second nearest is " << wm->getUNum() << endl;
		mark scill = *(new mark(wm, loader));
		VecPosition targ;
		scill.setCost(0);
		if (wm->getOpponent(i).getDistanceTo(wm->getMyLeftGoalPost())
				< wm->getOpponent(i).getDistanceTo(wm->getMyRightGoalPost()))
			targ = wm->getOpponent(i) - wm->getMyLeftGoalPost();
		else
			targ = wm->getOpponent(i) - wm->getMyRightGoalPost();

		targ *= 0.20;
		targ = ball - targ;
		scill.setTarget(targ);		//targ*= 0.50;
		//targ = ball - targ;
		skillset.push_back(scill);
	}

}
void Analyzer::generateCanditates() {
	if (wm->getPlayMode() == PM_PLAY_ON) {
		if (wm->isOut(wm->getMyPosition())) {
			resetCandidates();
			skilldesc scill = *(new skilldesc(SKILL_WALK_OMNI));
			//scill.skill = ;
			scill.setTarget(getDuePos());
			scill.setCost(0);
			skillset.push_back(scill);
			return;
		}
	}
	VecPosition ball = wm->getBall();
	if (wm->getUNum() == 1 && wm->distanceToMyGoal(ball) > 4) {
		return;
	}

	if (!skillset.empty())
		return;
	threatTime = 300; //big number
	skillset.push_back(*(new skilldesc(SKILL_STAND)));
	//cout<<"..x\n";
	int ballHolder = wm->getTeammateClosestTo(wm->getBall());
	bool really = false;
	for (int i = WO_TEAMMATE1; i <= WO_TEAMMATE11; i++) {
		if (wm->getUNum() + WO_TEAMMATE1 - 1 == i)
			continue;
		double dist = wm->getTeammate(i).getDistanceTo(ball);
		if (dist < ball.getDistanceTo(wm->getMyPosition()) && dist < 1) {
			really = true;
		}
	}
	if(wm->hasBall())
	if (ballHolder == wm->getUNum() //&& !really
			//&&loader->getTeamState() == ATTACKING
			) {
		generateBallHolderSkills();
		return;
	}
	else if (wm->getBallHolder() - WO_TEAMMATE1 < 11
			&& (wm->distanceToMyGoal(ball)) > 0) {

		generateAttackingSkills();
		return;
	}
	generateDefensiveSkills();
	return;

}
void Analyzer::generatePoints() {
	stringstream ss;
	ss << wm->getBall() << "_" << wm->getUNum();
	string out;
	ss >> out;
	NaoBehavior::debug->success(wm->getCycle(), out);
	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| i - WO_TEAMMATE1 + 1 == wm->getUNum())
			continue;
		VecPosition target = VecPosition(0, 0, 0); //wrong entry
		stringstream ss;
		ss << target << "_" << (i - WO_TEAMMATE1) << "_"
				<< wm->getWorldObject(i)->pos;
		string out;
		ss >> out;
		NaoBehavior::debug->debug(wm->getCycle(), out);
		if (wm->getBall().getDistanceTo(target) > maxShot)
			continue;
		passSkill scill = *(new passSkill(this->wm, this->loader));

		scill.setTarget(target);

		scill.calcCost();

		skillset.push_back(scill);
	}
}

void Analyzer::generatePlayers() {
	VecPosition ball = wm->getBall();
	for (int i = 0 + WO_TEAMMATE1; i <= WO_TEAMMATE11; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| i - WO_TEAMMATE1 + 1 == wm->getUNum()
				|| wm->isOut(wm->getTeammate(i)))
			continue;
		if (ball.getDistanceTo(wm->getTeammate(i)) > maxShot)
			continue;
		passSkill scill = *(new passSkill(this->wm, this->loader));
		//scill.skill = ;

		VecPosition target = wm->getTeammate(i)
				+ *(new VecPosition(0.2, Deg2Rad(wm->getWorldObject(i)->orien),
						0, POLAR));
		if ((wm->getTeammate(i) - ball).getAngleWithVector(
				target - wm->getTeammate(i)) < 10) {
			double shift = 10;
			if (ball.getY() > 0)
				shift = -10;
			target = VecPosition(target.getMagnitude(),
					target.getTheta() + shift, 0, POLAR);
		}
		if (target.getX() < wm->getBall().getX() && target.getX() < 0) {
			continue;
		}
		scill.setTarget(target);

		scill.calcCost();

		skillset.push_back(scill);
	}
}

void Analyzer::generatePassPoints(PassType type) {
	switch (type) {
	case POINTS:
		generatePoints();
		break;
	case PLAYERS:
		generatePlayers();
		break;
	}
}
skilldesc Analyzer::getTopSkill() {
	skilldesc t = *max;
	t.calcCost();
	double dmi = 100;
	double pmi = 100;
	if (skillset.empty()) {

		skilldesc max = *(new skilldesc(SKILL_STAND));
		//max.getType() = SKILL_STAND;
		return max;
	}
	max = &skillset[0];
	for (int i = 0; i < (int) skillset.size(); i++) {
		if (skillset[i].getCost() < max->getCost()
				&& skillset[i].getTime() < threatTime)
			max = &skillset[i];
		if (skillset[i].getType() == SKILL_DRIBBLE)
			dmi = min(dmi, skillset[i].getCost());
		if (skillset[i].getType() == SKILL_PASS)
			pmi = min(pmi, skillset[i].getCost());
//		if(wm->getTeammateClosestTo(wm->getBall()) == wm->getUNum()
//				&& skillset[i].getType() == SKILL_PASS)
		//cout<<skillset[i].getCost()<<" Is my pass cost\n";

	}

	off = of;
	stringstream ss;
	ss << (max->getType());
	string message;
	ss >> message;
	if (t.getCost() - max->getCost() < 0.7 && t.getType() == max->getType()
			&& (t.getType() == SKILL_PASS || t.getType() == SKILL_DRIBBLE))
		max = &t;
//	if(wm->getBallHolder() == WO_TEAMMATE1+wm->getUNum()-1)
//			cout <<threatTime<<" "<<max->getType() << " "
//			<< max->getTime() <<endl;

//	if(wm->getTeammateClosestTo(wm->getBall()) == wm->getUNum() )
//		cout<<max->getType()<<" Is my skill and "<<max->getCost() <<"\n";

	if (max->getType() == SKILL_DRIBBLE) {
		VecPosition tar = max->getTarget();
		//cout<<"IAm dribbling at"<<wm->g2l(tar).getTheta()<<endl;
	}
	//if(dmi < 100 && pmi < 100)
	//cout<<dmi <<" "<<pmi<<endl;

	return *max;
}

VecPosition kickSkill::NearestTeammate(VecPosition target) {
	return wm->getTeammate(wm->getTeammateClosestTo(target) + WO_TEAMMATE1);
}

VecPosition kickSkill::NearestOpponent(VecPosition target) {
	return wm->getOpponent(wm->getOpponentClosestTo(target) + WO_OPPONENT1);
}
double kickSkill::surroundingOpponents(VecPosition target,
		double Distance_NearestOpp_To_Ball) {
	double ret = 0;
	int n = 0;
	for (int i = 0 + WO_OPPONENT2; i < WO_OPPONENT1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos)
				|| wm->getWorldObject(i)->pos.getDistanceTo(target) > 4)
			continue;
		ret += exponential(wm->getPlayerTimeTo(i, target), Cnear);
		n++;
	}
	double width = HALF_FIELD_Y;
	width -= abs(target.getY());
	ret += exponential(width, Cnear);
	return n == 0 ? 0 : ret / 6;
}

double kickSkill::supportingTeamMates(VecPosition target) {
	if (NUM_AGENTS == 1)
		return 0;
	double ret = 0;
	int n = 0;
	for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		if (!wm->getWorldObject(i)->validPosition
				|| wm->isOut(wm->getWorldObject(i)->pos))
			continue;
		double distance = wm->getTeammate(i).getDistanceTo(target);
		if (distance > eps) {
			ret += 1 - exponential(distance, Cnear);
			n++;
		}
	}
	return n == 0 ? 0 : ret / n;
}
double kickSkill::passSafety(VecPosition passer, VecPosition target) {

	double length = (target - passer).getMagnitude();

	double nearest = 1e9;

	int targetPlayer = wm->getTeammateClosestTo(target);

	for (int i = 0 + WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; i++) {
		int curUNum = i - WO_TEAMMATE1 + 1;
		if (wm->getWorldObject(i)->validPosition && wm->getUNum() != curUNum
				&& targetPlayer != curUNum)

			nearest = min(nearest, perp(wm->getTeammate(i), passer, target));
	}

	for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; i++)
		if (wm->getWorldObject(i)->validPosition)
			nearest = min(nearest, perp(wm->getOpponent(i), passer, target));

	const double th = 0.2;

	if (nearest <= th) {
		return 1;
	}
	double ret = exponential(nearest, Ccut);

	return ret;
}

double passSkill::effectiveness(VecPosition passer, VecPosition target) {

	double myGoal = exponential(wm->distanceToMyGoal(target), MyGoalC);
	double theirGoal = 1
			- exponential(wm->distanceToOppGoal(target), TheirGoalC);
	double surrounding_Opponents = surroundingOpponents(target,
			wm->getPlayerTimeTo(
					wm->getTeammateFastestTo(target) + WO_TEAMMATE1 - 1,
					target));
//	double supporting_TeamMates = supportingTeamMates(target);

	double ret = (myGoal + 2 * theirGoal + surrounding_Opponents);
	return ret;
}

double passSkill::evaluatePass(VecPosition passer, VecPosition target) {
	passer.setZ(0);
	target.setZ(0);
	double e = effectiveness(passer, target);
	double s = passSafety(passer, target);
	cost = (7 * e + 1 * s) + wm->getBall().getX() / 7.5; // dribble preferal
	//cout << ">>>>>>>>>>PE = " << e << " PS = " << s << " PC = " << cost << "\n";

	return cost;
}

double passSkill::calcCost() {
	//if (wm->getBall().getDistanceTo(wm->getMyPosition()) > 1.5)
	//return cost = INF;
	cost = evaluatePass((wm->getBall()), (target));
	this->calcTime();
	return cost;
}

////
void Analyzer::generateKick() {
	shootSkill *scilll = new shootSkill(wm, loader);
	double cost = INF;
	VecPosition Pos_ToShot;
	VecPosition ball = wm->getBall();
	if (wm->distanceToOppGoal(ball) < this->maxShot - 1) {

//		Pos_ToShot = VecPosition(15.2, 0, 0);
//		scilll->setTarget(Pos_ToShot);
//		scilll->calcCost();
//		if (scilll->getCost() < cost) {
//
//			skillset.push_back(*scilll);
//		}
//		Pos_ToShot = VecPosition(15.2, -0.3, 0);
//		scilll->setTarget(Pos_ToShot);
//		scilll->calcCost();
//
//		if (scilll->getCost() < cost) {
//			skillset.push_back(*scilll);
//		}
//		Pos_ToShot = VecPosition(15.2, 0.3, 0);
//		scilll->setTarget(Pos_ToShot);
//		scilll->calcCost();
//
//		if (scilll->getCost() < cost) {
//			skillset.push_back(*scilll);
//		}
		double dis = maxShot - wm->distanceToOppGoal(ball);
		dis = dis / maxShot;
		dis *= fabs(wm->getOppRightGoalPost().getY());
		Pos_ToShot = VecPosition(15.2, -dis, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();
		Pos_ToShot = VecPosition(15.2, 0, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();
		Pos_ToShot = VecPosition(15.2, dis, 0);
		scilll->setTarget(Pos_ToShot);
		scilll->calcCost();

	}
}
double shootSkill::calcCost() {
	calcTime();
	return cost = factor * this->passSafety(wm->getBall(), this->target);
}
