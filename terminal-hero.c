/*  Terminal Hero
                           ____         ___
                         ,' __ ``.._..''   `.
                         `.`. ``-.___..-.    :
 ,---..____________________>/          _,'_  |
 `-:._,:_|_|_|_|_|_|_|_|_|_|_|.:SSt:.:|-|(/  |
                        _.' )   ____  '-'    ;
                       (    `-''  __``-'    /
                        ``-....-''  ``-..-''
    Libraries:
    ncurses - stream of keyboard input in ~real time
    fluidsynth - software synthesizer

    Compile command:
    g++ -w -o terminal-hero terminal-hero.c `pkg-config fluidsynth --libs` -lcurses
*/
#include <ncurses.h>
#include <fluidsynth.h>
#include "terminal-hero.h"

/*-------------------\
|------- MAIN -------|
\-------------------*/
int main(int argc, char **argv)
{
    // synth variables
    fluid_settings_t* _settings;
    fluid_synth_t* _synth;
    fluid_audio_driver_t* _adriver;
    int _sfont_id;

    // input and note variables
    int _inputChar;
    int _channel = 0;
    int _note = 48;
    int _velocity = 111;
    int _program = 25;

    // Create the synth and apply settings.
    _settings = new_fluid_settings();
    _synth = new_fluid_synth(_settings);
    _adriver = new_fluid_audio_driver(_settings, _synth);
    _sfont_id = fluid_synth_sfload(_synth, "Masterpiece.sf2", 1);

    // Channel 1 program
    fluid_synth_program_select(_synth, _channel, _sfont_id, 0, _program);

    // init curses
    cursesInit();

    // do our own initialization
    terminalHeroInit();

    // time
    uint64_t delta_us;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &end);

    /*-------------------\
    |----- MAIN LOOP ----|
    \-------------------*/
    while (true) {
      // clock keeping
      clock_gettime(CLOCK_MONOTONIC, &end);
      delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
      clock_gettime(CLOCK_MONOTONIC, &start);

      // doesn't seem to be needed
      // refresh();

      // book keep and update rain
      h_counter += delta_us;
      if (h_counter > MS_PER_FRAME * 1000){
        // print debug info about frames per second
        attrset(COLOR_PAIR(0)); // DEFAULT 
        mvprintw(0,0,"uSeconds per getch():\t%" PRIu64 "       ", delta_us);
        mvprintw(1,0,"uSeconds per update():\t%" PRIu64 "      ", h_counter);

        // call our update function
        update(); // this also resets the counter
      }

      // wait for keyboard input
      _inputChar = getch();

      // [Q]UIT on 'q' press
      if (_inputChar == 'q') {
        clear();
        refresh();
        break;
      } 

      /* test input char */
      switch (_inputChar) {
        case KEY_LEFT:
        case 'a':
        case 'A':
            _note = 48;
            if (y1 == FINISH_LINE) playNote(_synth, _channel, _note + 12, _velocity);
            break;

        case KEY_DOWN:
        case 's':
        case 'S':
            _note = 50;
            if (y2 == FINISH_LINE) playNote(_synth, _channel, _note + 12, _velocity);
            break;

        case KEY_UP:
        case 'd':
        case 'D':
            _note = 51;
            if (y3 == FINISH_LINE) playNote(_synth, _channel, _note + 12, _velocity);
            break;

        case KEY_RIGHT:
        case 'f':
        case 'F':
            _note = 55;
            if (y4 == FINISH_LINE) playNote(_synth, _channel, _note + 12, _velocity);
            break;

        // -1 represents no character, skip playing note
        case -1:
          continue;

        default:
          continue;
        }
    }

    /* Clean up fluidsynth */
    delete_fluid_audio_driver(_adriver);
    delete_fluid_synth(_synth);
    delete_fluid_settings(_settings);

    /* Clean up curses */
    endwin();

    /* Say Goodbye */
    std::cout << std::endl <<  "Thanks for playing!" << std::endl;

    /* End program successfully */
    return 0;
}

/*--------------------------\
|-FUNCTION IMPLEMENTATIONS -|
\--------------------------*/
void update(void)
{
  // make it rain
  make_it_rain();

  // reset the counter
  h_counter = 0;
}

void make_it_rain(void)
{
  // mvaddch(y1, NOTE_ONE_X, ACS_ULCORNER);
  // mvaddch(y1, NOTE_ONE_X+1, ACS_URCORNER);
  // mvaddch(y1+1, NOTE_ONE_X, ACS_LLCORNER);
  // mvaddch(y1+1, NOTE_ONE_X+1, ACS_LRCORNER);
  if (y1++ >= FINISH_LINE) {
    y1 = BOARD_START_Y + 1;
  }
  if (y2++ >= FINISH_LINE) {
    y2 = BOARD_START_Y + 1;
  }
  if (y3++ >= FINISH_LINE) {
    y3 = BOARD_START_Y + 1;
  }
  if (y4++ >= FINISH_LINE) {
    y4 = BOARD_START_Y + 1;
  }

  mvaddch(y1-1, NOTE_ONE_X, ERASE);
  mvaddch(y2-1, NOTE_TWO_X, ERASE);
  mvaddch(y3-1, NOTE_THREE_X, ERASE);
  mvaddch(y4-1, NOTE_FOUR_X, ERASE);

  attrset(COLOR_PAIR(2));
  mvaddch(y1, NOTE_ONE_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(1));
  mvaddch(y2, NOTE_TWO_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(3));
  mvaddch(y3, NOTE_THREE_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(4));
  mvaddch(y4, NOTE_FOUR_X, ACS_DIAMOND);
}

