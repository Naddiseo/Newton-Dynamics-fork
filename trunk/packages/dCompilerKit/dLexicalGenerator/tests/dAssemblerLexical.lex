
/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/


// lexical analizero fo and assembly compiler

AnyButAstr		[^\*]
AnyButSlash		[^\/]
Comment1        [\/][\*]({AnyButAstr}|[\*]{AnyButSlash})*[\*][\/]
Comment2        [\/][\/].*
Comment			({Comment1}|{Comment2})


%%
{Comment}			{}
[aA](d|D)[dD]		{ return ADD;}


