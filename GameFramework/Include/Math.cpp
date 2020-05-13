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

	// cos 밑변 / 빗변 
	// 코사인 값을 구한다.
	float fAngle = fWidth / fDist;

	// 코사인 값을 이용하여 각도를 구한다.
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

	// 두 충돌영역의 가운데를 구한다.

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

	// 두 원의 중심사이의 거리를 구한다.
	float fDist = CMath::GetDist(tSrc.tCenter, tDest.tCenter);

	// 두 원 중심사이의 거리가 두 원의 반지름의 합보다 더 크면. 충돌이 아니다.

	if (fDist > tSrc.fRadius + tDest.fRadius)
		return false;
	// else 충돌.

	// 두 원의 중심사이의 거리의 중심을 Intersect에 저장하려고 한다.

	float fLength = fDist - tDest.fRadius;

	if (fLength == 0.f)
	{
		// 두 원의 중심을 서로가 밟는 것이다.
		// 두 원의 중심의 중앙을 구한다.

		tIntersect = (tSrc.tCenter + tDest.tCenter) / 2.f;
		return true;
	}

	if (fLength < 0.f)
	{
		// src가 dest의 안에 들어가 있는 것이다.

		tIntersect = tSrc.tCenter;
		return true;
	}

	fLength = fDist - tSrc.fRadius;

	// 같은 경우는 이미 처리.

	if (fLength < 0.f)
	{
		// dest가 src안에 들어가 있는 것이다.
		tIntersect = tDest.tCenter;
		return true;
	}

	// 위의 경우가 아니라면, 누군가에게 포함이 되고 있지 않은 것이다.

	// 겹치는 구간의 길이를 구한다.
	float fIntersectLength = (tSrc.fRadius + tDest.fRadius) - fDist;

	fLength = fDist - tDest.fRadius + fIntersectLength / 2.f;

	// src의 중심에서 두 원의 중심의 중앙까지의 거리를 구한다.

	// 두 점간의 각도를 구한다.

	float fAngle = CMath::GetAngle(tSrc.tCenter, tDest.tCenter);

	// 해당 각도와 길이를 이용하여 충돌점을 구한다.

	tIntersect.x = tSrc.tCenter.x + cosf(fAngle) * fLength;
	tIntersect.y = tSrc.tCenter.y + sinf(fAngle) * fLength;

	return true;
}