void terminalHeroInit(void)
{
  // Prepare world
  draw_board();

  // set our notes up
  y1 = BOARD_START_Y;
  y2 = BOARD_START_Y+4;
  y3 = BOARD_START_Y+2;
  y4 = BOARD_START_Y+4;

  // init counter
  h_counter = 0;
}

void draw_board(void)
{
  attrset(COLOR_PAIR(7));
  mvaddch(BOARD_START_Y-1, BOARD_START_X, ACS_ULCORNER);
  mvaddch(BOARD_START_Y-1, BOARD_START_X + BOARD_WIDTH, ACS_URCORNER);
  mvhline(BOARD_START_Y-1, BOARD_START_X+1, ACS_HLINE, BOARD_WIDTH-1);
  mvvline(BOARD_START_Y, BOARD_START_X, ACS_VLINE, BOARD_HEIGHT);
  mvvline(BOARD_START_Y, BOARD_START_X + BOARD_WIDTH, ACS_VLINE, BOARD_HEIGHT);
  mvhline(FINISH_LINE, BOARD_START_X+1, ACS_HLINE, BOARD_WIDTH-1);
  mvaddch(FINISH_LINE, BOARD_START_X, ACS_LLCORNER);
  mvaddch(FINISH_LINE, BOARD_START_X + BOARD_WIDTH, ACS_LRCORNER);

  mvaddch(BOARD_START_Y-1, NOTE_ONE_X, ERASE);
  mvaddch(BOARD_START_Y-1, NOTE_TWO_X, ERASE);
  mvaddch(BOARD_START_Y-1, NOTE_THREE_X, ERASE);
  mvaddch(BOARD_START_Y-1, NOTE_FOUR_X, ERASE);

  attrset(COLOR_PAIR(2)); // GREEN
  mvaddch(FINISH_LINE+1, NOTE_ONE_X, 'A');
  mvaddch(FINISH_LINE, NOTE_ONE_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(1)); // RED
  mvaddch(FINISH_LINE+1, NOTE_TWO_X, 'S');
  mvaddch(FINISH_LINE, NOTE_TWO_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(3)); // YELLOW
  mvaddch(FINISH_LINE+1, NOTE_THREE_X, 'D');
  mvaddch(FINISH_LINE, NOTE_THREE_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(4)); // BLUE 
  mvaddch(FINISH_LINE+1, NOTE_FOUR_X, 'F');
  mvaddch(FINISH_LINE, NOTE_FOUR_X, ACS_DIAMOND);
  

  move(FINISH_LINE+2, 0);

  attrset(COLOR_PAIR(0)); // DEFAULT 
  printw("                           ____         ___\n");
  printw("                         ,' __ ``.._..''   `.\n");
  printw("                         `.`. ``-.___..-.    :\n");
  printw(" ,---..____________________>/          _,'_  |\n");
  printw(" `-:._,:_|_|_|_|_|_|_|_|_|_|_|.:SSt:.:|-|(/  |\n");
  printw("                        _.' )   ____  '-'    ;\n");
  printw("                       (    `-''  __``-'    /\n");
  printw("                        ``-....-''  ``-..-''\n");
}

void cursesInit(void)
{
    // init screen
    initscr();

    // tell curses not to do NL->CR/NL on output
    nonl();
    intrflush(stdscr, FALSE);

    // grant ability to read arrow keays
    keypad(stdscr, true);

    // do not print each char to the screen
    noecho();
    // echo();

    // cbreak() disables line buffering and erase/kill character-processing 
    // (interrupt and flow control characters are unaffected), 
    // making characters typed by the user immediately available to the program.
    cbreak();

    // The nodelay option causes getch to be a non-blocking call. 
    // If no input is ready, getch returns ERR. If disabled (bf is FALSE), 
    // getch waits until a key is pressed.
    nodelay(stdscr, true);

    // hide the default screen cursor.
    curs_set( 0 );

    if (has_colors()) {
        start_color();
        // Notes: color pair 0 cannotbe redefined.
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }
}

// Play a note on the synth.
void playNote(fluid_synth_t* synth, int channel, int note, int velocity)
{
  /* Play a note */
  fluid_synth_noteon(synth, channel, note, velocity);
  /* Stop the note */
  // fluid_synth_noteoff(synth, channel, note);
}
