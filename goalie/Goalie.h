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

	    SIM::Point2D goalLineStart;
	    SIM::Point2D goalLineEnd;
	    SIM::Point2D ballLineStart;
	    SIM::Point2D ballLineEnd;
	    bool notYet, endIt;
	    double times;
	    double t;
	    bool f;
	    double t1,t2;
	    double ballv;
	    VecPosition p1,p2;
	    int counter;
	SkillType selectSkill();
private:

	bool splitDecision (double distance);
	bool standDecision ();
	bool kickDecision ();

};

#endif
