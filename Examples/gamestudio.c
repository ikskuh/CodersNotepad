#include <acknex.h>
#include <default.c>

action rotate()
{
	while(me)
	{
		my.pan += time_step;
		wait(1);
	}
}

function main()
{
	level_load(NULL);
	ent_create(CUBE_MDL, vector(32, 0, 0), rotate);
}