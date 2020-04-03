# Disciple

Around summer 2002 we were working on a PC card game in 3D in at Motion-Twin - codename Disciple.

After getting bored of C++ I was quite into OCaml so I wrote the following:
- Win32 OCaml wrappers of a big part of the Win32 API
- Osiris, a high level UI Components API built on top of it
- Dragoon3, some kind of Games IDE (think Unity/UnrealEngine) in pure OCaml to help us view the 3D models, edit materials, but also scripting etc.
- Disciple code was itself written in OCaml

The game was stopped by the end of the summer - if I remember correctly.

In retrospect, I learnt that while OCaml expressivity was indeed helping a lot compared to C++, its strictness both in terms of typing and compilation modules dependencies was making it very hard to build an actual game with it, spending too much time on architecture and not enough on getting things done.

I think this opened a bit the path to writing my own languages.


## Note for IDEs

Dragoon2 is an old previous game IDE I was working on, but written in C++. I remember I was very into writing gamedev tools that could be extended with plugins, and I realize this haven't stopped since I'm still doing the same with my latest middleware HIDE https://github.com/heapsio/hide

Also, as a tip if you want to write games: don't write IDEs or game engines, just focus on the game :)



