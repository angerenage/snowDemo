#include "tree.h"

Mesh modelFromParams(unsigned int seed, const TreeParameters* params) {
	srand(seed);

	vec3 *crownPoints = (vec3*)malloc(params->crown_point_count * sizeof(vec3));

	vec3 center = params->crown_center;
	vec3 size = params->crown_size;
	for (unsigned int i = 0; i < params->crown_point_count; i++) {
		crownPoints[i] = (vec3){
			center.x + fmodf(rand(), size.x) - size.x * 0.5f,
			center.y + fmodf(rand(), size.y) - size.y * 0.5f,
			center.z + fmodf(rand(), size.z) - size.z * 0.5f
		};
	}
	
	printf("got the crown!\n");

	TreeGenerator gen = {
		params->gen_max_dist,
		params->gen_min_dist,
		params->branch_length,
		params->gen_max_dist * params->gen_max_dist,
		params->gen_min_dist * params->gen_min_dist,
		NULL,
		0,
		NULL,
		0
	};

	for (unsigned int i = 0; i < params->trunk_count; i++) {
		addTrunk(params->trunks[i], params, &gen, crownPoints);
		printf("got the trunk at: %f %f %f\n", params->trunks[i].x, params->trunks[i].y, params->trunks[i].z);
	}

	printf("starting generation!\n");
	processAll(100, crownPoints, params->crown_point_count, &gen);

	// TODO: add gravity

	printf("finished generation! %ld branches\n", gen.branchCount);

	vec3 *vertices = (vec3*)malloc(gen.branchCount * 2 * sizeof(vec3));
	for (unsigned int i = 0; i < gen.branchCount; i++) {
		Branch br = gen.branches[i];

		vertices[i * 2] = br.position;
		vertices[i * 2 + 1] = (vec3){
			br.position.x + br.growDirection.x,
			br.position.y + br.growDirection.y,
			br.position.z + br.growDirection.z
		};
	}

	GLuint vao = createVAO(vertices, gen.branchCount * 2);

	free(vertices);

	/*Urho3D::SharedPtr<Urho3D::Model> treemodel = Tree::tModel(context_, gen, params);*/
	return (Mesh){vao, gen.branchCount * 2, 0};
}

void addTrunk(vec3 base, const TreeParameters* params, TreeGenerator *gen, const vec3 *crowPoints) {
	gen->trunkCount++;
	gen->trunks = (unsigned int*)realloc(gen->trunks, gen->trunkCount * sizeof(unsigned int));
	gen->trunks[gen->trunkCount - 1] = gen->branchCount;

	gen->branchCount++;
	gen->branches = (Branch*)realloc(gen->branches, gen->branchCount * sizeof(Branch));
	gen->branches[gen->branchCount - 1] = (Branch){
		(vec3){0, 0, 0},
		0,
		base,
		0,
		NULL,
		0
	};

	while (true) {
		Branch *LB = &gen->branches[gen->branchCount - 1];

		for (unsigned int i = 0; i < params->crown_point_count; i++) {
			vec3 point = crowPoints[i];
			float distSQ =	(point.x - LB->position.x) * (point.x - LB->position.x) +
							(point.y - LB->position.y) * (point.y - LB->position.y) +
							(point.z - LB->position.z) * (point.z - LB->position.z);

			if (distSQ < gen->maxDistanceSQ) return;
		}

		//printf("test\n");

		LB->childrenCount++;
		LB->childrenID = (unsigned int*)realloc(LB->childrenID, LB->childrenCount * sizeof(unsigned int));
		LB->childrenID[LB->childrenCount - 1] = gen->branchCount;

		gen->branchCount++;
		gen->branches = (Branch*)realloc(gen->branches, gen->branchCount * sizeof(Branch));
		LB = &gen->branches[gen->branchCount - 2];

		gen->branches[gen->branchCount - 1] = (Branch){
			(vec3){0, 0, 0},
			0,
			(vec3){LB->position.x, LB->position.y + params->branch_length, LB->position.z},
			gen->branchCount - 1,
			NULL,
			0
		};
	}

	printf("trunk added!\n");
}

void erase_point(vec3** points, size_t* pointsCount, size_t i) {
	if (i >= *pointsCount) return;

	for (size_t j = i; j < *pointsCount - 1; j++) {
		(*points)[j] = (*points)[j + 1];
	}

	(*pointsCount)--;
}

unsigned int process(vec3* crownPoints, size_t crownPointsCount, TreeGenerator* gen) {
	unsigned int BranchesAdded = 0;

	for (unsigned int i = crownPointsCount - 1; i > 0; i--) {
		vec3 point = crownPoints[i];

		float closeDistSQ = gen->maxDistanceSQ * 2;
		Branch* closeBranch = NULL;

		for (unsigned int j = 0; j < gen->branchCount; j++) {
			Branch br = gen->branches[j];
			float distSQ =	(point.x - br.position.x) * (point.x - br.position.x) +
							(point.y - br.position.y) * (point.y - br.position.y) +
							(point.z - br.position.z) * (point.z - br.position.z);

			if (distSQ > gen->maxDistanceSQ) {
				continue;
			}
			else if (distSQ < gen->minDistanceSQ) {
				erase_point(&crownPoints, &crownPointsCount, i);
				closeBranch = NULL;
				break;
			}
			else if (distSQ < closeDistSQ) {
				closeDistSQ = distSQ;
				closeBranch = &gen->branches[j];
			}
		}

		if (closeBranch != NULL) {
			closeBranch->growCount++;

			vec3 originalDir = closeBranch->growDirection;
			closeBranch->growDirection = (vec3) {
				originalDir.x + (point.x - closeBranch->position.x),
				originalDir.y + (point.y - closeBranch->position.y),
				originalDir.z + (point.z - closeBranch->position.z)
			};
		}
	}

	for (unsigned int i = 0; i < gen->branchCount; i++) {
		Branch br = gen->branches[i];
		if (br.growCount > 0) {
			br.childrenCount++;
			br.childrenID = (unsigned int*)realloc(br.childrenID, br.childrenCount * sizeof(unsigned int));
			br.childrenID[br.childrenCount - 1] = gen->branchCount;

			gen->branchCount++;
			gen->branches = (Branch*)realloc(gen->branches, gen->branchCount * sizeof(Branch));
			gen->branches[gen->branchCount - 1] = growBranch(&br, gen->branchLength, i);

			BranchesAdded++;
		}
	}

	return BranchesAdded;
}

void processAll(const unsigned maxIter, vec3* crownPoints, size_t crownPointsCount, TreeGenerator* gen) {
	if (maxIter == 0) {
		while (process(crownPoints, crownPointsCount, gen) != 0);
	}
	else {
		for (unsigned i = 0; i < maxIter && process(crownPoints, crownPointsCount, gen) != 0; i++);
	}
}

Branch growBranch(Branch* branch, float branchLength, unsigned int parentID) {
	vec3 dir = vec3_scale(vec3_normalize(branch->growDirection), branchLength);
	vec3 newposition = (vec3){
		dir.x + branch->position.x + (((float)(rand() % 10000) / 5000.0f - 1.0f) * branchLength * 0.1f),
		dir.y + branch->position.y + (((float)(rand() % 10000) / 5000.0f - 1.0f) * branchLength * 0.1f),
		dir.z + branch->position.z + (((float)(rand() % 10000) / 5000.0f - 1.0f) * branchLength * 0.1f)
	};
	branch->growCount = 0;
	branch->growDirection = (vec3){0, 0, 0};

	return (Branch){
		dir,
		0,
		newposition,
		parentID,
		NULL,
		0
	};
}