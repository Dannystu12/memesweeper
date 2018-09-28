#include "Minefield.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include <random>
#include <assert.h>

void MineField::Tile::SpawnMine()
{
	assert(!hasMine);
	hasMine = true;
}

bool MineField::Tile::HasMine() const
{
	return hasMine;
}

void MineField::Tile::Draw(const Vei2& pos, Graphics& gfx) const
{
	switch (state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(pos, gfx);
		break;
	case State::Flagged:
		SpriteCodex::DrawTileButton(pos, gfx);
		SpriteCodex::DrawTileFlag(pos, gfx);
		break;
	case State::Revealed:
		if (hasMine)
		{
			SpriteCodex::DrawTileBomb(pos, gfx);
		}
		else
		{
			SpriteCodex::DrawTile0(pos, gfx);
		}
		break;
	}
}

void MineField::Tile::Reveal()
{
	assert(state == State::Hidden);
	state = State::Revealed;
}

bool MineField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

MineField::MineField(int nMines)
{
	assert(nMines > 0);
	assert(nMines < width * height);

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int i = 0; i < nMines; i++)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasMine());
		TileAt(spawnPos).SpawnMine();
	}
}

void MineField::Draw(Graphics & gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize, gfx);
		}
	}
}

RectI MineField::GetRect() const
{
	return RectI(0, SpriteCodex::tileSize * width, 0, SpriteCodex::tileSize * height);
}

void MineField::OnRevealClick(const Vei2& screenPos)
{
	const Vei2 gridPos = GetGridPos(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < width);
	assert(gridPos.y >= 0 && gridPos.y < height);
	Tile& tile = TileAt(gridPos);

	if (!tile.IsRevealed())
	{
		tile.Reveal();
	}
}

MineField::Tile& MineField::TileAt(const Vei2 & gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MineField::Tile& MineField::TileAt(const Vei2 & gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MineField::GetGridPos(const Vei2 & screenPos) const
{
	return screenPos / SpriteCodex::tileSize;;
}
