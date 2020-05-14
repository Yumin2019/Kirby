#include "Math.h"

float CMath::GetDist(const _tagPosition & tSrc, const _tagPosition & tDest)
{
	float fWidth = tDest.x - tSrc.x;
	float fHeight = tDest.y - tSrc.y;

	return sqrtf(fWidth * fWidth + fHeight * fHeight);
}

float CMath::GetDist(float x1, float x2)
{
	return abs(x1 - x2);
}

float CMath::GetAngle(const _tagPosition & tSrc, const _tagPosition & tDest)
{
	float fDist = GetDist(tSrc, tDest);
	float fWidth = tDest.x - tSrc.x;

	// cos �غ� / ���� 
	// �ڻ��� ���� ���Ѵ�.
	float fAngle = fWidth / fDist;

	// �ڻ��� ���� �̿��Ͽ� ������ ���Ѵ�.
	fAngle = acosf(fAngle);

	if (tDest.y < tSrc.y)
		fAngle = 2.f * PI - fAngle;

	return fAngle;
}

bool CMath::CollisionRectToRect(const RectInfo & tSrc, 
	const RectInfo & tDest, Position & tIntersect)
{
	if (tSrc.l > tDest.r)
		return false;

	else if (tSrc.r < tDest.l)
		return false;

	else if (tSrc.t > tDest.b)
		return false;

	else if (tSrc.b < tDest.t)
		return false;

	float l, t, r, b;

	// �� �浹������ ����� ���Ѵ�.

	l = tSrc.l > tDest.l ? tSrc.l : tDest.l;
	t = tSrc.t > tDest.t ? tSrc.t : tDest.t;
	r = tSrc.r > tDest.r ? tDest.r : tSrc.r;
	b = tSrc.b > tDest.b ? tDest.b : tSrc.b;

	tIntersect.x = (l + r) / 2.f;
	tIntersect.y = (t + b) / 2.f;

	return true;
}

bool CMath::CollisionSphereToSphere(const SphereInfo & tSrc,
	const SphereInfo & tDest, Position & tIntersect)
{

	// �� ���� �߽ɻ����� �Ÿ��� ���Ѵ�.
	float fDist = CMath::GetDist(tSrc.tCenter, tDest.tCenter);

	// �� �� �߽ɻ����� �Ÿ��� �� ���� �������� �պ��� �� ũ��. �浹�� �ƴϴ�.

	if (fDist > tSrc.fRadius + tDest.fRadius)
		return false;
	// else �浹.

	// �� ���� �߽ɻ����� �Ÿ��� �߽��� Intersect�� �����Ϸ��� �Ѵ�.

	float fLength = fDist - tDest.fRadius;

	if (fLength == 0.f)
	{
		// �� ���� �߽��� ���ΰ� ��� ���̴�.
		// �� ���� �߽��� �߾��� ���Ѵ�.

		tIntersect = (tSrc.tCenter + tDest.tCenter) / 2.f;
		return true;
	}

	if (fLength < 0.f)
	{
		// src�� dest�� �ȿ� �� �ִ� ���̴�.

		tIntersect = tSrc.tCenter;
		return true;
	}

	fLength = fDist - tSrc.fRadius;

	// ���� ���� �̹� ó��.

	if (fLength < 0.f)
	{
		// dest�� src�ȿ� �� �ִ� ���̴�.
		tIntersect = tDest.tCenter;
		return true;
	}

	// ���� ��찡 �ƴ϶��, ���������� ������ �ǰ� ���� ���� ���̴�.

	// ��ġ�� ������ ���̸� ���Ѵ�.
	float fIntersectLength = (tSrc.fRadius + tDest.fRadius) - fDist;

	fLength = fDist - tDest.fRadius + fIntersectLength / 2.f;

	// src�� �߽ɿ��� �� ���� �߽��� �߾ӱ����� �Ÿ��� ���Ѵ�.

	// �� ������ ������ ���Ѵ�.

	float fAngle = CMath::GetAngle(tSrc.tCenter, tDest.tCenter);

	// �ش� ������ ���̸� �̿��Ͽ� �浹���� ���Ѵ�.

	tIntersect.x = tSrc.tCenter.x + cosf(fAngle) * fLength;
	tIntersect.y = tSrc.tCenter.y + sinf(fAngle) * fLength;

	return true;
}

