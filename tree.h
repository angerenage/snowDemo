#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "glutils.h"

/*http://algorithmicbotany.org/papers/colonization.egwnp2007.large.pdf*/

typedef struct s_treeParameters {
	unsigned int crown_point_count;
	vec3 crown_center;
	vec3 crown_size;
	
	float gen_max_dist;
	float gen_min_dist;
	float branch_length;

	vec3* trunks;
	size_t trunk_count;

	unsigned int mdl_decimate_count;
	bool mdl_smooth;
	float mdl_min_R;
	float mdl_n_R;
	unsigned int mdl_min_vertex_count;
	unsigned int mdl_max_vertex_count;

	vec3 BBsize; //bounding-box size
} TreeParameters;

typedef struct s_branch {
	vec3 growDirection;
	unsigned int growCount;
	vec3 position;
	unsigned int parentID;

	unsigned int *childrenID;
	size_t childrenCount;
} Branch;

typedef struct s_TreeGenerator {
	float maxDistance;
	float minDistance;
	float branchLength;

	float maxDistanceSQ;
	float minDistanceSQ;

	Branch *branches;
	size_t branchCount;

	unsigned int *trunks;
	size_t trunkCount;
} TreeGenerator;

Mesh modelFromParams(unsigned int seed, const TreeParameters* params);
void addTrunk(vec3 base, const TreeParameters* params, TreeGenerator *gen, const vec3 *crowPoints);
unsigned int process(vec3* crownPoints, size_t crownPointsCount, TreeGenerator* gen);
void processAll(const unsigned maxIter, vec3* crownPoints, size_t crownPointsCount, TreeGenerator* gen);
Branch growBranch(Branch* branch, float branchLength, unsigned int parentID);