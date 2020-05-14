#pragma once

#include "Game.h"

class CMath
{
public:
	// �� �� ������ �Ÿ�.
	static float GetDist(const _tagPosition& tSrc, const _tagPosition& tDest);

	static float GetDist(float x1, float x2);
	
	// Src���� Dest�� ���ϴ� ������ ���ϴ� �Լ��̴�.
	static float GetAngle(const _tagPosition& tSrc, const _tagPosition& tDest);

	static bool CollisionRectToRect(const RectInfo& tSrc,
		const RectInfo& tDest, Position& tIntersect);

	static bool CollisionSphereToSphere(const SphereInfo& tSrc,
		const SphereInfo& tDest, Position& tIntersect);

	static bool CollisionRectToSphere(const RectInfo& tSrc,
		const SphereInfo& tDest, Position& tIntersect);

	static bool CollisionRectToPoint(const RectInfo& tSrc,
		const Position& tDest, Position& tIntersect);

	static bool CollisionSphereToPoint(const SphereInfo& tSrc,
		const Position& tDest, Position& tIntersect);

	static bool CollisionPointToPoint(const Position& tSrc,
		const Position& tDest, Position& tIntersect);

	static bool CollisionLineToLine(const Position& tSrcSrc,
		const Position& tSrcDest,
		const Position& tDestSrc,
		const Position& tDestDest,
		Position& tIntersect);

	static bool CollisionRectToLine(const RectInfo& tRect, const Position& tLineSrc,
		const Position& tLineDest, const RectInfo& tLineRect, Position& tIntersect);
};