bool CMath::CollisionRectToSphere(const RectInfo & tSrc, 
	const SphereInfo & tDest, Position & tIntersect)
{
	// �簢���� ���� ��������ŭ Ȯ���ؼ�,
	// Ȯ��� �簢�� ������ ���� �߽��� ���´ٸ�, �浹�̶�� �� �� �ִ�.

	// �ٸ�, �����ڸ��� ���ؼ��� ����ó��.
	// ���� �ڸ��� �ƴ� �κп� ���ؼ��� Ȯ��� �簢�� �ȿ� ���� �߽��� ������ �浹�̴�.
	// ������, �����ڸ��� ���ؼ��� ���� �߽��� Ȯ��� �簢���� ����������
	// �浹���� �ʴ� ��찡 �ִ�.

	// ���� ��ǥ�� �簢���� �¿쿡 ���Եǰų�, �� �Ͽ� ���Ե� ��쿡��
	// Ȯ��� �簢���� ������ �Ǵ��Ѵ�.

	if ((tSrc.l <= tDest.tCenter.x && tDest.tCenter.x <= tSrc.r) ||
		(tSrc.t <= tDest.tCenter.y && tDest.tCenter.y <= tSrc.b))
	{
		RectInfo tRC;

		tRC.l = tSrc.l - tDest.fRadius;
		tRC.t = tSrc.t - tDest.fRadius;
		tRC.r = tSrc.r + tDest.fRadius;
		tRC.b = tSrc.b + tDest.fRadius;

		// Ȯ��� �簢���� ���� �߽��� �������� ���Ѵ�.

		// �ƴ� �ͺ��� �Ÿ���.
		if (tDest.tCenter.x < tRC.l)
			return false;

		else if (tDest.tCenter.x > tRC.r)
			return false;

		else if (tDest.tCenter.y < tRC.t)
			return false;

		else if (tDest.tCenter.y > tRC.b)
			return false;

		tRC.l = tDest.tCenter.x - tDest.fRadius;
		tRC.t = tDest.tCenter.y - tDest.fRadius;
		tRC.r = tDest.tCenter.x + tDest.fRadius;
		tRC.b = tDest.tCenter.y + tDest.fRadius;

		// ���� �簢���̶� �����ϰ� �浹������ ���Ѵ�.

		float	l, t, r, b;

		l = tSrc.l > tRC.l ? tSrc.l : tRC.l;
		r = tSrc.r < tRC.r ? tSrc.r : tRC.r;
		t = tSrc.t > tRC.t ? tSrc.t : tRC.t;
		b = tSrc.b < tRC.b ? tSrc.b : tRC.b;

		tIntersect.x = (l + r) / 2.f;
		tIntersect.y = (t + b) / 2.f;

		return true;
	}

	// �簢���� �����ϴ� 4���� �� �߿� �� �ȿ� �� ���� �ִٸ� �浹�̴�.
	Position tPos[4];

	tPos[0] = Position(tSrc.l, tSrc.t);
	tPos[1] = Position(tSrc.l, tSrc.b);
	tPos[2] = Position(tSrc.r, tSrc.t);
	tPos[3] = Position(tSrc.r, tSrc.b);

	for (int i = 0; i < 4; ++i)
	{
		if (CMath::GetDist(tPos[i], tDest.tCenter) < tDest.fRadius)
		{
			// ���� �߽ɰ� �� ������ �Ÿ���, ���������� ������ �浹�̴�.

			RectInfo tRC;

			tRC.l = tDest.tCenter.x - tDest.fRadius;
			tRC.t = tDest.tCenter.y - tDest.fRadius;
			tRC.r = tDest.tCenter.x + tDest.fRadius;
			tRC.b = tDest.tCenter.y + tDest.fRadius;

			// ���� �簢���̶� �����ϰ� �浹������ ���Ѵ�.

			float	l, t, r, b;

			l = tSrc.l > tRC.l ? tSrc.l : tRC.l;
			r = tSrc.r < tRC.r ? tSrc.r : tRC.r;
			t = tSrc.t > tRC.t ? tSrc.t : tRC.t;
			b = tSrc.b < tRC.b ? tSrc.b : tRC.b;

			tIntersect.x = (l + r) / 2.f;
			tIntersect.y = (t + b) / 2.f;


			return true;
		}

		// else ���� ���� �˻��Ѵ�.
	}

	return false;
}

