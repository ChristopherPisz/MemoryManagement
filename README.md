# MemoryManagement
Custom Memory Management

After being asked on an interview whether or not I had done any custom memory management 
and was familiar with placement new, I decided to look into it.

I read through several articles and looked at a few libraries. The best article I found was:
https://www.gamedev.net/resources/_/technical/general-programming/c-custom-memory-allocation-r3010#commentsStart
upon which this code is based.

For now, I just wanted to get the concepts done and look at performance tests.
Next, I've got to figure out how to unit test this and make sure the memory allocation and deallocation does not become corrupt.
If all goes well, maybe I'll end up with a few template classes.

Dependencies:

) Common Library  
You can grab my common library from git hub https://github.com/ChristopherPisz/

Expects Headers at $(SolutionDir)..\Common  
Expects Library and Dll at $(SolutionDir)..\Common\$(Platform)\$(Configuration)

Copies dll using xcopy /y /d "$(SolutionDir)..\Common\$(Platform)\$(Configuration)\*.dll" "$(OutDir)"

) Boost Library  
You can grab boost from http://www.boost.org/  
This project doesn't use the entire boost library. It is for the most part just a dependency of the Common Library and only
a few of the boost libraries are used. Feel free to narrow it down if it concerns you. They are header only at this time, but
that might change as I further develop the common library.

Expects headers at $(SolutionDir)..\..\Third Party\boost_1_62_0  
Expects Libraries at $(SolutionDir)..\..\Third Party\boost_1_62_0\lib64-msvc-14.0

Copies dll using REM xcopy /y /d "$(SolutionDir)..\..\Third Party\boost_1_62_0\lib64-msvc-14.0\*.dll" "$(OutDir)"


