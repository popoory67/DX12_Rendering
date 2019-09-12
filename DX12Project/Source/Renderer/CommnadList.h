#pragma once

template<class TCommandList>
class CommandListBase
{

};

template<class TCommand>
struct Command
{
	void ExecuteAndDestruct(CommandListBase& CmdList)
	{
		TCommand* pCmd = static_cast<TCommand>(this);
		if (pCmd)
		{
			pCmd->Execute(CmdList);
			pCmd->~TCommand();
		}
	}
};