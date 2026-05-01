    #pragma once
    #include "bullets.h"
    #include "ships.h"

    extern Gun FiveInch;
    extern Gun EightInch;
    extern Gun SixteenInch;

    extern Battery DestroyerLoadout[SHIP_MAXBATTERIES];
    extern Battery BattleshipLoadout[SHIP_MAXBATTERIES];

    extern Ship DestroyerStats;


    extern Ship BattleshipStats;

    void MakeLoadouts();
