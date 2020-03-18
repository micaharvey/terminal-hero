/*  Terminal Hero
                           ____         ___
                         ,' __ ``.._..''   `.
                         `.`. ``-.___..-.    :
 ,---..____________________>/          _,'_  |
 `-:._,:_|_|_|_|_|_|_|_|_|_|_|.:SSt:.:|-|(/  |
                        _.' )   ____  '-'    ;
                       (    `-''  __``-'    /
                        ``-....-''  ``-..-''
*/
#include "terminal-hero.h"

/*-------------------\
|------- MAIN -------|
\-------------------*/
int main(int argc, char **argv)
{
  Options options;
  options.process(argc, argv);
  if (options.getArgCount() == 0) midifile.read("resources/midi-files/twinkle_twinkle.mid");
  else midifile.read(options.getArg(1));
  midifile.doTimeAnalysis();
  midifile.linkNotePairs();

  int tracks = midifile.getTrackCount();
  if (DEBUG) cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
  if (DEBUG) if (tracks > 1) cout << "TRACKS: " << tracks << endl;
  for (int track = 0; track < tracks; track++) {
    if (DEBUG) if (tracks > 1) cout << "\nTrack " << track << endl;
    if (DEBUG) cout << "Tick\tSeconds\tDur\tMessage" << endl;
    for (int event = 0; event < midifile[track].size(); event++) {
      if (DEBUG) cout << dec << midifile[track][event].tick;
      if (DEBUG) cout << '\t' << dec << midifile[track][event].seconds;
      if (DEBUG) cout << '\t';
      if (midifile[track][event].isNoteOn())
        if (DEBUG) cout << midifile[track][event].getDurationInSeconds();
        else if (midifile[track][event].isMeta() && midifile[track][event].isTempo()) {
          if (DEBUG) cout << midifile[track][event].getTempoBPM();
          BPM = midifile[track][event].getTempoBPM();
        }
      if (DEBUG) cout << '\t' << hex;
      for (int i = 0; i < midifile[track][event].size(); i++)
        if (DEBUG) cout << (int)midifile[track][event][i] << ' ';
      if (DEBUG) cout << endl;
    }
  }

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

  // Reset to Piano
  _program = 0;

  // Create the synth and apply settings.
  _settings = new_fluid_settings();
  _synth = new_fluid_synth(_settings);
  _adriver = new_fluid_audio_driver(_settings, _synth);
  _sfont_id = fluid_synth_sfload(_synth, "resources/sound-fonts/Masterpiece.sf2", 1);

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
    // clock keeping
    clock_gettime(CLOCK_MONOTONIC, &loopEndTime);
    clock_gettime(CLOCK_MONOTONIC, &nowTime);
    delta_us = (loopEndTime.tv_sec - loopStartTime.tv_sec) * 1000000 + (loopEndTime.tv_nsec - loopStartTime.tv_nsec) / 1000;
    now = (nowTime.tv_sec - beginningOfTime.tv_sec) * 1000 + (nowTime.tv_nsec - beginningOfTime.tv_nsec) / 1000000;
    clock_gettime(CLOCK_MONOTONIC, &loopStartTime);

    // doesn't seem to be needed
    // refresh();

    // ms per update
    if (now - frameStart > ms_per_update) {
      // print debug info about frames per second
      attrset(COLOR_PAIR(0)); // DEFAULT
      if (DEBUG) mvprintw(DEBUG_LINE_START_Y + 0, 0, "nSeconds per getch():\t%" PRIu64 "       ", delta_us);
      if (DEBUG) mvprintw(DEBUG_LINE_START_Y + 1, 0, "Now in milliseconds:\t%" PRIu64 "      ", now);

      // update frameStartTime
      clock_gettime(CLOCK_MONOTONIC, &frameStartTime);
      frameStart = now;

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
      if (a_column[0]) {
        playNote(_synth, _channel, a_column[0], _velocity);
      }
      else {
        streak = 0;
      }
      break;

    case KEY_DOWN:
    case 's':
    case 'S':
      _note = 50;
      if (s_column[0]) {
        playNote(_synth, _channel, s_column[0], _velocity);
      }
      else {
        streak = 0;
      }
      break;

    case KEY_UP:
    case 'd':
    case 'D':
      _note = 51;
      if (d_column[0]) {
        playNote(_synth, _channel, d_column[0], _velocity);
      }
      else {
        streak = 0;
      }
      break;

    case KEY_RIGHT:
    case 'f':
    case 'F':
      _note = 55;
      if (f_column[0]) {
        playNote(_synth, _channel, f_column[0], _velocity);
      }
      else {
        streak = 0;
      }
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

  //update Scoreboard
  updateScoreboard();
}

void make_it_rain(void)
{
  for (int i = 0; i < BOARD_HEIGHT - 1; i++) {
    a_column[i] = a_column[i + 1];
    if (a_column[i] > 0) {
      mvaddch(FINISH_LINE - i - 1, NOTE_ONE_X, ERASE);
      attrset(COLOR_PAIR(2));
      mvaddch(FINISH_LINE - i, NOTE_ONE_X, ACS_DIAMOND);
    }

  }

  for (int i = 0; i < BOARD_HEIGHT - 1; i++) {
    s_column[i] = s_column[i + 1];
    if (s_column[i] > 0) {
      mvaddch(FINISH_LINE - i - 1, NOTE_TWO_X, ERASE);
      attrset(COLOR_PAIR(1));
      mvaddch(FINISH_LINE - i, NOTE_TWO_X, ACS_DIAMOND);
    }
  }

  for (int i = 0; i < BOARD_HEIGHT - 1; i++) {
    d_column[i] = d_column[i + 1];
    if (d_column[i] > 0) {
      mvaddch(FINISH_LINE - i - 1, NOTE_THREE_X, ERASE);
      attrset(COLOR_PAIR(3));
      mvaddch(FINISH_LINE - i, NOTE_THREE_X, ACS_DIAMOND);
    }
  }

  for (int i = 0; i < BOARD_HEIGHT - 1; i++) {
    f_column[i] = f_column[i + 1];
    if (f_column[i] > 0) {
      mvaddch(FINISH_LINE - i - 1, NOTE_FOUR_X, ERASE);
      attrset(COLOR_PAIR(4));
      mvaddch(FINISH_LINE - i, NOTE_FOUR_X, ACS_DIAMOND);
    }
  }

  a_column[BOARD_HEIGHT - 1] = 0;
  s_column[BOARD_HEIGHT - 1] = 0;
  d_column[BOARD_HEIGHT - 1] = 0;
  f_column[BOARD_HEIGHT - 1] = 0;

  int note = spawnNote();
  while (note) {
    if (note % 4 == 0) {
      if (a_column[BOARD_HEIGHT - 1] == 0) a_column[BOARD_HEIGHT - 1] = note;
      else if (s_column[BOARD_HEIGHT - 1] == 0) s_column[BOARD_HEIGHT - 1] = note;
      else if (d_column[BOARD_HEIGHT - 1] == 0) d_column[BOARD_HEIGHT - 1] = note;
    } else if (note % 4 == 1) {
      if (s_column[BOARD_HEIGHT - 1] == 0) s_column[BOARD_HEIGHT - 1] = note;
      else if (d_column[BOARD_HEIGHT - 1] == 0) d_column[BOARD_HEIGHT - 1] = note;
      else if (f_column[BOARD_HEIGHT - 1] == 0) f_column[BOARD_HEIGHT - 1] = note;
    } else if (note % 4 == 2) {
      if (d_column[BOARD_HEIGHT - 1] == 0) d_column[BOARD_HEIGHT - 1] = note;
      else if (f_column[BOARD_HEIGHT - 1] == 0) f_column[BOARD_HEIGHT - 1] = note;
      else a_column[BOARD_HEIGHT - 1] = note;
    } else if (note % 4 == 3) {
      if (f_column[BOARD_HEIGHT - 1] == 0) f_column[BOARD_HEIGHT - 1] = note;
      else if (a_column[BOARD_HEIGHT - 1] == 0) a_column[BOARD_HEIGHT - 1] = note;
      else if (s_column[BOARD_HEIGHT - 1] == 0) s_column[BOARD_HEIGHT - 1] = note;
    }
    note = spawnNote();
  }
}

void terminalHeroInit(void)
{
  // Prepare world
  draw_board();

  // set our clocks
  clock_gettime(CLOCK_MONOTONIC, &loopStartTime);
  clock_gettime(CLOCK_MONOTONIC, &loopEndTime);
  clock_gettime(CLOCK_MONOTONIC, &beginningOfTime);
  clock_gettime(CLOCK_MONOTONIC, &nowTime);
  clock_gettime(CLOCK_MONOTONIC, &frameStartTime);

  // Beat = Quarter Note, 16th notes per update.  Full board is one measure
  float divide_beat_per_update = 4.0f;
  // update our ms_per_update now that we have the new BPM
  ms_per_update = 1000.0f / ((BPM / 60.0f) * divide_beat_per_update);
}

void draw_board(void)
{
  attrset(COLOR_PAIR(7));
  mvaddch(BOARD_START_Y - 1, BOARD_START_X, ACS_ULCORNER);
  mvaddch(BOARD_START_Y - 1, BOARD_START_X + BOARD_WIDTH, ACS_URCORNER);
  mvhline(BOARD_START_Y - 1, BOARD_START_X + 1, ACS_HLINE, BOARD_WIDTH - 1);
  mvvline(BOARD_START_Y, BOARD_START_X, ACS_VLINE, BOARD_HEIGHT);
  mvvline(BOARD_START_Y, BOARD_START_X + BOARD_WIDTH, ACS_VLINE, BOARD_HEIGHT);
  mvhline(FINISH_LINE, BOARD_START_X + 1, ACS_HLINE, BOARD_WIDTH - 1);
  mvaddch(FINISH_LINE, BOARD_START_X, ACS_LLCORNER);
  mvaddch(FINISH_LINE, BOARD_START_X + BOARD_WIDTH, ACS_LRCORNER);

  mvaddch(BOARD_START_Y - 1, NOTE_ONE_X, ERASE);
  mvaddch(BOARD_START_Y - 1, NOTE_TWO_X, ERASE);
  mvaddch(BOARD_START_Y - 1, NOTE_THREE_X, ERASE);
  mvaddch(BOARD_START_Y - 1, NOTE_FOUR_X, ERASE);

  attrset(COLOR_PAIR(2)); // GREEN
  mvaddch(FINISH_LINE + 1, NOTE_ONE_X, 'A');
  mvaddch(FINISH_LINE, NOTE_ONE_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(1)); // RED
  mvaddch(FINISH_LINE + 1, NOTE_TWO_X, 'S');
  mvaddch(FINISH_LINE, NOTE_TWO_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(3)); // YELLOW
  mvaddch(FINISH_LINE + 1, NOTE_THREE_X, 'D');
  mvaddch(FINISH_LINE, NOTE_THREE_X, ACS_DIAMOND);

  attrset(COLOR_PAIR(4)); // BLUE
  mvaddch(FINISH_LINE + 1, NOTE_FOUR_X, 'F');
  mvaddch(FINISH_LINE, NOTE_FOUR_X, ACS_DIAMOND);


  move(FINISH_LINE + 2, 0);

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

  // increase scoreboard
  score += BASE_SCORE_INCREMENT;
  streak++;
}

int spawnNote(void) {
  int note = 0;

  int tracks = midifile.getTrackCount();
  for (int track = 0; track < tracks; track++) {
    for (int event = currEvent; event < midifile[track].size(); event++) {
      if (now + 0.05 >= midifile[track][event].seconds * 1000) {
        if (midifile[track][event].isNoteOn()) {
          attrset(COLOR_PAIR(0)); // DEFAULT
          if (DEBUG) mvprintw(DEBUG_LINE_START_Y + 4, 0, "is Note On:\t%d       ", midifile[track][event].isNoteOn() );
          // for (int i = 0; i < midifile[track][event].size(); i++)
          if (DEBUG) mvprintw(DEBUG_LINE_START_Y + 3, 0, "Midi Note On:\t%d       ", (int)midifile[track][event][midifile[track][event].size() - 2]);
          if (DEBUG) mvprintw(DEBUG_LINE_START_Y + 2, 0, "Midi Event at:\t%f   seconds", midifile[track][event].seconds);

          currEvent = event + 1;
          // if ( (int)midifile[track][event][midifile[track][event].size() - 2] > note) note = (int)midifile[track][event][midifile[track][event].size() - 2];
          return (int)midifile[track][event][midifile[track][event].size() - 2];
        }
      } else if (now + 0.1 >= midifile[track][event].seconds * 1000) {
        if (midifile[track][event].isNoteOn()) {
          if (DEBUG) mvprintw(DEBUG_LINE_START_Y + 5, 0, "DBG:\t%f       ", midifile[track][event].seconds );
        }
      }
      else {
        return 0;
      }
    }
  }

  return note;
}

void updateScoreboard(void) {
  attrset(COLOR_PAIR(7)); // DEFAULT
  mvprintw(SCOREBOARD, BOARD_START_X, "Score: %d", score );
  mvprintw(SCOREBOARD + 1, BOARD_START_X, "Streak: %d    ", streak );
}