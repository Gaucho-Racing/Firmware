#include "main.h"
#include "Logomatic.h"
#include "CubeVCP.h"

void Loop(void)
{
	char msg[16] = "Hello from VCP\n";
	CubeVCP_SendString(msg, 16);
	LOGOMATIC_INFO("%.*s", 16, msg);
}
