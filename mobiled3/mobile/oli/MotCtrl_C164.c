//-----------------------------------------------
//local functions
//-----------------------------------------------
void execEvent(struct Event event)
{
	int i, iPara, iCmd;
	int iSynchVal, iSynchValMeasNorm, iDeltaSynchMeas;
	
	static enum STATE_Connection s_StConnection = ST_ConnReset;
	static enum STATE_Motor s_StMotor = ST_BrakeClosed;

	iCmd = event.ucData[0];

	switch(s_StConnection)
	{
	case ST_ConnReset:
		//-------------------
		switch(iCmd)
		{
		//-------------------
		case CMD_MOTCTRL_CONNECT:
			transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
			s_StConnection = ST_ConnConnected;
			break;
		//-------------------
		case CMD_MOTCTRL_DISCONNECT:
			_trap_(0x00);
			break;
		//-------------------
		default:
			if(event.iSource != EVSOURCE_INTERN)
				transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			break;
		}

		break;	//case ST_ConnReset

	case ST_ConnConnected:
		
		//---------------------------------------
		//commands always executed
		switch(iCmd)
		{
		//-------------------
		case CMD_MOTCTRL_DISCONNECT:
			_trap_(0x00);
			return;
		//-------------------
		//io
		case CMD_MOTCTRL_SETDIGOUT:
			setOutputPort8(event.ucData[1], event.ucData[2]);
			return;
		//-------------------
		case CMD_MOTCTRL_GETDIGIN:
			transmitMsg(event.iSource, P1H, P1L, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
			return;
		//-------------------
		case CMD_MOTCTRL_GETANALOGIN:
			iPara = g_iAnIn[event.ucData[1]];
			transmitMsg(event.iSource, iPara >> 8, iPara, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
			return;
		//-------------------
		//debug
		case CMD_MOTCTRL_SETCTRLPARA:
			g_iIn1 = (event.ucData[1] << 8) | event.ucData[2];
			g_iIn2 = (event.ucData[3] << 8) | event.ucData[4];
			g_iIn3 = (event.ucData[5] << 8) | event.ucData[6];

			//synch pos fine tuning
			//gc_iFieldPosSynchNorm = g_iIn1;

			return;
		//-------------------
		case CMD_MOTCTRL_GETCTRLPARA:
			transmitMsg(event.iSource, g_iOut1 >> 8, g_iOut1, g_iOut2 >> 8, g_iOut2, g_iOut3 >> 8, g_iOut3, 0, (iCmd << 2) | Msg_OK);
			return;
		//-------------------
		//get data
		case CMD_MOTCTRL_GETPOSVEL:
			transmitMsg(event.iSource, g_lPosMeas >> 24, g_lPosMeas >> 16, g_lPosMeas >> 8, g_lPosMeas, g_iVelMeas >> 8, g_iVelMeas, 0, (iCmd << 2) | Msg_OK);
			return;
		//-------------------
		case CMD_MOTCTRL_GETSTATUS:
			transmitMsg(event.iSource, g_iStateCtrl >> 8, g_iStateCtrl, g_iTempMeas >> 8, g_iTempMeas, 0, 0, 0, (iCmd << 2) | Msg_OK);
			return;
		}
		
		//---------------------------------------
		//motor control commands state machine
		switch(s_StMotor)
		{
		//-------------------
		case ST_BrakeClosed:
			switch(iCmd)
			{
			case CMD_MOTCTRL_DISABLEBRAKE:
				if(event.ucData[1] == 1)
				{
					openBrake(gc_iBrakeHasRelais);
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
					s_StMotor = ST_BrakeOpen;
				}
				break;	//CMD_MOTCTRL_DISABLEBRAKE

			default:
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			}	//switch(iCmd)

			break;	//ST_BrakeClosed

		//-------------------
		case ST_BrakeOpen:
			switch(iCmd)
			{
			case CMD_MOTCTRL_DISABLEBRAKE:
				if(event.ucData[1] == 0)
				{
					closeBrake(gc_iBrakeHasRelais);
					s_StMotor = ST_BrakeClosed;
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				}
				break;	//CMD_MOTCTRL_DISABLEBRAKE

			case CMD_MOTCTRL_ENABLEMOTOR: 
				if(event.ucData[1] == 1)
				{
					enableMotor(gc_iSineZero);
	
					g_iPosVelCtrlRun = 0;
					g_iTorqueCmd = 0;
					g_iTorqueCtrlRun = 1;
					g_iFieldPos = 0;
					g_iCommRun = 0;
					g_iSynchRun = 1;
	
					//increase torque slowly
					do
					{
						g_iTorqueCmd++;
						Delay(5000);
					}
					while(g_iTorqueCmd < gc_iSynchTorque);
	
					s_StMotor = ST_MotorEnabled;
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				}
				break;	//CMD_MOTCTRL_ENABLEMOTOR

			default:
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			}	//switch(iCmd)

			break;	//ST_BrakeOpen

		//-------------------
		case ST_MotorEnabled:
			switch(iCmd)
			{
			case CMD_MOTCTRL_ENABLEMOTOR:
				if(event.ucData[1] == 0)
				{
					disableMotorCtrl();
					s_StMotor = ST_BrakeOpen;
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				}
				break;	//CMD_MOTCTRL_ENABLEMOTOR
	
			case CMD_MOTCTRL_SYNCHMOTOR:
				//exec single rot to snap in rotor
				for(i=0; i<gc_iEncPulsePerRev; i++)
				{
					g_iFieldPos++;
					Delay(gc_iSynchVelDelay);
				}

				g_iFieldPos = 0;
				g_lPosOffset = g_lPosOffset + T2;	//g_lPosOffset used to move back to 0 after synch
				T2 = 0;

				//enable synch int
				g_iSynchIntCaptured = 0;
				EXICON = 0x0008 | 0x0001;			//EXIN0 interrupt on pos edge, EXIN1 interrupt on neg edge

				//wait for synch interrupt, communication blocking
				while(g_iSynchIntCaptured == 0)
				{
					_atomic_(3);
					//if no synch interrupt captured, update iSynchVal
					if(g_iSynchIntCaptured == 0)
						iSynchVal = T2;

					g_iFieldPos++;
					Delay(gc_iSynchVelDelay);
				}

				g_lPosOffset = g_lPosOffset + iSynchVal;

				//shift back iSynchVal
				while(iSynchVal > gc_iEncPulsePerPhase)
					iSynchVal = iSynchVal - gc_iEncPulsePerPhase;

				//reset T2, g_iT2RevCnt -> EncPosAbs = 0
				T2 = 0;
				g_iT2RevCnt = 0;
				if(gc_iMotorType == MotorType_6Pol)
					iSynchValMeasNorm = toFieldPosNorm_Mot6Pol_Enc4096(iSynchVal);
				if(gc_iMotorType == MotorType_4Pol)
					iSynchValMeasNorm = toFieldPosNorm_Mot4Pol_Enc4096(iSynchVal);

				iDeltaSynchMeas = iSynchValMeasNorm - gc_iFieldPosSynchNorm;
				if(iDeltaSynchMeas < -255)
					iDeltaSynchMeas = iDeltaSynchMeas + 512;
				if(iDeltaSynchMeas > 255)
					iDeltaSynchMeas = iDeltaSynchMeas - 512;
				
				//test for correct SynchVal
				if((iDeltaSynchMeas > -30) && (iDeltaSynchMeas < 90))
				{
					transmitMsg(event.iSource, iSynchValMeasNorm >> 8, iSynchValMeasNorm, gc_iFieldPosSynchNorm >> 8, gc_iFieldPosSynchNorm, 0, 0, 0, (iCmd << 2) | Msg_OK);
					s_StMotor = ST_MotorSynch;
				}
				else
				{
					disableMotorCtrl();
					transmitMsg(event.iSource, iSynchValMeasNorm >> 8, iSynchValMeasNorm, gc_iFieldPosSynchNorm >> 8, gc_iFieldPosSynchNorm, 0, 0, 0, (iCmd << 2) | Msg_Error);
					s_StMotor = ST_BrakeOpen;
				}
				break;	//CMD_MOTCTRL_SYNCHMOTOR

			default:
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			}	//switch(iCmd)

			break;	//ST_MotorEnabled

		//-------------------
		case ST_MotorSynch:
			switch(iCmd)
			{
			case CMD_MOTCTRL_ENABLECOMM:
				g_iTorqueCmd = 0;
				while(g_lTorqueCmd != 0)
					;
				g_iT2Overflow = 0;
				g_lPosCmd = getEncPosAbsAsLong();
				g_iSynchRun = 0;
				g_iCommRun = 1;
				g_iPosVelCtrlRun = 1;
				s_StMotor = ST_MotorRunning;

				setVoltDownBrake(gc_iBrakeHasRelais);

				transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				break;	//CMD_MOTCTRL_ENABLECOMM

			default:
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			}	//switch

			break;	//ST_MotorSynch

		//-------------------
		case ST_MotorRunning:
			switch(iCmd)
			{
			case CMD_MOTCTRL_SETCMDVAL:
				g_lPosCmd = (((long int)event.ucData[1]) << 24 | ((long int)event.ucData[2]) << 16 | ((long int)event.ucData[3]) << 8 | (long int)event.ucData[4]);
				g_iVelCmd = event.ucData[5] << 8 | event.ucData[6];

				transmitMsg(event.iSource, g_iStateCtrl >> 8, g_iStateCtrl, g_iTempMeas >> 8, g_iTempMeas, 0, 0, 0, (CMD_MOTCTRL_GETSTATUS << 2) | Msg_OK);
				transmitMsg(event.iSource, g_lPosMeas >> 24, g_lPosMeas >> 16, g_lPosMeas >> 8, g_lPosMeas, g_iVelMeas >> 8, g_iVelMeas, 0, (CMD_MOTCTRL_GETPOSVEL << 2) | Msg_OK);

				break;	//CMD_MOTCTRL_SETCMDVAL
			
			case CMD_MOTCTRL_SETMOTIONTYPE:
				g_iStMotionType = event.ucData[1];
				transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				break;	//CMD_MOTCTRL_SETMOTIONTYPE
	
			case CMD_MOTCTRL_SETEMSTOP:
				g_iPosVelCtrlRun = 0;
				g_iVelCmd = 0;
				g_iTorqueCmd = 0;
				//decrease torque slowly
				/*
				while(g_iTorqueCmd > 0)
				{
					g_iTorqueCmd--;
					Delay(3000);
				}
				*/
				closeBrake(gc_iBrakeHasRelais);
				s_StMotor = ST_MotorEMStop;
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				break;	//CMD_MOTCTRL_SETEMSTOP

			case CMD_MOTCTRL_ENABLEMOTOR:
				if(event.ucData[1] == 0)
				{
					disableMotorCtrl();
					s_StMotor = ST_BrakeOpen;
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				}
				break;	//CMD_MOTCTRL_ENABLEMOTOR

			default:
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			}	//switch(iCmd)

			break;	//ST_MotorRunning

		//-------------------
		case ST_MotorEMStop:
			switch(iCmd)
			{
			case CMD_MOTCTRL_SETCMDVAL:
				transmitMsg(event.iSource, g_iStateCtrl >> 8, g_iStateCtrl, g_iTempMeas >> 8, g_iTempMeas, 0, 0, 0, (CMD_MOTCTRL_GETSTATUS << 2) | Msg_OK);
				transmitMsg(event.iSource, g_lPosMeas >> 24, g_lPosMeas >> 16, g_lPosMeas >> 8, g_lPosMeas, g_iVelMeas >> 8, g_iVelMeas, 0, (CMD_MOTCTRL_GETPOSVEL << 2) | Msg_OK);
				break;	//CMD_MOTCTRL_SETCMDVAL

			case CMD_MOTCTRL_RESETEMSTOP:
				openBrake(gc_iBrakeHasRelais);
				g_iPosVelCtrlRun = 1;
				s_StMotor = ST_MotorRunning;
				transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				break;
			
			case CMD_MOTCTRL_ENABLEMOTOR:
				if(event.ucData[1] == 0)
				{
					disableMotorCtrl();
					s_StMotor = ST_BrakeOpen;
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_OK);
				}
				break;	//CMD_MOTCTRL_ENABLEMOTOR

			default:
				if(event.iSource != EVSOURCE_INTERN)
					transmitMsg(event.iSource, 0, 0, 0, 0, 0, 0, 0, (iCmd << 2) | Msg_NotAccept);
			}	//switch(iCmd)

			break;	//ST_MotorEMStop

		}	//switch(StMotor)

	}	//switch(s_iStConnection)
}
//-----------------------------------------------
