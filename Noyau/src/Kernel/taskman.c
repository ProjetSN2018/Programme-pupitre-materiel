/*
 * taskman.c
 *
 * Created: 31/01/2018 17:42:26
 *  Author: eleve
 */ 

 #include "../Headers/appli.h"

 //Déclaration de la liste des tâches à exécuter
 t_task tasks[TASKMAN_TASKS_LIST_LEN];

 //Déclaration de la liste des tâches différées à exécuter
 t_task dTasks[TASKMAN_DELAYED_TASKS_LIST_LEN];

 //Taskman Object Model
 struct {
	 t_task*pPush;
	 t_task*pPop;
 }taskman; //Unique taskman object.


 ///////// TIMER TICK INITIALIZER //////////////////////////////////////////
 static void TCWaveformInitialize(void);

 //Use TC Peripheral 0 ///////////////////////////////////////////////
 #define TC								TC0
 #define TC_PERIPHERAL					0

 //Configure TC0 channel 0 as waveform output ////////////////////////
 #define TC_CHANNEL_WAVEFORM			0
 #define ID_TC_WAVEFORM					ID_TC0
 #define PIN_TC_WAVEFORM				PIN_TC0_TIOA0
 #define PIN_TC_WAVEFORM_MUX			PIN_TC0_TIOA0_MUX

 #define TC_WAVEFORM_TIMER_SELECTION	TC_CMR_TCCLKS_TIMER_CLOCK4
 #define TC_WAVEFORM_DIVISOR			128
 #define TC_WAVEFORM_FREQUENCY			1000
 #define TC_WAVEFORM_DUTY_CYCLE			30

 #define TC_IRQn						TC0_IRQn
 #define TC_Handler						TC0_Handler
 ////////////////////////////////////////////////////////////////////////////


 /////// TASKMAN PRIVATE SERVICE CODES //////////////////////////////////////
 enum{
	_TASKMAN_DELAYED_TASK_PROC = 1
 };


 /////////////////////////////////////////////////////////////////////////////
 uint32_t Taskman(uint32_t sc, ...)
 {
//	uint8_t criticalStatus;
	
	switch(sc)
	{
	case TASKMAN_NEW:
#define k sc
		taskman.pPop=taskman.pPush=tasks;
		for(k=0;k<TASKMAN_DELAYED_TASKS_LIST_LEN;k++)
		{
			dTasks[k].delay=0;
		}
		//// TIMER TICK CONFIGURATION //////////////////////////////////////////////
		ioport_set_pin_mode(PIN_TC_WAVEFORM, PIN_TC_WAVEFORM_MUX);
		ioport_disable_pin(PIN_TC_WAVEFORM);

		TCWaveformInitialize();

		//Enable TC interrupt //////////////////////////////////////////////////////
		NVIC_DisableIRQ(TC_IRQn);
		NVIC_ClearPendingIRQ(TC_IRQn);
		NVIC_SetPriority(TC_IRQn, 0);
		NVIC_EnableIRQ(TC_IRQn);

		//Enable the waveform channel interrupt ///////////////////////////////////
		tc_enable_interrupt(TC, TC_CHANNEL_WAVEFORM, TC_IER_CPAS);

#undef k
		break;

	case TASKMAN_PUSH_TASK:
#define _func		((t_ptf)pa1)
#define _sc			((uint32_t)pa2)
#define _pParam		((void*)pa3)
#define _delay		((uint32_t)pa4)
#define  k			sc
		cpu_irq_enter_critical();
		if(_delay)
		{
			//Search for a free location in dTasks list
			for(k=0;(k<TASKMAN_DELAYED_TASKS_LIST_LEN)&&(dTasks[k].delay);k++);
			if(k==TASKMAN_DELAYED_TASKS_LIST_LEN) Error(ERR_TASKMAN_DELAYED_TASKLIST_FULL, (uint32_t)0);
			dTasks[k].pFunc=_func;
			dTasks[k].sc=_sc;
			dTasks[k].pParam=_pParam;
			dTasks[k].delay=_delay;
		}
		else
		{
			taskman.pPush->pFunc=_func;
			taskman.pPush->sc=_sc;
			taskman.pPush->pParam=_pParam;
			taskman.pPush++;
			if(taskman.pPush>=tasks+TASKMAN_TASKS_LIST_LEN) taskman.pPush=tasks;
			if(taskman.pPush==taskman.pPop) Error(ERR_TASKMAN_TASKLIST_FULL, (uint32_t)0);
		}
		cpu_irq_leave_critical();
#undef _func
#undef _sc
#undef _pParam
#undef _delay
#undef k
		break;

	case TASKMAN_POP_TASK:
		if(taskman.pPop!=taskman.pPush)
		{
			t_ptf	 func;
			void*	 pa;
			func = taskman.pPop->pFunc;
			sc = taskman.pPop->sc;
			pa = taskman.pPop->pParam;
			taskman.pPop++;
			if(taskman.pPop>=tasks+TASKMAN_TASKS_LIST_LEN) taskman.pPop=tasks;
			//Function call
			func(sc,pa);
		}
		break;

	//Private services implementation section /////////////////////////////////
	case _TASKMAN_DELAYED_TASK_PROC:
		gpio_toggle_pin(PIO_PA2_IDX);
#define k sc
		for(k=0;k<TASKMAN_DELAYED_TASKS_LIST_LEN;k++)
		{
			if(dTasks[k].delay)
			{
				if(--dTasks[k].delay==0)
				{
					PushTask(
						dTasks[k].pFunc,
						dTasks[k].sc,
						dTasks[k].pParam,
						0
					);
				}
			}
		}
#undef k		
		break;

	///////////////////////////////////////////////////////////////////////////
	default:
		Error(ERR_TASKMAN_SWITCH_BAD_SC, sc);
	}
	////////////////////////////////////////////////////////////////////////////
	return 0;
 }


 void TC0_Handler(void)
 {
	 tc_get_status(TC, TC_CHANNEL_WAVEFORM);
	 Taskman(_TASKMAN_DELAYED_TASK_PROC);
 }

 /////////////////////////////////////////// TIMER TICK INITIALIZER ////////////////////////////////////////////////////
 static void TCWaveformInitialize(void)
 {
	 uint32_t ra, rc;

	 //Configure the PMC to enable the TC module.
	 sysclk_enable_peripheral_clock(ID_TC_WAVEFORM);

	 //Init TC to waveform mode.
	 tc_init(
	 TC,
	 TC_CHANNEL_WAVEFORM,
	 TC_WAVEFORM_TIMER_SELECTION	|	//Waveform Clock Selection
	 TC_CMR_WAVE |					//Waveform mode is enabled
	 TC_CMR_ACPA_SET |				//RA Compare Effect: set
	 TC_CMR_ACPC_CLEAR |				//RC Compare Effect: clear
	 TC_CMR_CPCTRG					//UP mode with automatic trigger on RC Compare
	 );

	 //Configure waveform frequency and duty cycle.
	 rc =	(sysclk_get_peripheral_bus_hz(TC) /
	 TC_WAVEFORM_DIVISOR) /
	 TC_WAVEFORM_FREQUENCY;
	 tc_write_rc(TC, TC_CHANNEL_WAVEFORM, rc);

	 ra = (100 - TC_WAVEFORM_DUTY_CYCLE) * rc / 100;
	 tc_write_ra(TC, TC_CHANNEL_WAVEFORM, ra);

	 //Enable TC TC_CHANNEL_WAVEFORM
	 tc_start(TC, TC_CHANNEL_WAVEFORM);
 }
