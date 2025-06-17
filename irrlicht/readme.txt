==========================================================================
The Irrlicht Engine SDK version 0.6
==========================================================================

  Welcome the Irrlicht Engine SDK.

  Content of this file:

  1. Directory structure overview
  2. How to start
  3. Release Notes
  4. License
  5. Contact



==========================================================================
1. Directory structure overview
==========================================================================

  You will find some directories after decompressing the archive in which
  came the SDK. These are:
  
  \bin       The compiled library Irrlicht.DLL and some compiled demo 
             and example applications, just start them to see the 
             Irrlicht Engine in action.
  \doc       Documentation of the Irrlicht Engine.
  \examples  Examples and tutorials showing how to use the engine. 
  \include   Header files to include when using the engine.
  \lib       Lib to link with your programs when using the engine.
  \media     Graphics and sound data for the demo applications and
             examples.
  \source    The source code of the Irrlicht Engine. This code is
             not needed to develop applications with the engine,
             but it is included to let you be able to recompile and
             debug it, if necessary.     
  \tools     Useful tools and their sourcecode for the engine.



==========================================================================
2. How to start
==========================================================================

  To see the engine in action in Windows, just go to the \bin\VisualStudio
  directory, and start some applications. There should also be an
  application named TechDemo.exe, this should show the most
  interesting things.

  The Irrlicht Engine is a static lib under linux. A precomipled version can be
  found in \lib\Linux. 
  If you wish to compile the engine in linux yourself, unzip the source .zip
  file in the \source directory and run 'make'. After this, you should be able 
  to make all example applications in \examples. Then just start an X Server
  and run them, from the directory where they are.
  
  To start developing own applications and games with the engine take 
  a look at the 1.HelloWorld example in the \examples directory.
  There will also be a .html file with a tutorial easy to understand.


==========================================================================
3. Release Notes
==========================================================================

  This is an alpha version of the SDK. Therefore please note that there are
  features missing in the engine and maybe also some bugs. 

  Informations about changes in this new version of the engine can be 
  found in changes.txt.

==========================================================================
4. License
==========================================================================

  The license of the Irrlicht Engine is based on the zlib/libpng license.
  Even though this license does not require you to mention that you are
  using the Irrlicht Engine in your product, an acknowledgement
  would be highly appreciated.

  Please note that the Irrlicht Engine is based in part on the work of 
  the Independent JPEG Group and the zlib. This means that if you use
  the Irrlicht Engine in your product, you must acknowledge somewhere 
  in your documentation that you've used the IJG code. It would also be
  nice to mention that you use the Irrlicht Engine and the zlib.
  See the README files in the jpeglib and the zlib for
  further informations.


  The Irrlicht Engine License
  ===========================

  Copyright (C) 2002-2004 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  

  
==========================================================================
5. Contact
==========================================================================

  If you have problems, questions or suggestions, please visit the 
  official homepage of the Irrlicht Engine:
  
  http://irrlicht.sourceforge.net
  
  You will find forums, bugtrackers, patches, tutorials, and other stuff
  which will help you out.
  
  If want to contact the author of the engine, please send an email to
  Nikolaus Gebhardt:
  
  irrlicht@users.sourceforge.net


  
    