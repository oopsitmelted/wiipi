#include <pololu/orangutan.h>
#include <stdint.h>

#define CMD_M1_FWD 0x80
#define CMD_M1_REV 0x81
#define CMD_M2_FWD 0x82
#define CMD_M2_REV 0x83

uint8_t rx_buf[32];
uint8_t rx_idx;

enum
{
	STATE_WAIT_CMD,
	STATE_WAIT_M1_FWD_SPD,
	STATE_WAIT_M2_FWD_SPD,
	STATE_WAIT_M1_REV_SPD,
	STATE_WAIT_M2_REV_SPD
} e_State = STATE_WAIT_CMD;

uint8_t read_next_byte(void)
{
	uint8_t bytes_received;
	uint8_t byte;

	// Block until bytes are received
	while( ( bytes_received = serial_get_received_bytes() ) == rx_idx );

	byte = rx_buf[rx_idx];
	if( ++rx_idx == sizeof( rx_buf ) )
		rx_idx = 0;

	return byte;
}

int main()
{
	// Write a message to the LCD
	clear();
	//print("WiiPi");

	// Open serial port
	rx_idx = 0;
	serial_set_baud_rate(115200);
	serial_receive_ring( (char *)rx_buf, sizeof( rx_buf ) );

	while(1)
	{
		uint8_t rx_byte;

		// Process incoming commands
		rx_byte = read_next_byte();

		if(rx_byte > 127)
			e_State = STATE_WAIT_CMD;

		switch(e_State)
		{
			case STATE_WAIT_CMD:
			{
				switch(rx_byte)
				{
					case CMD_M1_FWD:
					{
						e_State = STATE_WAIT_M1_FWD_SPD;
						break;
					}

					case CMD_M1_REV:
					{
						e_State = STATE_WAIT_M1_REV_SPD;
						break;
					}

					case CMD_M2_FWD:
					{
						e_State = STATE_WAIT_M2_FWD_SPD;
						break;
					}

					case CMD_M2_REV:
					{
						e_State = STATE_WAIT_M2_REV_SPD;
						break;
					}
					
					default:
						break;
				}
				break;
			}

			case STATE_WAIT_M1_FWD_SPD:
			{
				set_m1_speed(rx_byte * 2);
				e_State = STATE_WAIT_CMD;
				break;
			}

			case STATE_WAIT_M1_REV_SPD:
			{
				set_m1_speed(-(rx_byte * 2));
				e_State = STATE_WAIT_CMD;
				break;
			}

			case STATE_WAIT_M2_FWD_SPD:
			{
				set_m2_speed(rx_byte * 2);
				e_State = STATE_WAIT_CMD;
				break;
			}

			case STATE_WAIT_M2_REV_SPD:
			{
				set_m2_speed(-(rx_byte * 2));
				e_State = STATE_WAIT_CMD;
				break;
			}

			default:
				break;
		}
	}

	return 0;
}