bool CMath::CollisionRectToSphere(const RectInfo & tSrc, 
	const SphereInfo & tDest, Position & tIntersect)
{
	// 사각형을 원의 반지름만큼 확장해서,
	// 확장된 사각형 안으로 원의 중심이 들어온다면, 충돌이라고 할 수 있다.

	// 다만, 가장자리에 대해서는 예외처리.
	// 가장 자리가 아닌 부분에 대해서는 확장된 사각형 안에 원의 중심이 들어오면 충돌이다.
	// 하지만, 가장자리에 대해서는 원의 중심이 확장된 사각형에 들어왔음에도
	// 충돌하지 않는 경우가 있다.

	// 원의 좌표가 사각형의 좌우에 포함되거나, 상 하에 포함될 경우에는
	// 확장된 사각형을 가지고 판단한다.

	if ((tSrc.l <= tDest.tCenter.x && tDest.tCenter.x <= tSrc.r) ||
		(tSrc.t <= tDest.tCenter.y && tDest.tCenter.y <= tSrc.b))
	{
		RectInfo tRC;

		tRC.l = tSrc.l - tDest.fRadius;
		tRC.t = tSrc.t - tDest.fRadius;
		tRC.r = tSrc.r + tDest.fRadius;
		tRC.b = tSrc.b + tDest.fRadius;

		// 확장된 사각형에 원의 중심이 들어오는지 비교한다.

		// 아닌 것부터 거른다.
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

		// 원을 사각형이라 가정하고 충돌영역을 구한다.

		float	l, t, r, b;

		l = tSrc.l > tRC.l ? tSrc.l : tRC.l;
		r = tSrc.r < tRC.r ? tSrc.r : tRC.r;
		t = tSrc.t > tRC.t ? tSrc.t : tRC.t;
		b = tSrc.b < tRC.b ? tSrc.b : tRC.b;

		tIntersect.x = (l + r) / 2.f;
		tIntersect.y = (t + b) / 2.f;

		return true;
	}

	// 사각형을 구성하는 4개의 점 중에 원 안에 들어간 것이 있다면 충돌이다.
	Position tPos[4];

	tPos[0] = Position(tSrc.l, tSrc.t);
	tPos[1] = Position(tSrc.l, tSrc.b);
	tPos[2] = Position(tSrc.r, tSrc.t);
	tPos[3] = Position(tSrc.r, tSrc.b);

	for (int i = 0; i < 4; ++i)
	{
		if (CMath::GetDist(tPos[i], tDest.tCenter) < tDest.fRadius)
		{
			// 원의 중심과 점 사이의 거리가, 반지름보다 작으면 충돌이다.

			RectInfo tRC;

			tRC.l = tDest.tCenter.x - tDest.fRadius;
			tRC.t = tDest.tCenter.y - tDest.fRadius;
			tRC.r = tDest.tCenter.x + tDest.fRadius;
			tRC.b = tDest.tCenter.y + tDest.fRadius;

			// 원을 사각형이라 가정하고 충돌영역을 구한다.

			float	l, t, r, b;

			l = tSrc.l > tRC.l ? tSrc.l : tRC.l;
			r = tSrc.r < tRC.r ? tSrc.r : tRC.r;
			t = tSrc.t > tRC.t ? tSrc.t : tRC.t;
			b = tSrc.b < tRC.b ? tSrc.b : tRC.b;

			tIntersect.x = (l + r) / 2.f;
			tIntersect.y = (t + b) / 2.f;


			return true;
		}

		// else 다음 것을 검사한다.
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
			// src가 사각형의 선이다.

			if (tSrcMove.x == 0)
			{
				// x증가량이 0 즉, y축에 평행하다.

				// 충돌처리 방식. 
				// 일단, 사각형의 선분이 아닌 것으로 직선의 방정식을 구해서, 여기에
				// 사각형의 선분에 해당하는 x = a or y = b 꼴의 좌표를 넣어서 
				// 교점을 구한다. 

				// 그리고 이렇게 구한 교점이 사각형의 선분 내부에 들어와 있다면. 충돌인 것이다.

				// 선분의 범위를 넘어가는 좌표인지 확인한다. (X)
				float MinX = tDestSrc.x < tDestDest.x ? tDestSrc.x : tDestDest.x;
				float MaxX = tDestSrc.x == MinX ? tDestDest.x : tDestSrc.x;
				

				if (tSrcSrc.x < MinX || tSrcSrc.x > MaxX)
					return false;

				// x = a꼴. 
				float X = tSrcSrc.x;
				
				// src : x = a
				// Dest : y = mx + b
				// y = { (y2 - y1) / (x2 / x1) }x + b
				// y = m(x - x1) + y1

				/*
					참고로 이 처리가 가능한 이유는 이 함수를 호출하기 이전에 예외처리를 하고 있기 때문이다.
					직선의 방정식에 해당 X or Y 좌표를 넣어서 나오는 좌표가 해당 선분의 길이는 넘어갔지만,
					변 위의 점이라서 충돌이 성립하는 경우는 존재한다.
					하지만, 이런 경우는 선분에 의해서 생기는 Rect와 사각형을 충돌시켰을 때
					false가 나오는 상황이기에 미리 배제가 되는 것이다.
					하 근데, 이거 여기서 또 예외가 있네.ㅋㅋㅋㅋ
				*/

				float fInclination = tDestMove.y / tDestMove.x;
				float fResultY = fInclination * (X - tDestSrc.x) + tDestSrc.y;

				// 이렇게 구한 Y 좌표가 사각형의 직선의 내부에 존재한다면 충돌이다.

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

				// 선분의 범위를 넘어가는 좌표인지 확인한다. (Y)
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

			// dest가 사각형의 선이다.

			if (tDestMove.x == 0)
			{
				// y축에 평행한 선이다.


				// 선분의 범위를 넘어가는 좌표인지 확인한다. (X)
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

				// 이렇게 구한 Y 좌표가 사각형의 직선의 내부에 존재한다면 충돌이다.

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
				// x축에 평행한 선이다.

				// 선분의 범위를 넘어가는 좌표인지 확인한다. (Y)
				float MinY = tSrcSrc.y < tSrcDest.y ? tSrcSrc.y : tSrcDest.y;
				float MaxY = tSrcSrc.y == MinY ? tSrcDest.y : tSrcSrc.y;


				if (tDestSrc.y < MinY || tDestSrc.y > MaxY)
					return false;

				// dest : y = a
				// src 방정식.


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

// 사각형 vs Line의 충돌은 사각형의 네 변과 선분을 충돌시키는 것이다.
// 일단 충돌이 절대 나올 수 없는 경우에 대하여 예외처리를 한다.
// 두 점이 그리는 렉트와 사각형의 정보를 가지고 충돌처리를 해본다.
// 여기서 false가 나오면 선분과 사각형은 절대 충돌일 수 없다.

bool CMath::CollisionRectToLine(const RectInfo & tRect, const Position & tLineSrc,
	const Position & tLineDest, const RectInfo& tLineRect, Position & tIntersect)
{
	// Line으로 만들어지는 Rect와 충돌을 시켜본다.

	if (CollisionRectToRect(tRect, tLineRect,tIntersect))
	{
		// 충돌 가능성이 있으므로 4개의 변에 대해서 충돌처리를 한다.
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
