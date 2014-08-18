#include "stm32f429i_discovery.h"
#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//Player1
int16_t player1X =10;
int16_t player1Y = 10;
uint16_t player1W = 60;
uint16_t player1H = 10;
uint8_t player1IsReversed = 0;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH - 20;
int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
uint16_t player2W = 60;
uint16_t player2H = 10;
uint8_t player2IsReversed = 1;

//Ball
uint16_t ballSize = 5;
int16_t ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
int16_t ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;
int16_t ballVX = 5;
int16_t ballVY = 5;
uint8_t ballIsRun = 0;

//Mode
uint8_t demoMode = 1;

void
BallReset()
{

	ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
	ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;
	int con =(rand() % 4) +1;
	switch(con){
		case 1:
			ballVX = 5;
			ballVY = 5;
			break;
		case 2:
			ballVX = -5;
			ballVY = 5;
			break;
		case 3:
			ballVX = 5;
			ballVY = -5;
			break;
		case 4:
			ballVX = -5;
			ballVY = -5;
			break;
	}

	ballIsRun = 1;
}

void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		player1IsReversed = 0;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		player1IsReversed = 1;
	}
}
//GAME_EventHandler2()
//{
//	if( IOE_TP_GetState()->TouchDetected ){
//
//		player2IsReversed = 1;
//
//		while( IOE_TP_GetState()->TouchDetected );
//		//TP_State.TouchDetected
//		player2IsReversed = 0;
//	}
//}

 int32_t x, xr;
void
GAME_EventHandler2()
{
		TP_STATE *s =IOE_TP_GetState();

		if( s->TouchDetected){
			x = s->X;
			xr=x;
//			if (abs(x-temp)>=100)
//				xr = temp;
//			else
//				xr=x;

			/* x value first correction */
//					if(x <= 3000){
//						x = 3870 - x;
//					}
//					else{
//						x = 3800 - x;
//					}
//					/* x value second correction */
//					xr = x / 15;
//					/* return x position value */
//					if(xr <= 0){
//						xr = 0;
//					}
//					else if (xr > 240){
//						xr = 239;
//					}
//					else{}
		//player2IsReversed = 0;
		/* Read x value from DATA_X register */
//		x = I2C_ReadDataBuffer(IOE_REG_TP_DATA_X);

	}

}

void
GAME_EventHandler3()
{
	if( ballIsRun == 0 ){
		BallReset();
	}
}

void
GAME_Update()
{
	//Player1
		LCD_SetTextColor( LCD_COLOR_BLACK );
		LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
		LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
		if( demoMode == 0 ){
			if( player1IsReversed )
				player1X -= 5;
			else
				player1X += 5;
			if( player1X <= 0 )
				player1X = 0;
			else if( player1X + player1W >= LCD_PIXEL_WIDTH )
				player1X = LCD_PIXEL_WIDTH - player1W;

		//Player2
			if( player2IsReversed )
				player2X -= 5;
			else
				player2X += 5;
			if( player2X <= 0 )
				player2X = 0;
			else if( player2X + player2W >= LCD_PIXEL_WIDTH )
				player2X = LCD_PIXEL_WIDTH - player2W;

		//Ball
		if( ballIsRun == 1 ){

			LCD_SetTextColor( LCD_COLOR_BLUE );
			LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );

			//Touch wall
			ballX += ballVX;
			if( ballX <= 0 ){
				ballX = 0;
				ballVX *= -1;
			}
			else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
				ballX = LCD_PIXEL_WIDTH - ballSize;
				ballVX *= -1;
			}

			//PONG!
			ballY += ballVY;
			if( ballY + ballSize >= player2Y ){
				if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX - ballSize <= player2Y + player2W/4 ){
						ballVY =-3;
						ballVX =-7;
					}
					else if( ballX >= player2Y + player2W - player2W/4 ){
						ballVY =-3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player2Y + player2W/2 ){
						ballVY =-7;
						ballVX =-3;
					}
					else if( ballX > player2Y + player2W/2 ){
						ballVY =-7;
						ballVX = 3;
					}
					else{
						ballVY =-9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){
					if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
						if( ballX - ballSize <= player1Y + player1W/4 ){
							ballVY = 3;
							ballVX =-7;
						}
						else if( ballX >= player1Y + player1W - player1W/4 ){
							ballVY = 3;
							ballVX = 7;
						}
						else if( ballX + ballSize < player1Y + player1W/2 ){
							ballVY = 7;
							ballVX =-3;
						}
						else if( ballX > player1Y + player1W/2 ){
							ballVY = 7;
							ballVX = 3;
						}
						else{
							ballVY = 9;
							ballVX = 0;
						}
					}
					else
						BallReset();
				}
			}
		}
	//----------------------------------------------------------------------------------------------------------------------------------
		else{			//if demoMode == 1

			//Player1 move
			if( ballVY < 0 ){
				if( player1X + player1W/2 < ballX + ballSize/2 ){
					player1X += 6;
					//player2X += 2;
					 if( player1X + player1W >= LCD_PIXEL_WIDTH )
						 player1X = LCD_PIXEL_WIDTH - player1W;
				}
				else{
					player1X -= 6;
					//player2X -= 2;
					if( player1X <= 0 )
						player1X = 0;
				}
			}

			//Player2 move
//			if( ballVY > 0 ){
//				if( player2X + player2W/2 < ballX + ballSize/2 ){
//					player1X += 2;
//					player2X += 8;
//				}
//				else{
//					player1X -= 2;
//					player2X -= 8;
//				}
//
//			}
//
//			if( player1X <= 0 )
//				player1X = 0;
//			else if( player1X + player1W >= LCD_PIXEL_WIDTH )
//				player1X = LCD_PIXEL_WIDTH - player1W;
//
//			if( player2X <= 0 )
//				player2X = 0;
//			else if( player2X + player2W >= LCD_PIXEL_WIDTH )
//				player2X = LCD_PIXEL_WIDTH - player2W;
//
			//Player2 move
				player2X  = xr;

				if( player2X <= 0 )
					player2X = 0;
				else if( player2X + player2W >= LCD_PIXEL_WIDTH )
					player2X = LCD_PIXEL_WIDTH - player2W;


			//Ball
			if( ballIsRun == 1 ){

				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );

				//Touch wall
				ballX += ballVX;
				if( ballX <= 0 ){
					ballX = 0;
					ballVX *= -1;
				}
				else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
					ballX = LCD_PIXEL_WIDTH - ballSize;
					ballVX *= -1;
				}

				//PONG!
				ballY += ballVY;
				if( ballY + ballSize >= player2Y ){
					if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX - ballSize <= player2Y + player2W/4 ){
						ballVY =-3;
						ballVX =-7;
					}
					else if( ballX >= player2Y + player2W - player2W/4 ){
						ballVY =-3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player2Y + player2W/2 ){
						ballVY =-7;
						ballVX =-3;
					}
					else if( ballX > player2Y + player2W/2 ){
						ballVY =-7;
						ballVX = 3;
					}
					else{
						ballVY =-9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){
				if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
					if( ballX - ballSize <= player1Y + player1W/4 ){
						ballVY = 3;
						ballVX =-7;
					}
					else if( ballX >= player1Y + player1W - player1W/4 ){
						ballVY = 3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player1Y + player1W/2 ){
						ballVY = 7;
						ballVX =-3;
					}
					else if( ballX > player1Y + player1W/2 ){
						ballVY = 7;
						ballVX = 3;
					}
					else{
						ballVY = 9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}
		}
	}
}

	void
GAME_Render()
{
	LCD_SetTextColor( LCD_COLOR_WHITE);
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );
	LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}
