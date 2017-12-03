#pragma once

#define BUFSIZE 4096 

class PowerShellProcess
{
public:
	PowerShellProcess();
	~PowerShellProcess();

private:

public:
	static wstring ExecCmd(wstring pCmdArg);
	static wstring ExecPowerShellScript(wstring filePath);

private:
	static wstring CleanResult(wstring subject);
};
