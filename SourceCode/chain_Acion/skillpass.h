/*
 * skillpass.h
 *
 *  Created on: Feb 6, 2017
 *      Author: aastteam
 */

#ifndef CHAIN_ACION_SKILLPASS_H_
#define CHAIN_ACION_SKILLPASS_H_
class WorldModel;
class kickSkill: public skilldesc {
protected:
	WorldModel * wm;
	formationLoader *fm;
	double Ccut = 2.5, Cnear = 8.5;

public:
	kickSkill(SkillType type, WorldModel *wm, formationLoader *fm);
	double supportingTeamMates(VecPosition target);
	double surroundingOpponents(VecPosition target,
			double Distance_NearestOpp_To_Ball);
	VecPosition NearestOpponent(VecPosition target);
	VecPosition NearestTeammate(VecPosition target);

	double passSafety(VecPosition passer, VecPosition target);
	//double calcCost();
	virtual ~kickSkill() {
		//~base();
	}
};
class passSkill: public kickSkill {
protected:

	double myGoalP, theirGoalP, surrOppP, passlenP, passCutP, supportP, MyGoalC,
			TheirGoalC;

public:
	passSkill(WorldModel *wm, formationLoader *fm);
	double evaluatePass(VecPosition passer, VecPosition target);
	double effectiveness(VecPosition passer, VecPosition target);
	double calcCost();
	virtual ~passSkill() {
		//~base();
	}
	/***
	 * returns time to performs s pass in seconds
	 */
	double calcTime() {
		double time = 0;
		double rotSpeed = 100;
		VecPosition tempTar = wm->g2l(target);
		double turnAng = tempTar.getTheta();
		time = turnAng / rotSpeed;
		if (wm->getBall().getDistanceTo(this->target) < 4.5)
			time += 1; //KICK_IK time
		else
			time += 2; //KICK_FORWARD time
		return time;
	}
};
class throughSkill: public passSkill {

public:
	throughSkill(WorldModel *wm, formationLoader *fm);
//	double devaluatePass(VecPosition passer, VecPosition target,int r,bool x);
//	double deffectiveness(VecPosition passer, VecPosition target,int receiver,bool x);
//	double dsupportingTeamMates(VecPosition target,bool x);
//		double dsurroundingOpponents(VecPosition target,
//				double Distance_NearestOpp_To_Ball,bool x);
//
//		double dpassSafety(VecPosition passer, VecPosition target,bool x);

	//double calcCost();
	virtual ~throughSkill() {
		//~base();
	}
};
class shootSkill: public kickSkill {

public:

	shootSkill(WorldModel *wm, formationLoader *fm);

	double calcCost();
	~shootSkill() {
		//~base();
	}
	/***
	 * returns time to performs s pass in seconds
	 */
	double calcTime() {
		double time = 0;
		double rotSpeed = 100;
		VecPosition tempTar = wm->g2l(target);
		double turnAng = tempTar.getTheta();
		time = turnAng / rotSpeed;
		if (wm->getBall().getDistanceTo(this->target) < 4.5)
			time += 1; //KICK_IK time
		else
			time += 2; //KICK_FORWARD time
		return time;
	}
};
class dribble: public skilldesc {
protected:
	WorldModel * wm;
	formationLoader *fm;
	double CTime, C_TheirGoal, TheirGoalP, surrP, Cnear, safeP, effP;
public:
	dribble(WorldModel *wm, formationLoader *fm);
	double calcCost();
	double dribbleSafety();
	double effectiveness();
//double angle;
//double dribbleReliability();
	double surroundingOpponents();
	~dribble() {
//~base();
	}
	/***
		 * returns time to performs s pass in seconds
		 */
		double calcTime()  {
			double time= 0;
			double rotSpeed = 100;
			VecPosition tempTar = wm->g2l(target);
			double turnAng = tempTar.getTheta();
			time = turnAng / rotSpeed;
			return time;
		}
};
class ndribble: public skilldesc {
protected:
	WorldModel * wm;
	formationLoader *fm;
	double angle, shift;
public:
	ndribble(WorldModel *wm, formationLoader *fm);
	double calcCost();
	void setAngle(double angle);
	double getAngle();
	double safety();
	~ndribble() {
//~base();
	}

};
class mark: public skilldesc {
protected:
	WorldModel * wm;
	formationLoader *fm;
	double Cthre, Cbal, Cpos;

	double thre, bal, pos;
public:
	mark(WorldModel *wm, formationLoader *fm);
	double calcCost();
	double threatDist();
	double ballDist();
	double transitionDist();
	virtual ~mark() {
		//base
	}

};

#endif
