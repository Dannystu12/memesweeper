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

void MineField::Tile::Draw(const Vei2& screenPos, GameState gameState, Graphics& gfx) const
{
	if (gameState == GameState::Playing)
	{
		switch (state)
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		case State::Flagged:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			SpriteCodex::DrawTileFlag(screenPos, gfx);
			break;
		case State::Revealed:
			if (hasMine)
			{

				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighbourMines, gfx);
			}
			break;
		}
	}
	else
	{
		switch (state)
		{
		case State::Hidden:
			if (hasMine)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighbourMines, gfx);
			}
			break;
		case State::Flagged:
			if (hasMine)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighbourMines, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (hasMine)
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighbourMines, gfx);
			}
			break;
		}
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

bool MineField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

bool MineField::Tile::HasMine() const
{
	return hasMine;
}

int MineField::Tile::GetNeighbourMineCount()
{
	return nNeighbourMines;
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
	assert(nNeighbourMines == -1);
	assert(mineCount >= 0 && mineCount <= 8);
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
	DrawBorder(gfx);
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	const Vei2 padding(xPadding, yPadding);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize + padding, gameState, gfx);
		}
	}

	if (gameState == GameState::Win)
	{
		SpriteCodex::DrawWin({Graphics::ScreenWidth / 2,
			Graphics::ScreenHeight / 2 }, 
			gfx);
	}
}

RectI MineField::GetRect() const
{
	return RectI(xPadding, xPadding + SpriteCodex::tileSize * width, yPadding, yPadding + SpriteCodex::tileSize * height);
}

bool MineField::OnRevealClick(const Vei2& screenPos)
{
	if (gameState == GameState::Playing)
	{
		const Vei2 gridPos = GetGridPos(screenPos);
		assert(gridPos.x >= 0 && gridPos.x < width);
		assert(gridPos.y >= 0 && gridPos.y < height);
		Tile& tile = TileAt(gridPos);

		if (tile.IsHidden())
		{
			
			if (tile.HasMine())
			{
				tile.Reveal();
				gameState = GameState::Lose;
				return true;
			}
			else if (tile.GetNeighbourMineCount() == 0)
			{
				RevealAdjacentTiles(gridPos);
			}
			else
			{
				tile.Reveal();
				CheckForWin();
			}

		}
	}
	return false;
}

void MineField::OnFlagClick(const Vei2 & screenPos)
{
	if (gameState != GameState::Playing) return;
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
	const Vei2 padding(xPadding, yPadding);
	return (screenPos - padding) / SpriteCodex::tileSize;
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

void MineField::DrawBorder(Graphics & gfx) const
{
	gfx.DrawRect(xPadding - SpriteCodex::tileSize, 
		yPadding - SpriteCodex::tileSize, 
		xPadding + width * SpriteCodex::tileSize + SpriteCodex::tileSize, 
		yPadding + height * SpriteCodex::tileSize + SpriteCodex::tileSize, borderColor);
}

void MineField::CheckForWin()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			const Vei2 gridPos(x, y);
			if (TileAt(gridPos).IsHidden() && !TileAt(gridPos).HasMine())
			{
				return;

			}
		}
	}
	gameState = GameState::Win;
}

void MineField::RevealAdjacentTiles(const Vei2& gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int xEnd = std::min(width - 1, gridPos.x + 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int yEnd = std::min(height - 1, gridPos.y + 1);

	Tile& tile = TileAt(gridPos);

	if (!tile.IsHidden() || tile.HasMine() || tile.IsFlagged()) return;
	else
	{
		tile.Reveal();
		if (tile.GetNeighbourMineCount() > 0) return;
		for (Vei2 pos = { xStart, yStart }; pos.y <= yEnd; pos.y++)
		{
			for (pos.x = xStart; pos.x <= xEnd; pos.x++)
			{
				RevealAdjacentTiles(pos);
			}
		}
	}




}
