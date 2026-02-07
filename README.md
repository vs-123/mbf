# MBF

`mbf` is a modified implementation of the esoteric programming language [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck), with the addition of macros.

`mbf` stands for "macro brainfuck".

## HELLO WORLD

`hello_world.mbf`:
```bf
   # hello macro
   hello {
      > 8+ [< 9+ > -] < .
      > 4+ [< 7+ > -] > +.
      7+ . .
      3+ .
      >> 6+ [< 7+ >-] < ++.
      12- .
      > 6+ [< 9+ >-] < + .
      <.
      3+ .
      6- .
      8- .
      >>> 4+ [< 8+ >-] <+ .
   }

   # call hello
   hello;
```

## USAGE
```bf   
   %  # use `mbf -h` for help
   
   %  mbf ./hello_world.mbf
   Hello, World!

   %  mbf -E expanded.bf hello_world.mbf
   [SUCCESS] file was successfully written!
   
   %  cat expanded.bf
   >++++++++[<+++++++++>-]<.>++++[<+++++++>-]<+.+++++++..+++.>>++++++[<+++++++>-]<++.------------.>++++++[<+++++++++>-]<+.<.+++.------.--------.>>>++++[<++++++++>-]<+.

   % # see `examples/` for more examples
```

## FEATURES

- Strict superset of BF
- 100% compatible with ordinary BF
- Includes an interpreter to execute both MBF & BF code
- Error messages
- Supports comments
- Macros are able to call other macros
- Infinite cycle detection (see `examples/cycle.mbf`)
- Lets you save expanded BF code with `-E` flag
- Helpful `-h` / `--help` flag for help

## HOW TO USE

- Prerequisites -- GNU Make & a C99 Compiler
   
- Clone this repository into `mbf/` and `cd` into it:
```
   %  git clone --recursive https://github.com/vs-123/mbf.git
   %  ls
   ./ ../ mbf/
   %  cd mbf/
```
   
- Manually set `CC` variable in case `make` fails to find your C compiler.
   
- Use `make` to build the project:
```
   %  pwd
   ~/mbf
   
   %  make build
   [INFO] building...
   [SUCCESS] successfully built!
   
   %  ls build/ && cd $_
   ./ ../ mbf*
   
   %  ./mbf ../examples/hello_world.mbf
   Hello, World!
```
   
- To run tests, use `make test`:
```
   %  make test
   [INFO] building tests...
   [INFO] running tests...
```
  
- To perform a clean operation, use =make clean=:
```
   %  make clean
   [INFO] cleaning...
   [SUCCESS] cleaned successfully!
```
   
## EXAMPLES

   You may find more examples in `examples/` directory.

## LICENSE

   This program is licensed under the GNU Affero General Public License version 3.0 or later.
   **NO WARRANTY PROVIDED**
   For full terms, see `LICENSE` file or visit **https://www.gnu.org/licenses/agpl-3.0.html**.
