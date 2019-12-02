# terminal hero
Terminal rhythm game

## Linux Quickstart Setup
Install fluidsynth
```
sudo apt-get install fluidsynth
sudo apt-get install libfluidsynth-dev
```

## Mac Quickstart Setup
Important Note:  fluidsynth is currently broken on macOS when installing from homebrew.  The new patch, soon to be released, will fix this.  Again fluidsynth 2.0.8 will not work and will cause this program to seg-fault.  I will keep an eye on the homebrew version and post back when it is updated.  Until that time terminal hero is much harder to set up and we will just have to wait :/  Thankfully the folks working on fluidsynth are fighting the good fight.

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

Run terminal-hero
```
./terminal-hero
```

Press q to [q]uit.

Enjoy the rain for now
