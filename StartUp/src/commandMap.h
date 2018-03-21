/*
 * commandMap.h
 *
 * Created: 11/03/2018 13:19:33
 *  Author: Thierry
 */

/////// Commands code mapping //////////////////////////////////////////
const t_command_entry commands[]={
	//ToDo: Insert your command entries below
	// { 0x...., (t_pCmdFunc)funcPtr },
	//{0x6CAB, (t_pCmdFunc)set     },    //command set
	{0x624F, (t_pCmdFunc)start		},	//command start
	{0xAE76, (t_pCmdFunc)stop		},	//command stop

	////////////////// LIST TERMINATOR ////////////////////////////////
	{0x0000, NULL           } //Don't remove!
};
