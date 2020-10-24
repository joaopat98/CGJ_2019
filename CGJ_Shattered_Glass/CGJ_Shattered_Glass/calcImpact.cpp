#include "calcImpact.h"

vec2 calcPos(float z, vec3 initPos, vec3 velocity) {
	float t = (z - initPos.z) / velocity.z;
	return {(float) (initPos.x + velocity.x * t),(float) (initPos.y + velocity.y * t - (9.81 / 2) * t * t )};
}

vec2 impactPoint(float ballRadius, vec3 initPos, vec3 velocity, vec3 panelOrigin, float scale) {
	vec3 impact = calcPos(panelOrigin.z + ballRadius, initPos, velocity);;
	impact.z = panelOrigin.z;
	return (impact - panelOrigin)/scale;
}