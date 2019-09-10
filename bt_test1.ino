#include <Arduino.h>
#include <SoftwareSerial.h>


SoftwareSerial bt(10,11);


class BTCommand
{
public:
	bool decode( char* command );
	uint8_t get_device(){ return this->device; }
	uint8_t get_actuator(){ return this->actuator; }
	uint8_t get_command(){ return this->command; }

private:
	uint8_t device;
	uint8_t actuator;
	uint8_t command;

	char device_as_array[2];
	char actuator_as_array[2];
	char command_as_array[2];

	uint8_t ascii2bin( char* data );
};

uint8_t BTCommand::ascii2bin( char* data )
{
	uint8_t val;
	uint8_t offset;

	val = data[0];

	if( val >= 'a' ) offset = 87;
	else if( val >= 'A' ) offset = 55;
	else offset = 48;

	uint8_t msb = ( data[0] - offset ) << 4;

	val = data[1];

	if( val >= 'a' ) offset = 87;
	else if( val >= 'A' ) offset = 55;
	else offset = 48;

	uint8_t lsb = data[1] - offset;

	return msb | lsb;
}

/**
 * @brief Decodes the command received through bluetooth
 *
 * @param command The command. Must obbey the correct format.
 *
 * @return true if the command obbeys the correct format; false otherwise.
 *
 * @post The getter methods make no sense have sense whenever this function returns
 * false.
 */
bool BTCommand::decode( char* command )
{
	bool valid{ false };

	char* car = command;

	size_t count = 0;

	for( count = 0; *car == '#'; ++count, ++car ) { ; }
	// contamos el número de #

	if( count == 2 ){

		for( count = 0; *car != '/' ; ++count, ++car ){
			device_as_array[ count ] =  *car;
		}

		++car;
		// nos comemos el '/'

		if( count == 2 ){

			for( count = 0; *car != '/' ; ++count, ++car ){
				actuator_as_array[ count ] =  *car;
			}

			++car;

			if( count == 2 ){

				for( count = 0; *car != '\0' ; ++count, ++car ){
					command_as_array[ count ] =  *car;
				}

				if( count == 2 ){
					this->device = ascii2bin( this->device_as_array );
					this->actuator = ascii2bin( this->actuator_as_array );
					this->command = ascii2bin( this->command_as_array );

					valid = true;
				}
			}
		}
	} 

	return valid;
}




void setup()
{
	Serial.begin( 115200 );
	Serial.println( "Ready..." );

	pinMode( 10, INPUT );
	pinMode( 11, OUTPUT );
	bt.begin( 9600 );

	pinMode( 13, OUTPUT );

	digitalWrite( 13, HIGH );
	delay( 500 );
	digitalWrite( 13, LOW );
}

void loop()
{
	BTCommand bt_cmd;

	bool state{ false };

	char command[16];

	while( 1 )
	{
		if ( bt.available() == 10 ){
			for( size_t i = 0; i < 10; ++i ){
				command[i] = bt.read();
			}
			command[10] = '\0';
			Serial.println( "Command: " + String( command ) );

			if( bt_cmd.decode( command ) == true ){
				if( bt_cmd.get_command() == 0 ){
					digitalWrite( 13, HIGH );
				} else {
					digitalWrite( 13, LOW );
				}
			} else{ 
				Serial.println( "Error decoding the command..." ); 

				while( bt.available() > 0 ) bt.read();
			}
		}
	}
}

