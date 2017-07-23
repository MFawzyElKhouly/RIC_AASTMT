#ifndef GOALIE_H
#define GOALIE_H

#include "../behaviors/naobehavior.h"
#include "../behaviors/behavior.h"
#include "../headers/headers.h"
#include "../parser/parser.h"
#include "../worldmodel/worldmodel.h"
#include "../bodymodel/bodymodel.h"
#include "../particlefilter/PFLocalization.h"
#include "../skills/skill.h"

// For UT Walk
#include <MotionCore.h>
#include <memory/Memory.h>
#include <memory/FrameInfoBlock.h>
#include <memory/SensorBlock.h>
#include <memory/JointBlock.h>
#include <memory/JointCommandBlock.h>
#include <memory/SimEffectorBlock.h>
#include <memory/WalkRequestBlock.h>

using namespace std;

class Goalie : public NaoBehavior
{
public:
	Goalie(const std::string teamName, int uNum, const map<string, string>& namedParams_, const string& rsg_);
	~Goalie();
protected:
	SkillType selectSkill();
private:

	VecPosition target;
	double oldX=0.0,oldY=0.0;
	double timeforGK=0.0;
	bool splitDecision (double distance);
	double getMagnitude(double p1, double q1, double p2, double q2);
	bool standDecision ();
	bool kickDecision ();
	bool canStand();
	bool checkingFall();

};

#endif
