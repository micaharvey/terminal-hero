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
int main(int argc, char **argv) {
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
    int _program = 0;

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

    /*-------------------\
    |----- MAIN LOOP ----|
    \-------------------*/
    while (true) {
      refresh();

      // book keep and update rain
      if (h_counter++ > ITERATIONS_PER_UPDATE){
        update();
      }

      // wait for keyboard input
      _inputChar = getch();

      // [Q]UIT on 'q' press
      if (_inputChar == 'q') {
        clear();
        refresh();
        break;
      } 

      /* test inputted key and determine direction */
      switch (_inputChar) {
        case KEY_UP:
        case 'w':
        case 'W':
            _note = 60;
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            _note = 48;
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            _note = 55;
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            _note = 51;
            break;

        // -1 represents no character, skip playing note
        case -1:
          continue;

        default:
          continue;
        }

        playNote(_synth, _channel, _note, _velocity);
    }

    /* Clean up synth */
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
  make_it_rain();
  // reset the counter
  h_counter = 0;
}

void make_it_rain(void)
{
  // erase trail
  mvaddch(h_y-2, h_x, ERASE);
  /* by default, you get a blinking cursor - but it is turned off */
  mvaddch(h_y, h_x, NOTE);
  // move(h_y, h_x);
  // make_it_rain();
  if (++h_y > LINES-1 ) {
    mvaddch(LINES-1, h_x, ERASE);
    mvaddch(LINES-2, h_x, ERASE);
    h_y = 3;
  }
}

void terminalHeroInit(void) {
  // Prepare world
    draw_board();
    h_y = 3;
    h_x = 11;
    h_counter = 0;
}

void draw_board(void)
{
  mvvline(3, LINES, '|', COLS);
  mvvline(3, 9, '|', COLS);
}

void cursesInit(void){
      // init screen
    initscr();

    nonl(); /* tell curses not to do NL->CR/NL on output */
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

    curs_set( 0 ); // hide the default screen cursor.

    if (has_colors()) {
        start_color();
        // Notes: color pair 0 cannotbe redefined.
        init_pair(1, COLOR_RED,     COLOR_WHITE);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_WHITE);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }
}

// Play a note on the synth.  This will only do a "stab" not a held note currently.
void playNote(fluid_synth_t* synth, int channel, int note, int velocity) {
  /* Play a note */
  fluid_synth_noteon(synth, channel, note, velocity);
  /* Stop the note */
  fluid_synth_noteoff(synth, channel, note);
}
