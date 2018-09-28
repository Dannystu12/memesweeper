#include "Minefield.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include <random>
#include <assert.h>
#include <algorithm>

void MineField::Tile::SpawnMine()
{
	assert(!hasMine);
	hasMine = true;
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
		else if(nNeighbourMines > 0)
		{
			SpriteCodex::DrawTileNumber(pos, nNeighbourMines, gfx);
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

bool MineField::Tile::IsHidden() const
{
	return state == State::Hidden;
}

bool MineField::Tile::HasMine() const
{
	return hasMine;
}

void MineField::Tile::ToggleFlag()
{
	assert(!IsRevealed());
	if (state == State::Hidden)
	{
		state = State::Flagged;
	}
	else
	{
		state = State::Hidden;
	}
}

void MineField::Tile::SetNeighbourMineCount(int mineCount)
{
	assert(mineCount >= 0 && mineCount <= 9);
	nNeighbourMines = mineCount;
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

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile& tile = TileAt({ x, y });
			if (!tile.HasMine())
			{
				tile.SetNeighbourMineCount(CountNeighboursMines({x, y}));
			}
		}
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

	if (tile.IsHidden())
	{
		tile.Reveal();
	}
}

void MineField::OnFlagClick(const Vei2 & screenPos)
{
	const Vei2 gridPos = GetGridPos(screenPos);
	assert(gridPos.x >= 0 && gridPos.x < width);
	assert(gridPos.y >= 0 && gridPos.y < height);
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed())
	{
		tile.ToggleFlag();
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

int MineField::CountNeighboursMines(const Vei2 & gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int xEnd = std::min(width - 1, gridPos.x + 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int yEnd = std::min(height - 1, gridPos.y + 1);

	int mineCount = 0;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			if (TileAt({ x, y }).HasMine())
			{
				mineCount++;
			}
		}
	}


	return mineCount;
}
