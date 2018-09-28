#pragma once
#include "Vei2.h"
#include "Graphics.h"
#include "RectI.h"

class MineField
{
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
		bool HasMine() const;
		void Draw(const Vei2& pos, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
	private:
		State state = State::Hidden;
		bool hasMine = false;
	};
public:
	MineField(int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2& screenPos);
private:
	Tile & TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 GetGridPos(const Vei2& screenPos ) const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
};