bool CMath::CollisionRectToPoint(const RectInfo & tSrc,
	const Position & tDest, Position & tIntersect)
{
	if (tSrc.l > tDest.x)
		return false;

	else if (tSrc.t > tDest.y)
		return false;

	else if (tSrc.r < tDest.x)
		return false;

	else if (tSrc.b < tDest.y)
		return false;

	tIntersect = tDest;

	return true;
}

bool CMath::CollisionSphereToPoint(const SphereInfo & tSrc,
	const Position & tDest, Position & tIntersect)
{
	float	fDist = CMath::GetDist(tSrc.tCenter, tDest);

	if (fDist > tSrc.fRadius)
		return false;

	tIntersect = tDest;

	return true;
}

bool CMath::CollisionPointToPoint(const Position & tSrc, 
	const Position & tDest, Position & tIntersect)
{
	if ((int)tSrc.x != (int)tDest.x || (int)tSrc.y != (int)tDest.y)
		return false;

	tIntersect = tDest;

	return true;
}

bool CMath::CollisionLineToLine(const Position & tSrcSrc, const Position & tSrcDest,
	const Position & tDestSrc, const Position & tDestDest,
	Position & tIntersect)
{

	Position tSrcMove = tSrcDest - tSrcSrc;
	Position tDestMove = tDestSrc - tDestDest;
		if (tSrcMove.x == 0 || tSrcMove.y == 0)
		{
			// src�� �簢���� ���̴�.

			if (tSrcMove.x == 0)
			{
				// x�������� 0 ��, y�࿡ �����ϴ�.

				// �浹ó�� ���. 
				// �ϴ�, �簢���� ������ �ƴ� ������ ������ �������� ���ؼ�, ���⿡
				// �簢���� ���п� �ش��ϴ� x = a or y = b ���� ��ǥ�� �־ 
				// ������ ���Ѵ�. 

				// �׸��� �̷��� ���� ������ �簢���� ���� ���ο� ���� �ִٸ�. �浹�� ���̴�.

				// ������ ������ �Ѿ�� ��ǥ���� Ȯ���Ѵ�. (X)
				float MinX = tDestSrc.x < tDestDest.x ? tDestSrc.x : tDestDest.x;
				float MaxX = tDestSrc.x == MinX ? tDestDest.x : tDestSrc.x;
				

				if (tSrcSrc.x < MinX || tSrcSrc.x > MaxX)
					return false;

				// x = a��. 
				float X = tSrcSrc.x;
				
				// src : x = a
				// Dest : y = mx + b
				// y = { (y2 - y1) / (x2 / x1) }x + b
				// y = m(x - x1) + y1

				/*
					����� �� ó���� ������ ������ �� �Լ��� ȣ���ϱ� ������ ����ó���� �ϰ� �ֱ� �����̴�.
					������ �����Ŀ� �ش� X or Y ��ǥ�� �־ ������ ��ǥ�� �ش� ������ ���̴� �Ѿ����,
					�� ���� ���̶� �浹�� �����ϴ� ���� �����Ѵ�.
					������, �̷� ���� ���п� ���ؼ� ����� Rect�� �簢���� �浹������ ��
					false�� ������ ��Ȳ�̱⿡ �̸� ������ �Ǵ� ���̴�.
					�� �ٵ�, �̰� ���⼭ �� ���ܰ� �ֳ�.��������
				*/

				float fInclination = tDestMove.y / tDestMove.x;
				float fResultY = fInclination * (X - tDestSrc.x) + tDestSrc.y;

				// �̷��� ���� Y ��ǥ�� �簢���� ������ ���ο� �����Ѵٸ� �浹�̴�.

				if (fResultY > tSrcDest.y || fResultY < tSrcSrc.y)
					return false;

				else
				{
					tIntersect = Position(X, fResultY);
					return true;
				}
			}
			else
			{
				// Dest : y = mx + b
				// y = { (y2 - y1) / (x2 / x1) }x + b
				// y - y1 = m(x - x1)
				// y - y1 = mx - mx1
				// (y - y1 + mx1) / m = x 

				// ������ ������ �Ѿ�� ��ǥ���� Ȯ���Ѵ�. (Y)
				float MinY = tDestSrc.y < tDestDest.y ? tDestSrc.y : tDestDest.y;
				float MaxY = tDestSrc.y == MinY ? tDestDest.y : tDestSrc.y;


				if (tSrcSrc.y < MinY || tSrcSrc.y > MaxY)
					return false;
				

				// Src : y = a

				float Y = tSrcSrc.y;
				float fInclination = tDestMove.y / tDestMove.x;
				float fResultX = (Y - tDestSrc.y + fInclination * tDestSrc.x) / fInclination;


				if (fResultX > tSrcDest.x || fResultX < tSrcSrc.x)
					return false;

				else
				{
					tIntersect = Position(fResultX, Y);
					return true;
				}
			}
		}

		else
		{

			// dest�� �簢���� ���̴�.

			if (tDestMove.x == 0)
			{
				// y�࿡ ������ ���̴�.


				// ������ ������ �Ѿ�� ��ǥ���� Ȯ���Ѵ�. (X)
				float MinX = tSrcSrc.x < tSrcDest.x ? tSrcSrc.x : tSrcDest.x;
				float MaxX = tSrcSrc.x == MinX ? tSrcDest.x : tSrcSrc.x;


				if (tDestSrc.x < MinX || tDestSrc.x > MaxX)
					return false;

				float X = tDestSrc.x;

				// src :  y = mx + b
				// Dest : x = a
				// y = { (y2 - y1) / (x2 / x1) }x + b
				// y = m(x - x1) + y1
				// 

				float fInclination = tSrcMove.y / tSrcMove.x;
				float fResultY = fInclination * (X - tSrcSrc.x) + tSrcSrc.y;

				// �̷��� ���� Y ��ǥ�� �簢���� ������ ���ο� �����Ѵٸ� �浹�̴�.

				if (fResultY > tDestDest.y || fResultY < tDestSrc.y)
					return false;

				else
				{
					tIntersect = Position(X, fResultY);
					return true;
				}
			}
			else
			{
				// x�࿡ ������ ���̴�.

				// ������ ������ �Ѿ�� ��ǥ���� Ȯ���Ѵ�. (Y)
				float MinY = tSrcSrc.y < tSrcDest.y ? tSrcSrc.y : tSrcDest.y;
				float MaxY = tSrcSrc.y == MinY ? tSrcDest.y : tSrcSrc.y;


				if (tDestSrc.y < MinY || tDestSrc.y > MaxY)
					return false;

				// dest : y = a
				// src ������.


				float Y = tDestSrc.y;
				float fInclination = tSrcMove.y / tSrcMove.x;
				float fResultX = (Y - tSrcSrc.y + fInclination * tSrcSrc.x) / fInclination;


				if (fResultX > tDestDest.x || fResultX < tDestSrc.x)
					return false;

				else
				{
					tIntersect = Position(fResultX, Y);
					return true;
				}
			}
		}
	//}

	
	return false;
}

