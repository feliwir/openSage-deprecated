#include "Audio.hpp"
#include <mpg123.h>
#include <iostream>


void Audio::Initialize()
{
	int  err = MPG123_OK;
	if ((err = mpg123_init()) != MPG123_OK)
	{
		std::cerr << mpg123_plain_strerror(err) << std::endl;
	}
}

void Audio::Release()
{
	mpg123_exit();
}