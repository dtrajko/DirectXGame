float4 main( float4 pos : POSITION ) : SV_POSITION
{
	if (pos.y > 0 && pos.y < 1)
	{
		pos.x += 0.25f;
	}

	if (pos.y > -1 && pos.y < 0 && pos.x > 0 && pos.x < 1)
	{
		pos.y += 0.25f;
	}

	if (pos.y > 0 && pos.y < 1 && pos.x > -1 && pos.x < 0)
	{
		pos.y -= 0.25f;
	}

	return pos;
}