// �簢�� vs Line�� �浹�� �簢���� �� ���� ������ �浹��Ű�� ���̴�.
// �ϴ� �浹�� ���� ���� �� ���� ��쿡 ���Ͽ� ����ó���� �Ѵ�.
// �� ���� �׸��� ��Ʈ�� �簢���� ������ ������ �浹ó���� �غ���.
// ���⼭ false�� ������ ���а� �簢���� ���� �浹�� �� ����.

bool CMath::CollisionRectToLine(const RectInfo & tRect, const Position & tLineSrc,
	const Position & tLineDest, const RectInfo& tLineRect, Position & tIntersect)
{
	// Line���� ��������� Rect�� �浹�� ���Ѻ���.

	if (CollisionRectToRect(tRect, tLineRect,tIntersect))
	{
		// �浹 ���ɼ��� �����Ƿ� 4���� ���� ���ؼ� �浹ó���� �Ѵ�.
		Position tPos[4];
		bool IsCollision = false;

		tPos[0] = Position(tRect.l, tRect.t);
		tPos[1] = Position(tRect.r, tRect.t);
		tPos[2] = Position(tRect.l, tRect.b);
		tPos[3] = Position(tRect.r, tRect.b);

		if (CollisionLineToLine(tLineSrc, tLineDest, tPos[2], tPos[3], tIntersect))
			return true;

		else if (CollisionLineToLine(tLineSrc, tLineDest, tPos[0], tPos[2], tIntersect))
			return true;

		else if (CollisionLineToLine(tLineSrc, tLineDest, tPos[1], tPos[3], tIntersect))
			return true;

		else if (CollisionLineToLine(tLineSrc, tLineDest, tPos[0], tPos[1], tIntersect))
			return true;

	}

	return false;
}
