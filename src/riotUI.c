#include "riotUI.h"
#include "riotUnits.h"

void uiSet(enum GameMode gameMode, struct Interface *win) {

    int y, x;

    switch (gameMode) {

        case INIT:
            initscr();
            noecho(); // hide keypresses
            curs_set(FALSE); // hide cursor

            /* Verify terminal dimensions */
            getmaxyx(stdscr, y, x);
            if ((x < MAX_COLS) || (y < MAX_ROWS))
                quit("Terminal size too small");

            /* Set window positions*/
            win->header = newwin(HEADER, MAX_COLS, 0, 0);
            win->body = newwin(MAIN, MAX_COLS, HEADER, 0);
            win->footer = newwin(FOOTER, MAX_COLS, HEADER + MAIN, 0);
            win->menu = newwin(MAX_ROWS, MAX_COLS, 0, 0);
            break;

        case MENU:
            wrefresh(win->menu);
            break;

        case NEW:
        case CONTINUE:
        case PLAY:
            break;

        case EXIT:
            if (win->header) delwin(win->header);
            if (win->body) delwin(win->body);
            if (win->footer) delwin(win->footer);
            if (win->menu) delwin(win->menu);
            endwin();
            break;

        default:
            quit("Invalid game mode initialized");
            break;
    }
    return;
}

enum GameMode menuMain(struct Interface *gameInterface) {

    enum GameMode gameMode=_GAME_MODE_LIMIT;
    WINDOW *menu = gameInterface->menu;

    int y = 3;

    wclear(menu);
    box(menu, 0, 0);
    mvwaddstr(menu, y++, 21, "8888888b. 8888888 .d88888b.88888888888");
    mvwaddstr(menu, y++, 21, "888   Y88b  888  d88P   Y88b   888");
    mvwaddstr(menu, y++, 21, "888    888  888  888     888   888");
    mvwaddstr(menu, y++, 21, "888   d88P  888  888     888   888");
    mvwaddstr(menu, y++, 21, "8888888P    888  888     888   888");
    mvwaddstr(menu, y++, 21, "888 T88b    888  888     888   888");
    mvwaddstr(menu, y++, 21, "888  T88b   888  Y88b. .d88P   888");
    mvwaddstr(menu, y++, 21, "888   T88b8888888  Y88888P     888");

    mvwhline(menu, y += 2, 21, ACS_HLINE, 37);

    mvwaddstr(menu, y += 3, 21, "GAME MENU");
    y += 2;
    mvwaddstr(menu, y++, 21, "[n]ew game");
    mvwaddstr(menu, y++, 21, "[c]ontinue");
    mvwaddstr(menu, y++, 21, "[e]xit");

    wrefresh(menu);

    while (gameMode != 'n' && gameMode != 'c' && gameMode != 'e')
        gameMode = wgetch(menu);

    return gameMode;
}


short menuContinue(struct Interface *gameInterface, struct MapList *mapList) {

    WINDOW *menu = gameInterface->menu;
    struct Map *current, *last;
    int select;
    int y = 3;
    bool unlocked[MAX_LEVELS];

    wclear(menu);
    box(menu, 0, 0);

    /* Print header information */
    mvwaddstr(menu, y, 21, "LEVEL SELECT");
    mvwhline(menu, y += 2, 21, ACS_HLINE, 37);
    y += 2;

    /* Always print first level */
    mvwprintw(menu, y, 21, "[0] %s", mapList->level[0].name);
    unlocked[0] = true;

    /* Print additional levels */
    for (int i = 1; i < mapList->count; i++) {
        current = &mapList->level[i];
        last = &mapList->level[i - 1];

        if (!current->hidden) {
            mvwprintw(menu, y + i, 21, "[%c] %s",
                last->beaten ? i + '0' : '-', current->name);
        } else if (current->hidden && last->beaten) {
            mvwprintw(menu, y + i, 21, "[%i] %s", i, current->name);
        } else y--;

        /* Set unlocked state */
        unlocked[i] = current->beaten;
    }
    mvwaddstr(menu, MAX_ROWS - 4, 21, "[b]ack");
    wrefresh(menu);

    /* Get user input */
    do {
        select = (char)wgetch(menu);
        if (select == 'b') return -1;
        if (select -'0' < 0 || select > MAX_LEVELS) continue;
    } while (!unlocked[select-'0']);

    return (short) (select - '0');
}


void drawLevel(struct Interface *win, struct MapList *ml, short rep, short panic, int lvl) {

    struct Map *m = &ml->level[lvl];
    int y;

    /* Draw Header */ 
    mvwprintw (win->header, 1, 1, "Level %d:%s", lvl, getLevelName(lvl)); // Display Level
    mvwprintw (win->header, 1, MAX_COLS-24, "Panic:%d%%",panic); // Display Panic
    mvwprintw (win->header, 1, MAX_COLS-11, "Rep:%d",rep); // Display Rep

    /* Draw Footer */ 
    mvwprintw (win->footer, 1, 1, "INMATES");
    mvwprintw (win->footer, 1, 15, "[h]omeboy(10)\t[b]ruiser(16)\t[l]unatic(16)\t[f]atty(60)");
    mvwprintw (win->footer, 2, 15, "[s]peedy(10)\t[c]utie(20)\t[a]ttorney[30]\t[d]octer(10)");


    /* Draw the game map */
    for (y = 0; y < MAP_ROWS; y++)
        mvwprintw (win->body, y, 0, m->overlay[y]);
    wrefresh(win->body);

    /* Draw window borders around windows */
    box(win->header, 0, 0);
    box(win->footer, 0, 0);
    mvwaddch(win->footer, 0, 0, ACS_LTEE);
    mvwaddch(win->footer, 0, MAX_COLS - 1, ACS_RTEE);
    mvwaddch(win->header, 0, 0, ACS_ULCORNER);
    mvwaddstr(win->header, 0, 1, "riot");
    mvwaddch (win->header, 2, 0, ACS_LTEE);
    mvwaddch (win->header, 2, MAX_COLS - 1, ACS_RTEE);
    for (y = 0; y < MAX_ROWS; y++) {
        mvwaddch(win->body, y, 0, ACS_VLINE);
        mvwaddch(win->body, y, MAX_COLS - 1, ACS_VLINE);
    }

    /* Draw Queue */
    //TODO

    /* Refresh windows */
    wrefresh(win->body);
    wrefresh(win->header);
    wrefresh(win->body);
    wrefresh(win->footer);

    //TODO

    getchar(); //temporary

    return;
}

char * getLevelName (int level) {
    switch(level) {
        case 0:
            return "Detention (Classroom)";
        case 1:
            return "The Drunk Tank";
        case 2:
            return "Minimum Security Facility";
        case 3:
            return "Tijuana Lockup";
        case 4:
            return "Medium Security Lockup";
        case 5:
            return "Supermax";
        case 6:
            return "Guantanamo";
        default:
            return 0;
    }
}

void redrawUnit(char unitType, int health, int currentPosition, int newPosition) {

}

void drawUnit(char unitType,int health, int position) {

 }

void eraseUnit(int position) {

}

void drawTile(char type, int position) {
    
}
