# terminal hero

Terminal rhythm game

## Linux Quickstart Setup

Install fluidsynth

```
sudo apt-get install fluidsynth
sudo apt-get install libfluidsynth-dev
```

## Mac Quickstart Setup

Install Homebrew

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Install fluidsynth via Homebrew

```
brew install fluidsynth pkg-config
```

## Running the program

Compile the code

```
source compile.sh
```

Run terminal-hero (Play twinkle twinkle by default)

```
./terminal-hero
```

Play Happy Birthday

```
./terminal-hero resources/midi-files/happy_birthday.mid
```

Load and automatically play your own midi file (experimental)

```
./terminal-hero /absolute/or/relative/path/to/my/midifile.mid
```

## Becoming the Terminal Hero

Play notes with `A`, `S`, `D`, and `F` Keys as the notes reach the bottom of the board.

Press `Q` to [Q]uit.
