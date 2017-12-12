Simulates social network formation using better-response dynamics. Agents are
modelled as 2D coordinates. Each agent can maintain a number of relationships
and gains utility based on a function of the distance between the coordinates
for the agents. Agents will take turns changing relationships maintained to
improve utility.

Parameters:
uniform: agent coordinates will be distributed uniformly within a square
doublenormal: Each agent coordinate will have a bimodal distribution (sum of two
	normal distributions), meaning there will be four clusters of agents.

global: agents can attempt to switch relationships to any other agent
twoaway: agents can only attempt to switch relationships to friends of friends
	(agents with relationships with agents they have relationships with)
	
symm: pairs of agents must agree to maintain a relationship for either to gain
	a benefit.
asymm: agents may gain utility from unilateral relationships.

inverse/exponential/linear: functions of distance between agents used to
	determine utility gained.
	
immediate: agents only gain utility from immediate friends
fof: agents gain utility at a reduced rate from friends of their friends
mlvl_const/mlvl_inv/mlvl_exp: agents gain utility from multiple levels of
	indirect relationships, with varying functions for utility falloff as the
	number of hops increases.

friendlevels <num>: when using mlvl_const, mlvl_inv, or mlvl_exp, determines the
	maximum depth for utility gain.

Requires OpenCV and glpk.


