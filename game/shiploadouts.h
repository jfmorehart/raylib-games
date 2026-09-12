    #pragma once
    #include "bullets.h"
    #include "ships.h"
    #include "cutscene.h"

    extern Gun FiveInch;
    extern Gun EightInch;
    extern Gun SixteenInch;

    extern Battery DestroyerLoadout[SHIP_MAXBATTERIES];
    extern Battery BattleshipLoadout[SHIP_MAXBATTERIES];

    extern Ship DestroyerStats;


    extern Ship BattleshipStats;

    void MakeLoadouts();

    void InitRvecs(Ship *ship);


    PolyPoly cruiser;
    PolyPoly destroyer;
    PolyPoly battleship;

    void LoadShipIcons();

    void CreateWordBank();
    void CreateCaptainName_NonAlloc(char * writeTo);
    void CreateShipName_NonAlloc(char * writeTo);
