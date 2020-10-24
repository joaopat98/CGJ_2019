#include "calcImpact.h"

float calcHeight(float z, vec3 initPos, vec3 velocity) {
	float t = (z - initPos.z) / velocity.z;
	return initPos.y + velocity.y * t - (9.81 / 2) * t * t;
}

vec2 impactPoint(float ballRadius, vec3 initPos, vec3 velocity, vec3 panelOrigin, float scale) {
	vec3 impact;
	impact.x = initPos.x;
	impact.y = calcHeight(panelOrigin.z + ballRadius, initPos, velocity);
	impact.z = panelOrigin.z;
	return (impact - panelOrigin)/scale;
}