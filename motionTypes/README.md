# Motion-Types

When ActionScript 2 was introduced in Macromedia Flash, the "compiler" was doing a very poor job. It was very slow (type checking could take tens of seconds), buggy (some types errors where unoticed).

So based on the MotionScript parser I did before I wrote ASML, a AS2 parser and type checker, which was doing the job much better. It was just a type checker, so you would get errors very fast with ASML, then only when successful you would compile with the (slow) Flash AS2 Compiler. This was done by the end of 2003.

Inspired by OCaml, ASML was also doing type inference as decribed (in french) in asml/doc files.

At some point I wanted to introduce typed Arrays and some other syntax improvements over AS2, and generate AS1 code from it, in order to bypass the slow compiler.

So I wrote MotionTypes, a new in-house language and compiler in 2004, which was parsing .mt files, and doing type checking similar to ASML but with my advanced features.

Once type checking was complete, it was outputing a single big ActionScript 1 file (think about it like a single vanilla .js untyped file) that was compiled by Flash compiler - but this time faster because no type checking was done here. Since AS2 was only a wrapper over AS1 there was no performance impact in doing so.

I later added a direct SWF output, which contained the ActionScript 1 bytecode, so our Linux developers could compile their games by injecting the bytecode directly into as SWF library containing the assets packaged by artists without having to launch Flash.

Later, MotionTypes technology was used to develop open source MTASC compiler here (https://github.com/ncannasse/mtasc). This was done only for community as we were using Motion-Types in house, and never really used MTASC, but the idea was the same expect MTASC was only parsing pure AS2.

Finally, when working on Dinoparc server side, I wrote a Motion-Types MTS (MotionScript) output - similar to AS1 one - to do server-side development.

The two standard libraries, Flash client and MTS server were quite different and it was annoying to switch between the two targets because of these minor differences.

Since we wanted to unify client/server and also add JavaScript output - which was getting more and more possible thanks to the new popular browser Mozilla Firefox - I later redesigned the Motion-Types language and entirely rewrote the compiler and standard library, which became Haxe (http://haxe.org).


