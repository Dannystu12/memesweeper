#pragma once
#include "Vei2.h"
#include "Graphics.h"
#include "RectI.h"
#include "SpriteCodex.h"
#include "Colors.h"
#include "Sound.h"

class MineField
{
public:
	enum class GameState
	{
		Playing,
		Win,
		Lose
	};
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMine();
		void Draw(const Vei2& screenPos, MineField::GameState gameState, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		bool IsHidden() const;
		bool HasMine() const;
		void ToggleFlag();
		void SetNeighbourMineCount(int mineCount);
	private:
		State state = State::Hidden;
		bool hasMine = false;
		int nNeighbourMines = -1;
	};

public:
	MineField(int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	bool OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
private:
	Tile & TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 GetGridPos(const Vei2& screenPos ) const;
	int CountNeighboursMines(const Vei2& gridPos);
	void DrawBorder(Graphics& gfx) const;
	void CheckForWin();
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	static constexpr int xPadding = Graphics::ScreenWidth / 2 - width / 2 * SpriteCodex::tileSize;
	static constexpr int yPadding = Graphics::ScreenHeight / 2 - height / 2 * SpriteCodex::tileSize;
	static constexpr Color borderColor = Colors::Blue;
	GameState gameState = GameState::Playing;
	Tile field[width * height];
};